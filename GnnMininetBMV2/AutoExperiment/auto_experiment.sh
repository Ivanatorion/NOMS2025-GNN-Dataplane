#!/usr/bin/env bash

REPEAT="10"

CFG_p4=("ecn.p4" "noecn.p4" "gnn.p4")
CFG_dir=("iperfLogsECN" "iperfLogsNoECN" "iperfLogsGNN")
CFG_build_time=("12" "12" "35")
CFG_tcp_ecn=("1" "2" "1")
CFG_tcp_congestion=("dctcp" "cubic" "dctcp")

set -xe

rm -rf ../iperfLogs
mkdir ../iperfLogs

for (( k=0; k<3; k++ ))
do
    sudo cp ${CFG_p4[$k]} ../gnn.p4
    cd ..

    sudo sysctl -w net.ipv4.tcp_ecn=${CFG_tcp_ecn[$k]}
    sudo sysctl -w net.ipv4.tcp_congestion_control=${CFG_tcp_congestion[$k]}

    tmux new-session -d -s mininet;

    for (( j=0; j<$REPEAT; j++ ))
    do
        tmux send-keys 'sudo make clean' Enter;
        #tmux send-keys 'p4' Enter;
        sleep 5;
        tmux send-keys 'sudo make run' Enter; sleep ${CFG_build_time[$k]};

        tmux send-keys 'h1 iperf3 -s --one-of -i 60 --logfile iperfLogs/h1.txt &' Enter; sleep 1;
        tmux send-keys 'h2 iperf3 -s --one-of -i 60 --logfile iperfLogs/h2.txt &' Enter; sleep 1;
        tmux send-keys 'h3 iperf3 -s --one-of -i 60 --logfile iperfLogs/h3.txt &' Enter; sleep 1;
        tmux send-keys 'h4 iperf3 -s --one-of -i 60 --logfile iperfLogs/h4.txt &' Enter; sleep 1;

        tmux send-keys "h11 iperf3 -c 10.0.1.1 -t 60 -i 60 -l 64 -C ${CFG_tcp_congestion[$k]} &" Enter; sleep 5;
        tmux send-keys "h12 iperf3 -c 10.0.1.2 -t 50 -i 60 -l 64 -C ${CFG_tcp_congestion[$k]} &" Enter; sleep 5;
        tmux send-keys "h13 iperf3 -c 10.0.1.3 -t 40 -i 60 -l 64 -C ${CFG_tcp_congestion[$k]} &" Enter; sleep 5;
        tmux send-keys "h14 iperf3 -c 10.0.1.4 -t 30 -i 60 -l 64 -C ${CFG_tcp_congestion[$k]} &" Enter;

        sleep 90;

        tmux send-keys 'exit' Enter; sleep 3;
    done
    tmux send-keys 'sudo make clean' Enter; sleep 5;
    tmux kill-session -t mininet;

    cat iperfLogs/h1.txt | grep "receiver" > iperfLogs/host1.txt
    cat iperfLogs/h2.txt | grep "receiver" > iperfLogs/host2.txt
    cat iperfLogs/h3.txt | grep "receiver" > iperfLogs/host3.txt
    cat iperfLogs/h4.txt | grep "receiver" > iperfLogs/host4.txt

    sudo mv iperfLogs/host1.txt iperfLogs/h1.txt
    sudo mv iperfLogs/host2.txt iperfLogs/h2.txt
    sudo mv iperfLogs/host3.txt iperfLogs/h3.txt
    sudo mv iperfLogs/host4.txt iperfLogs/h4.txt

    mkdir iperfLogs/${CFG_dir[$k]}/
    sudo mv iperfLogs/*.txt iperfLogs/${CFG_dir[$k]}/.
    cd AutoExperiment
done

sudo sysctl -w net.ipv4.tcp_ecn=1
sudo sysctl -w net.ipv4.tcp_congestion_control=cubic

sudo python3 calcJain.py ../iperfLogs
