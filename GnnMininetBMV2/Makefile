BUILD_DIR = build
PCAP_DIR = pcaps
LOG_DIR = logs

P4C_ARGS = --p4runtime-files $(BUILD_DIR)/gnn.p4.p4info.txt

source = gnn.p4
compiled_json := $(BUILD_DIR)/gnn.json

all: run

run: build
	sudo python3 utils/run.py -t topology.json -b simple_switch_grpc

stop:
	sudo mn -c

build: dirs $(compiled_json)

$(compiled_json): gnn.p4
	p4c-bm2-ss --p4v 16 $(P4C_ARGS) -o $(compiled_json) $<

dirs:
	mkdir -p $(BUILD_DIR) $(PCAP_DIR) $(LOG_DIR)

clean: stop
	rm -f *.pcap
	rm -rf $(BUILD_DIR) $(PCAP_DIR) $(LOG_DIR)

