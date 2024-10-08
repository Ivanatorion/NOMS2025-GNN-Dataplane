#From P4GNN directory, run: sudo docker build -f Docker/Dockerfile -t p4gnn .
#To run: sudo docker run --privileged -it --network host p4gnn /bin/bash

FROM ubuntu:focal-20221130

RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections && apt-get update \
 && apt-get install -y sudo dialog python3 python3-pip apt-utils tmux iperf3 net-tools nano \
    openvswitch-switch \
    openvswitch-testcontroller \
 && rm -rf /var/lib/apt/lists/* \
 && ln /usr/bin/ovs-testcontroller /usr/bin/controller

RUN adduser --disabled-password --gecos '' docker
RUN adduser docker sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

USER docker

WORKDIR /home/docker/

RUN sudo pip3 install tzdata

#COPY GenerateP4 /home/docker/GenerateP4

COPY P4InstallScriptsUbuntu20 /home/docker/P4InstallScriptsUbuntu20

RUN export DEBIAN_FRONTEND=noninteractive && cp /home/docker/P4InstallScriptsUbuntu20/* /home/docker/. && ./install_dependencies.sh
RUN sudo rm -f mininet-dont-install-python2-2022-apr.patch disable-Wno-error-and-other-small-changes.diff install_dependencies.sh README.md py3localpath.py && sudo rm -rf /home/docker/P4InstallScriptsUbuntu20

COPY GnnMininetBMV2 /home/docker/GnnMininetBMV2
