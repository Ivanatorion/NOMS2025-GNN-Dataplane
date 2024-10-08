#include <stdio.h>
#include <string.h>

void genS1Json(const int nHosts){
    int i;

    FILE *fout = fopen("s1-runtime.json", "w");
    fprintf(fout, "{\n  \"target\": \"bmv2\",\n  \"p4info\": \"build/gnn.p4.p4info.txt\",\n  \"bmv2_json\": \"build/gnn.json\",\n  \"table_entries\": [\n");
    fprintf(fout, "    {\n      \"table\": \"MyIngress.ipv4_forward_table\",\n      \"default_action\": true,\n      \"action_name\": \"MyIngress.drop\",\n");
    fprintf(fout, "      \"action_params\": { }\n    },\n");

    for(i = 0; i < nHosts; i++){
        fprintf(fout, "    {\n      \"table\": \"MyIngress.ipv4_forward_table\",\n      \"match\": {\n        \"hdr.ipv4.dstAddr\": [\"10.0.1.%d\", 32]\n", i+1);
        fprintf(fout, "      },\n      \"action_name\": \"MyIngress.ipv4_forward\",\n      \"action_params\": {\n        \"dstAddr\": \"08:00:00:00:%02x:%02x\",\n", i+1, i+1);
        fprintf(fout, "        \"port\": %d\n      }\n    }", i+1);
       
        fprintf(fout, ",\n");
    }
    for(i = 0; i < nHosts; i++){
        fprintf(fout, "    {\n      \"table\": \"MyIngress.ipv4_forward_table\",\n      \"match\": {\n        \"hdr.ipv4.dstAddr\": [\"10.0.2.%d\", 32]\n", i+1);
        fprintf(fout, "      },\n      \"action_name\": \"MyIngress.ipv4_forward\",\n      \"action_params\": {\n        \"dstAddr\": \"08:00:00:00:%02x:%02x\",\n", i+11, i+11);
        fprintf(fout, "        \"port\": 10\n      }\n    }");
        if(i != nHosts - 1)
            fprintf(fout, ",\n");
        else
            fprintf(fout, "\n");
    }
    fprintf(fout, "  ]\n}\n");
    fclose(fout);
}

void genS2Json(const int nHosts){
    int i;

    FILE *fout = fopen("s2-runtime.json", "w");
    fprintf(fout, "{\n  \"target\": \"bmv2\",\n  \"p4info\": \"build/gnn.p4.p4info.txt\",\n  \"bmv2_json\": \"build/gnn.json\",\n  \"table_entries\": [\n");
    fprintf(fout, "    {\n      \"table\": \"MyIngress.ipv4_forward_table\",\n      \"default_action\": true,\n      \"action_name\": \"MyIngress.drop\",\n");
    fprintf(fout, "      \"action_params\": { }\n    },\n");

    for(i = 0; i < nHosts; i++){
        fprintf(fout, "    {\n      \"table\": \"MyIngress.ipv4_forward_table\",\n      \"match\": {\n        \"hdr.ipv4.dstAddr\": [\"10.0.2.%d\", 32]\n", i+1);
        fprintf(fout, "      },\n      \"action_name\": \"MyIngress.ipv4_forward\",\n      \"action_params\": {\n        \"dstAddr\": \"08:00:00:00:%02x:%02x\",\n", i+11, i+11);
        fprintf(fout, "        \"port\": %d\n      }\n    }", i+1);
       
        fprintf(fout, ",\n");
    }
    for(i = 0; i < nHosts; i++){
        fprintf(fout, "    {\n      \"table\": \"MyIngress.ipv4_forward_table\",\n      \"match\": {\n        \"hdr.ipv4.dstAddr\": [\"10.0.1.%d\", 32]\n", i+1);
        fprintf(fout, "      },\n      \"action_name\": \"MyIngress.ipv4_forward\",\n      \"action_params\": {\n        \"dstAddr\": \"08:00:00:00:%02x:%02x\",\n", i+1, i+1);
        fprintf(fout, "        \"port\": 10\n      }\n    }");
        if(i != nHosts - 1)
            fprintf(fout, ",\n");
        else
            fprintf(fout, "\n");
    }
    fprintf(fout, "  ]\n}\n");
    fclose(fout);
}

void genSwitchCommands(const int nSwitches){
    int i;
    char buffer[256];
    for(i = 0; i < nSwitches; i++){
        sprintf(buffer, "s%d-commands.txt", i+1);
        FILE *fout = fopen(buffer, "w");

        fprintf(fout, "mirroring_add 500 0\n");
        fprintf(fout, "mc_node_create 1 2\n");
        fprintf(fout, "mc_node_create 1 3\n");
        fprintf(fout, "mc_mgrp_create 1\n");
        fprintf(fout, "mc_node_associate 1 0\n");
        fprintf(fout, "mc_node_associate 1 1\n");

        fclose(fout);
    }
}

int main(int argc, char *argv[]){
    int nHosts = 4;
    int i;
    
    if(nHosts > 8){
        fprintf(stderr, "Error: Max hosts is 8\n");
        return 1;
    }

    FILE *fout = fopen("topology.json", "w");

    fprintf(fout, "{\n    \"hosts\": {\n");
    for(i = 0; i < nHosts; i++){
        fprintf(fout, "        \"h%d\": {\"ip\": \"10.0.1.%d/24\", \"mac\": \"08:00:00:00:%02x:%02x\",\n", i+1, i+1, i+1, i+1);
        fprintf(fout, "               \"commands\":[\"route add default gw 10.0.1.10 dev eth0\",\n");
        fprintf(fout, "                           \"arp -i eth0 -s 10.0.1.10 08:00:00:00:01:00\"]},\n"); 
    }
    for(i = 0; i < nHosts; i++){
        fprintf(fout, "        \"h%d\": {\"ip\": \"10.0.2.%d/24\", \"mac\": \"08:00:00:00:%02x:%02x\",\n", i+11, i+1, i+11, i+11);
        fprintf(fout, "               \"commands\":[\"route add default gw 10.0.2.20 dev eth0\",\n");
        fprintf(fout, "                           \"arp -i eth0 -s 10.0.2.20 08:00:00:00:02:00\"]}");

        if(i == nHosts - 1)
            fprintf(fout, "\n");
        else
            fprintf(fout, ",\n"); 
    }
    fprintf(fout, "    },\n");

    fprintf(fout, "    \"switches\": {\n");
    fprintf(fout, "        \"s1\": {\n            \"cli_input\" : \"s1-commands.txt\",\n            \"runtime_json\" : \"s1-runtime.json\"\n        },\n");
    fprintf(fout, "        \"s2\": {\n            \"cli_input\" : \"s2-commands.txt\",\n            \"runtime_json\" : \"s2-runtime.json\"\n        }\n    },\n");

    fprintf(fout, "    \"links\": [\n        [\"s1-p10\", \"s2-p10\", \"0\", 2.0],\n       ");

    for(i = 0; i < nHosts; i++){
        fprintf(fout, " [\"h%d\", \"s1-p%d\", \"0\", 1.0],", i+1, i+1);
    }
    fprintf(fout, "\n       ");
    for(i = 0; i < nHosts; i++){
        fprintf(fout, " [\"h%d\", \"s2-p%d\", \"0\", 1.0]", i+11, i+1);
        if(i != nHosts - 1)
            fprintf(fout, ",");
    }
    fprintf(fout, "\n    ]\n");

    fprintf(fout, "}\n");
    fclose(fout);

    genS1Json(nHosts);
    genS2Json(nHosts);

    genSwitchCommands(2);

    return 0;
}