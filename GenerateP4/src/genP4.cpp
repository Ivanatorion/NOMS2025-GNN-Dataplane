#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <cstdint>
#include <vector>

typedef uint32_t FixedPointT;

enum Activation {NONE = 0, RELU, SIGMOID};

FixedPointT convert(const double input, const int precision){
    return (FixedPointT)(input * (1 << precision));
}

class NeuralNetworkMatrix{
    public:
        NeuralNetworkMatrix(int lines, int cols) {this->weights.resize(lines); for(int i = 0; i < lines; i++) this->weights[i].resize(cols, 0.0);}

        void SetAt(int i, int j, double value) {if (i < (int) this->weights.size() && j < (int) this->weights[i].size()) this->weights[i][j] = value;}
        double GetAt(int i, int j) const {return (i < (int) this->weights.size() && j < (int) this->weights[i].size()) ? this->weights[i][j] : 0.0;}

        int GetNLines() const {return (int) this->weights.size();}
        int GetNCols() const {return (int) this->weights[0].size();}

        NeuralNetworkMatrix operator * (NeuralNetworkMatrix const &mtx){
            NeuralNetworkMatrix result((int) this->weights.size(), (int) mtx.weights[0].size());
            for(int ir = 0; ir < (int) result.weights.size(); ir++){
                for(int jr = 0; jr < (int) result.weights[ir].size(); jr++){
                    result.weights[ir][jr] = 0.0;
                    for(int k = 0; k < (int) mtx.weights.size(); k++){
                        result.weights[ir][jr] = result.weights[ir][jr] + this->weights[ir][k] * mtx.weights[k][jr];
                    }
                }
            }

            return result;
        }

        void Print(){
            for(int i = 0; i < (int) this->weights.size(); i++){
                for(int j = 0; j < (int) this->weights[i].size(); j++){
                    printf("%.4f ", this->weights[i][j]);
                }
                printf("\n");
            }
        }

    private:
        std::vector<std::vector<double>> weights;
};

class NeuralNetworkMatrixFixedPoint{
    public:
        NeuralNetworkMatrixFixedPoint(int lines, int cols) {this->weights.resize(lines); for(int i = 0; i < lines; i++) this->weights[i].resize(cols, 0.0);}
        NeuralNetworkMatrixFixedPoint(const NeuralNetworkMatrix& nnm, const int precision){
            const int lines = nnm.GetNLines();
            const int cols = nnm.GetNCols();
            this->weights.resize(lines);
            for(int i = 0; i < lines; i++) 
                this->weights[i].resize(cols, 0.0);

            for(int i = 0; i < lines; i++){
                for(int j = 0; j < cols; j++){
                    this->SetAt(i, j, convert(nnm.GetAt(i, j), precision));
                }
            }
        }

        void SetAt(int i, int j, FixedPointT value) {if (i < (int) this->weights.size() && j < (int) this->weights[i].size()) this->weights[i][j] = value;}
        FixedPointT GetAt(int i, int j) const {return (i < (int) this->weights.size() && j < (int) this->weights[i].size()) ? this->weights[i][j] : 0.0;}

        int GetNLines() const {return (int) this->weights.size();}
        int GetNCols() const {return (int) this->weights[0].size();}

        void Print(){
            for(int i = 0; i < (int) this->weights.size(); i++){
                for(int j = 0; j < (int) this->weights[i].size(); j++){
                    printf("%d ", this->weights[i][j]);
                }
                printf("\n");
            }
        }

    private:
        std::vector<std::vector<uint32_t>> weights;
};

class NeuralNetwork{
    public:
        NeuralNetwork(){}

        void AddLayerMatrix(const NeuralNetworkMatrix& m, const NeuralNetworkMatrix& bias, const Activation a) {this->m_layers.push_back(m); this->m_biases.push_back(bias); this->m_activations.push_back(a);}

        NeuralNetworkMatrix Apply(NeuralNetworkMatrix input){
            for(int i = 0; i < (int) this->m_layers.size(); i++){
                input = input * this->m_layers[i];
                for(int j = 0; j < this->m_biases[i].GetNCols(); j++){
                    input.SetAt(0, j, input.GetAt(0, j) + this->m_biases[i].GetAt(0, j));
                    if(this->m_activations[i] == SIGMOID)
                        input.SetAt(0, j, 1.0/(1.0 + exp(-input.GetAt(0, j))));
                    else if(this->m_activations[i] == RELU && input.GetAt(0, j) < 0.0)
                        input.SetAt(0, j, 0.0);
                }
            }
            return input;
        }

        NeuralNetworkMatrix Apply(std::vector<double> input){
            NeuralNetworkMatrix m(1, (int) input.size());
            for(int i = 0; i < (int) input.size(); i++)
                m.SetAt(0, i, input[i]);
            return this->Apply(m);
        }

        int GetNLayers(){return (int) this->m_layers.size();}

        std::vector<NeuralNetworkMatrixFixedPoint> getLayersFixedPoint(const int precision) const{
            std::vector<NeuralNetworkMatrixFixedPoint> result;

            for(const NeuralNetworkMatrix& layer : m_layers){
                result.push_back(NeuralNetworkMatrixFixedPoint(layer, precision));
            }

            return result; 
        }

        std::vector<NeuralNetworkMatrixFixedPoint> getBiasesFixedPoint(const int precision) const{
            std::vector<NeuralNetworkMatrixFixedPoint> result;

            for(const NeuralNetworkMatrix& bias : m_biases){
                result.push_back(NeuralNetworkMatrixFixedPoint(bias, precision));
            }

            return result; 
        }

        std::vector<NeuralNetworkMatrix> getLayers() const{
            return this->m_layers;
        }

    private:
        std::vector<NeuralNetworkMatrix> m_layers;
        std::vector<NeuralNetworkMatrix> m_biases;
        std::vector<Activation> m_activations;
};

void parseArgs(int argc, char *argv[], bool *help, char modelfFilePath[], char outFilePath[], int *precision){
    int i = 1;
    while(i < argc){
        if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
            *help = true;
        }
        else if(!strcmp(argv[i], "-f") || !strcmp(argv[i], "--file")){
            i++;
            if(i < argc){
                strcpy(modelfFilePath, argv[i]);
            }
        }
        else if(!strcmp(argv[i], "-o") || !strcmp(argv[i], "--out")){
            i++;
            if(i < argc){
                strcpy(outFilePath, argv[i]);
            }
        }
        else if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--precision")){
            i++;
            if(i < argc){
                *precision = atoi(argv[i]);
            }
        }

        i++;
    }
}

void showHelp(char pName[]){
    printf("Usage: %s [OPTIONS]\n\n", pName);
    printf("-h        | --help           : Displays help message\n");
    printf("-f [PATH] | --file [PATH]    : Specify model weights input file\n");
    printf("-o [PATH] | --out  [PATH]    : Specify the p4 output file\n");
    printf("-p [N]    | --precision [N]  : Specify fixed point precision\n");
}

typedef struct nn3{
    std::vector<NeuralNetworkMatrixFixedPoint> msgW;
    std::vector<NeuralNetworkMatrixFixedPoint> msgB;
    std::vector<NeuralNetworkMatrixFixedPoint> roW;
    std::vector<NeuralNetworkMatrixFixedPoint> roB;
    std::vector<NeuralNetworkMatrixFixedPoint> uptW;
    std::vector<NeuralNetworkMatrixFixedPoint> uptB;

    int nIterations;
    int hiddenStateDim;
} NN3;

NN3 getLayersAsFixedPoint(const int precision, const char filePath[]){
    char buffer[256];
    NN3 result;

    FILE *fp = fopen(filePath, "r");
    if(!fp){
        fprintf(stderr, "Could not open file: %s\n", filePath);
        exit(1);
    }

    int rb = 0;

    NeuralNetwork msgNN;
    NeuralNetwork roNN;
    NeuralNetwork upNN;

    int msgLayers, aggLayers, uptLayers, readoutLayers;
    
    rb += fscanf(fp, "%d %d %d %d\n", &uptLayers, &aggLayers, &msgLayers, &readoutLayers);

    rb += fscanf(fp, "%d\n%s\n%d\n", &result.nIterations, buffer, &result.hiddenStateDim);
    //TODO: buffer is aggregation type

    double wgt;

    std::vector<Activation> activations;
    int activationsIndex = 0;
    for(int i = 0; i < msgLayers + aggLayers + uptLayers + readoutLayers; i++){
        rb += fscanf(fp, "%s\n", buffer);
        if(!strcmp(buffer, "None"))
        activations.push_back(NONE);
        else if(!strcmp(buffer, "sigmoid"))
        activations.push_back(SIGMOID);
        else if(!strcmp(buffer, "relu"))
        activations.push_back(RELU);
        else{
        fprintf(stderr, "Error: Unknown layer activation \"%s\"\n", buffer);
        exit(1);
        }
    }

    for(int i = 0; i < uptLayers; i++){
        
        int l, c;
        rb += fscanf(fp, "%d %d\n", &l, &c);
        NeuralNetworkMatrix layer(l, c);
        for(int j = 0; j < l; j++){
        for(int k = 0; k < c; k++){
            rb += fscanf(fp, "%lf ", &wgt);
            layer.SetAt(j, k, wgt);
        }
        }

        rb += fscanf(fp, "%d\n", &c);
        NeuralNetworkMatrix bias(1, c);
        for(int k = 0; k < c; k++){
        rb += fscanf(fp, "%lf ", &wgt);
        bias.SetAt(0, k, wgt);
        }

        upNN.AddLayerMatrix(layer, bias, activations[activationsIndex]);
        activationsIndex++;
    }

    for(int i = 0; i < msgLayers; i++){
        
        int l, c;
        rb += fscanf(fp, "%d %d\n", &l, &c);
        NeuralNetworkMatrix layer(l, c);
        for(int j = 0; j < l; j++){
        for(int k = 0; k < c; k++){
            rb += fscanf(fp, "%lf ", &wgt);
            layer.SetAt(j, k, wgt);
        }
        }

        rb += fscanf(fp, "%d\n", &c);
        NeuralNetworkMatrix bias(1, c);
        for(int k = 0; k < c; k++){
        rb += fscanf(fp, "%lf ", &wgt);
        bias.SetAt(0, k, wgt);
        }

        msgNN.AddLayerMatrix(layer, bias, activations[activationsIndex]);
        activationsIndex++;
    }

    for(int i = 0; i < readoutLayers; i++){
        
        int l, c;
        rb += fscanf(fp, "%d %d\n", &l, &c);
        NeuralNetworkMatrix layer(l, c);
        for(int j = 0; j < l; j++){
        for(int k = 0; k < c; k++){
            rb += fscanf(fp, "%lf ", &wgt);
            layer.SetAt(j, k, wgt);
        }
        }

        rb += fscanf(fp, "%d\n", &c);
        NeuralNetworkMatrix bias(1, c);
        for(int k = 0; k < c; k++){
        rb += fscanf(fp, "%lf ", &wgt);
        bias.SetAt(0, k, wgt);
        }

        roNN.AddLayerMatrix(layer, bias, activations[activationsIndex]);
        activationsIndex++;
    }

    fclose(fp);

    result.msgW = msgNN.getLayersFixedPoint(precision);
    result.msgB = msgNN.getBiasesFixedPoint(precision);
    result.roW = roNN.getLayersFixedPoint(precision);
    result.roB = roNN.getBiasesFixedPoint(precision);
    result.uptW = upNN.getLayersFixedPoint(precision);
    result.uptB = upNN.getBiasesFixedPoint(precision);
    return result;
}

void getBinaryString(char destBuffer[], const FixedPointT value){
    destBuffer[0] = '0';
    destBuffer[1] = 'b';
    destBuffer[34] = '\0';

    FixedPointT mask = (1 << ((sizeof(FixedPointT) * 8) - 1));

    for(int i = 0; i < 32; i++){
        destBuffer[i+2] = ((mask & value) == 0) ? '0' : '1';
        mask = mask >> 1;
    }
}

void genHeaders(const int featureVecLen, FILE *out){
    fprintf(out, "/*************** H E A D E R S ***************/\n\n");
    fprintf(out, "typedef bit<9>  egressSpec_t;\ntypedef bit<48> macAddr_t;\ntypedef bit<32> ip4Addr_t;\n\n");
    fprintf(out, "header ethernet_t {\n    macAddr_t dstAddr;\n    macAddr_t srcAddr;\n    bit<16> etherType;\n}\n\n");
    fprintf(out, "header ipv4_t {\n    bit<4> version;\n    bit<4> ihl;\n    bit<6> diffserv;\n    bit<2> ecn;\n    bit<16> totalLen;\n    bit<16> identification;\n");
    fprintf(out, "    bit<3> flags;\n    bit<13> fragOffset;\n    bit<8> ttl;\n    bit<8> protocol;\n    bit<16> hdrChecksum;\n    ip4Addr_t srcAddr;\n    ip4Addr_t dstAddr;\n}\n\n");
    
    fprintf(out, "struct gnn_feature_vec {\n");
    for(int i = 0; i < featureVecLen; i++)
        fprintf(out, "    WORD feature%d;\n", i);

    fprintf(out, "}\n\nheader gnn_t {\n");
    fprintf(out, "    gnn_feature_vec features;\n");
    fprintf(out, "    WORD iteration;\n}\n\n");

    fprintf(out, "struct metadata {\n    /* empty */\n}\n\nstruct headers {\n    ethernet_t ethernet;\n    ipv4_t ipv4;\n    gnn_t gnn;\n}\n\n");
}

void genAuxFunctions(FILE *out, const int precision){
    char buffer[256];

    fprintf(out, "/************** A U X   F U N C T I O N S **************/\n\n");
    fprintf(out, "WORD FPMult(in WORD opA, in WORD opB) {\n");
    fprintf(out, "    DWORD opAD = (DWORD) opA;\n    DWORD opBD = (DWORD) opB;\n");
    fprintf(out, "    if (opAD & (1 << (WORDSIZE-1)) != 0) {\n");
    fprintf(out, "        opAD = opAD + DWORD_NEGATIVE_PADDING;\n    }\n");
    fprintf(out, "    if (opBD & (1 << (WORDSIZE-1)) != 0) {\n");
    fprintf(out, "        opBD = opBD + DWORD_NEGATIVE_PADDING;\n    }\n\n");
    fprintf(out, "    DWORD result = ((opAD * opBD) >> PRECISION);\n");
    fprintf(out, "    return (WORD) result;\n");
    fprintf(out, "}\n\n");

    fprintf(out, "WORD FixedDivision(in WORD numerator, in WORD denominator) {\n");
    fprintf(out, "    if (denominator == 2){\n        return numerator >> 1;\n    }\n");

    getBinaryString(buffer, convert(0.333333, precision));
    fprintf(out, "    else if (denominator == 3){\n        return FPMult(numerator, %s); // * (0.333333)\n    }\n", buffer);
    fprintf(out, "    else if (denominator == 4){\n        return numerator >> 2;\n    }\n");

    getBinaryString(buffer, convert(0.2, precision));
    fprintf(out, "    else if (denominator == 5){\n        return FPMult(numerator, %s); // * (0.2)\n    }\n", buffer);
    fprintf(out, "\n    return numerator;\n");
    fprintf(out, "}\n\n");

    fprintf(out, "WORD RELu(in WORD op) {\n");
    fprintf(out, "    if (op & (1 << (WORDSIZE-1)) != 0) {\n        return 0;\n    }\n");
    fprintf(out, "    return op;\n");
    fprintf(out, "}\n\n");

    fprintf(out, "bit<1> BooleanSigmoid(in WORD op) {\n");
    fprintf(out, "    if (op & (1 << (WORDSIZE-1)) != 0) {\n        return 0;\n    }\n");
    fprintf(out, "    return 1;\n");
    fprintf(out, "}\n\n");
}

void genGNNUpdate(const int featureVectorLen, FILE *out){
    fprintf(out, "gnn_feature_vec GNNUpdate() {\n");
    fprintf(out, "    gnn_feature_vec MyFV;\n    gnn_feature_vec MyFA;\n\n");

    for(int i = 0; i < featureVectorLen; i++)
        fprintf(out, "    RegFV.read(MyFV.feature%d, %d);\n", i, i);
    fprintf(out, "\n");
    for(int i = 0; i < featureVectorLen; i++)
        fprintf(out, "    RegFACC.read(MyFA.feature%d, %d);\n", i, i);
    fprintf(out, "\n");

    fprintf(out, "    WORD receivedThisIT;\n    RegReceivedThisIteration.read(receivedThisIT, 0);\n");

    for(int i = 0; i < featureVectorLen; i++){
        fprintf(out, "    if (MyFA.feature%d & (1 << (WORDSIZE - 1)) != 0) { //Negative number\n", i);
        fprintf(out, "        MyFA.feature%d = ~ MyFA.feature%d + 1;\n", i, i);
        fprintf(out, "        MyFA.feature%d = FixedDivision(MyFA.feature%d, receivedThisIT);\n", i, i);
        fprintf(out, "        MyFA.feature%d = ~ MyFA.feature%d + 1;\n", i, i);
        fprintf(out, "    } else {\n        MyFA.feature%d = FixedDivision(MyFA.feature%d, receivedThisIT);\n    }\n\n", i, i);
    }

    fprintf(out, "    // MyFA will contain updated feature vector\n    NN_UPD_Apply(MyFV, MyFA);\n\n    return MyFA;\n}\n\n");
}

void genParser(FILE *out){
    fprintf(out, "/*************** P A R S E R ***************/\n\n");
    fprintf(out, "parser MyParser(packet_in packet, out headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {\n");
    fprintf(out, "    state start {\n        transition parse_ethernet;\n    }\n\n    state parse_ethernet {\n        packet.extract(hdr.ethernet);\n\n");
    fprintf(out, "        transition select(hdr.ethernet.etherType) {\n            TYPE_IPV4: parse_ipv4;\n            TYPE_GNN:  parse_gnn;\n            default:   accept;\n");
    fprintf(out, "        }\n    }\n\n    state parse_ipv4 {\n        packet.extract(hdr.ipv4);\n        transition accept;\n    }\n\n    state parse_gnn {\n        packet.extract(hdr.gnn);\n        transition accept;\n    }\n}\n\n");
}

void genVerifyChecksum(FILE *out){
    fprintf(out, "/********** C H E C K S U M    V E R I F I C A T I O N **********/\n\n");
    fprintf(out, "control MyVerifyChecksum(inout headers hdr, inout metadata meta) {\n    apply { }\n");
    fprintf(out, "}\n\n");
}

void genIngress(const int featureVectorLen, FILE *out){
    fprintf(out, "/************ I N G R E S S   P R O C E S S I N G ************/\n\n");
    fprintf(out, "control MyIngress(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {\n");

    fprintf(out, "\n    action drop() {\n        mark_to_drop(standard_metadata);\n    }\n\n");
    fprintf(out, "    action ipv4_forward(macAddr_t dstAddr, egressSpec_t port) {\n        standard_metadata.egress_spec = port;\n        hdr.ethernet.srcAddr = hdr.ethernet.dstAddr;\n");
    fprintf(out, "        hdr.ethernet.dstAddr = dstAddr;\n        hdr.ipv4.ttl = hdr.ipv4.ttl - 1;\n    }\n\n");

    fprintf(out, "    action send_gnn_packets() {\n        const bit<32> REPORT_MIRROR_SESSION_ID = 500;\n        clone(CloneType.I2E, REPORT_MIRROR_SESSION_ID);\n    }\n\n");

    fprintf(out, "    table ipv4_forward_table {\n        key = {\n            hdr.ipv4.dstAddr: lpm;\n        }\n        actions = {\n            ipv4_forward;\n            drop;\n");
    fprintf(out, "            NoAction;\n        }\n        size = 1024;\n        default_action = drop();\n    }\n\n");

    fprintf(out, "    apply {\n");
    fprintf(out, "        if (hdr.ipv4.isValid()) {\n            ipv4_forward_table.apply();\n\n");
    fprintf(out, "            @atomic {\n\n");
    
    fprintf(out, "            bit<48> MyLastTS;\n            RegLastTimestamp.read(MyLastTS, 0);\n\n");

    fprintf(out, "            if (standard_metadata.ingress_global_timestamp - GNN_ITERATION_PERIOD > MyLastTS) {\n");
    fprintf(out, "                RegLastTimestamp.write(0, standard_metadata.ingress_global_timestamp);\n\n");
    fprintf(out, "                WORD iterationSendVal;\n                RegIteration.read(iterationSendVal, 1);\n\n");
    fprintf(out, "                RegIteration.write(0, iterationSendVal);\n                iterationSendVal = iterationSendVal + 1;\n                RegIteration.write(1, iterationSendVal);\n\n");
    fprintf(out, "                gnn_feature_vec MyFA;\n                MyFA = GNNUpdate();\n\n");
    fprintf(out, "                if (((iterationSendVal + 1) %% N_GNN_ITERATIONS) == 0) {\n");
    fprintf(out, "                    bit<1> resultRO = NN_RO_Apply(MyFA);\n\n");
    fprintf(out, "                    RegGNNResult.write(0, resultRO);\n");
    fprintf(out, "                    //TODO: Reset RegFV;\n");
    fprintf(out, "                } else {\n");
    for(int i = 0; i < featureVectorLen; i++)
        fprintf(out, "                    RegFV.write(%d, MyFA.feature%d);\n", i, i);
    fprintf(out, "                }\n\n");
    for(int i = 0; i < featureVectorLen; i++)
        fprintf(out, "                RegFACC.write(%d, MyFA.feature%d);\n", i, i);
    fprintf(out, "\n                RegReceivedThisIteration.write(0, 0);\n\n                send_gnn_packets();\n            }\n            }\n");

    fprintf(out, "        } else if (hdr.gnn.isValid()) {\n");
    fprintf(out, "            @atomic {\n\n");
    fprintf(out, "            if (standard_metadata.instance_type == PKT_INSTANCE_TYPE_INGRESS_RECIRC) {\n                standard_metadata.mcast_grp = 1;\n            } else {\n");
    fprintf(out, "                WORD iterationReceiveVal;\n                WORD iterationSendVal;\n");
    fprintf(out, "                RegIteration.read(iterationReceiveVal, 0);\n                RegIteration.read(iterationSendVal, 1);\n\n");
    fprintf(out, "                if (hdr.gnn.iteration == iterationReceiveVal) {\n");
    fprintf(out, "                    gnn_feature_vec MyFV;\n");
    for(int i = 0; i < featureVectorLen; i++)
        fprintf(out, "                    RegFV.read(MyFV.feature%d, %d);\n", i, i);
    fprintf(out, "\n                    // hdr.gnn.features gets updated here\n                    NN_MSG_Apply(MyFV, hdr.gnn.features);\n\n");
    fprintf(out, "                    // Accumulate\n                    WORD feature;\n");
    for(int i = 0; i < featureVectorLen; i++){
        fprintf(out, "                    RegFACC.read(feature, %d);\n", i);
        fprintf(out, "                    RegFACC.write(%d, feature + hdr.gnn.features.feature%d);\n", i, i);
    }
    fprintf(out, "\n                    WORD receivedThisIT;\n");
    fprintf(out, "                    RegReceivedThisIteration.read(receivedThisIT, 0);\n");
    fprintf(out, "                    RegReceivedThisIteration.write(0, receivedThisIT + 1);\n\n");

    fprintf(out, "                    // If we did not send our packets yet, send now\n");
    fprintf(out, "                    if (iterationReceiveVal == iterationSendVal) {\n");
    fprintf(out, "                        RegIteration.write(1, iterationSendVal + 1);\n                        send_gnn_packets();\n");
    fprintf(out, "                    }\n                } else if (hdr.gnn.iteration < iterationReceiveVal) {\n                    // Ignore earlier iteration messages\n");
    fprintf(out, "                } else { //hdr.gnn.iteration > iterationReceiveVal\n");

    fprintf(out, "                    iterationReceiveVal = hdr.gnn.iteration;\n                    iterationSendVal = hdr.gnn.iteration + 1;\n");
    fprintf(out, "                    RegIteration.write(0, iterationReceiveVal);\n                    RegIteration.write(1, iterationSendVal);\n\n");
    fprintf(out, "                    gnn_feature_vec MyFA;\n                    gnn_feature_vec MyFV;\n                    MyFA = GNNUpdate();\n\n");
    fprintf(out, "                    if (((iterationSendVal + 1) %% N_GNN_ITERATIONS) == 0) {\n");
    fprintf(out, "                        bit<1> resultRO = NN_RO_Apply(MyFA);\n\n");
    fprintf(out, "                        RegGNNResult.write(0, resultRO);\n");
    fprintf(out, "                        //TODO: Reset RegFV (and MyFV);\n");
    fprintf(out, "                    } else {\n");
    for(int i = 0; i < featureVectorLen; i++)
        fprintf(out, "                        RegFV.write(%d, MyFA.feature%d);\n", i, i);
    fprintf(out, "\n");
    for(int i = 0; i < featureVectorLen; i++)
        fprintf(out, "                        MyFV.feature%d = MyFA.feature%d;\n", i, i);
    fprintf(out, "                    }\n\n");
    fprintf(out, "                    NN_MSG_Apply(MyFV, hdr.gnn.features);\n\n");
    for(int i = 0; i < featureVectorLen; i++)
        fprintf(out, "                    RegFACC.write(%d, hdr.gnn.features.feature%d);\n", i, i);
    fprintf(out, "\n                    RegReceivedThisIteration.write(0, 1);\n\n                    send_gnn_packets();\n");
    fprintf(out, "                }\n            }\n\n            }\n        }\n    }\n}\n\n");
}

int getPacketLen(const int featureVectorLen){
    //Ethernet header + iterationN + 4 * featureVecLen
    int result = 14 + 4 + 4 * featureVectorLen;
    while(result % 32 != 0)
        result++;
    return result;
}

void genEgress(const int featureVectorLen, FILE *out){
    fprintf(out, "/************* E G R E S S   P R O C E S S I N G *************/\n\n");
    fprintf(out, "control MyEgress(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {\n\n");
    fprintf(out, "    action change_to_gnn() {\n");
    fprintf(out, "        hdr.ethernet.etherType = TYPE_GNN;\n        hdr.gnn.setValid();\n        hdr.ipv4.setInvalid();\n\n");
    for(int i = 0; i < featureVectorLen; i++)
        fprintf(out, "        RegFV.read(hdr.gnn.features.feature%d, %d);\n", i, i);
    fprintf(out, "\n        RegIteration.read(hdr.gnn.iteration, 0);\n    }\n\n");

    fprintf(out, "    apply {\n");
    fprintf(out, "        // Check if this is a cloned packet used to trigger multicast via recirculate\n");
    fprintf(out, "        if (standard_metadata.instance_type == PKT_INSTANCE_TYPE_INGRESS_CLONE) {\n");
    fprintf(out, "            change_to_gnn();\n            recirculate_preserving_field_list(0);\n        }\n\n");
    fprintf(out, "        if (hdr.gnn.isValid()) {\n            truncate(%d);\n        }\n", getPacketLen(featureVectorLen));
    fprintf(out, "    }\n}\n\n");
}

void genComputeChecksum(FILE *out){
    fprintf(out, "/********** C H E C K S U M    C O M P U T A T I O N **********/\n\n");
    fprintf(out, "control MyComputeChecksum(inout headers hdr, inout metadata meta) {\n    apply {\n");

    fprintf(out, "        update_checksum(\n            hdr.ipv4.isValid(),\n");
    fprintf(out, "            { hdr.ipv4.version,\n");
    fprintf(out, "              hdr.ipv4.ihl,\n");
    fprintf(out, "              hdr.ipv4.diffserv,\n");
    fprintf(out, "              hdr.ipv4.ecn,\n");
    fprintf(out, "              hdr.ipv4.totalLen,\n");
    fprintf(out, "              hdr.ipv4.identification,\n");
    fprintf(out, "              hdr.ipv4.flags,\n");
    fprintf(out, "              hdr.ipv4.fragOffset,\n");
    fprintf(out, "              hdr.ipv4.ttl,\n");
    fprintf(out, "              hdr.ipv4.protocol,\n");
    fprintf(out, "              hdr.ipv4.srcAddr,\n");
    fprintf(out, "              hdr.ipv4.dstAddr },\n");
    fprintf(out, "            hdr.ipv4.hdrChecksum,\n            HashAlgorithm.csum16);\n    }\n}\n\n");
}

void genDeparser(FILE *out){
    fprintf(out, "/************* D E P A R S E R *************/\n\n");
    fprintf(out, "control MyDeparser(packet_out packet, in headers hdr) {\n    apply {\n        packet.emit(hdr.ethernet);\n        packet.emit(hdr.ipv4);\n        packet.emit(hdr.gnn);\n    }\n}\n\n");
}

void genSwitch(FILE *out){
    fprintf(out, "/****************** S W I T C H ******************/\n\n");
    fprintf(out, "V1Switch(\nMyParser(),\nMyVerifyChecksum(),\nMyIngress(),\nMyEgress(),\nMyComputeChecksum(),\nMyDeparser()\n) main;\n");
}

int main(int argc, char *argv[]){
    int precision = 24;
    int gnnIterationPeriod = 100 * 1000; //microsseconds
    int featureVecLen = 4;

    char modelfFilePath[512] = "NO_INPUT";
    char outFilePath[512] = "NO_OUT";
    bool help = false;

    parseArgs(argc, argv, &help, modelfFilePath, outFilePath, &precision);

    if(help){
        showHelp(argv[0]);
        return 0;
    }

    if(!strcmp(modelfFilePath, "NO_INPUT")){
        fprintf(stderr, "No input file\n");
        return 1;
    }

    NN3 layers = getLayersAsFixedPoint(precision, modelfFilePath);
    featureVecLen = layers.hiddenStateDim;

    FILE *of;
    if(!strcmp(outFilePath, "NO_OUT"))
        of = stdout;
    else{
        of = fopen(outFilePath, "w");
        if(!of){
            fprintf(stderr, "Could not open file: %s\n", outFilePath);
            return 1;
        }
    }

    fprintf(of, "#include <core.p4>\n#include <v1model.p4>\n\n");

    fprintf(of, "/* Packet types as in the p4c specification */\n");
    fprintf(of, "#define PKT_INSTANCE_TYPE_NORMAL 0\n#define PKT_INSTANCE_TYPE_INGRESS_CLONE 1\n#define PKT_INSTANCE_TYPE_EGRESS_CLONE 2\n");
    fprintf(of, "#define PKT_INSTANCE_TYPE_COALESCED 3\n#define PKT_INSTANCE_TYPE_INGRESS_RECIRC 4\n#define PKT_INSTANCE_TYPE_REPLICATION 5\n#define PKT_INSTANCE_TYPE_RESUBMIT 6\n\n");

    fprintf(of, "#define PRECISION %d\n#define FEATURE_VEC_LEN %d\n\n", precision, featureVecLen);
    fprintf(of, "#define WORDSIZE 32\n#define D_WORDSIZE 64\n#define DWORD_NEGATIVE_PADDING 0b1111111111111111111111111111111100000000000000000000000000000000\n\n");
    fprintf(of, "typedef bit<WORDSIZE> WORD;\ntypedef bit<D_WORDSIZE> DWORD;\n\n");
    fprintf(of, "const bit<48> GNN_ITERATION_PERIOD = %d;\nconst WORD N_GNN_ITERATIONS = %d;\n\nconst bit<16> TYPE_IPV4 = 0x800;\nconst bit<16> TYPE_GNN  = 0x801;\n\n", gnnIterationPeriod, layers.nIterations);

    for(int ii = 0; ii < (int) layers.msgW.size(); ii++){
        NeuralNetworkMatrixFixedPoint* layer = &layers.msgW[ii];
        for(int i = 0; i < layer->GetNCols(); i++){
            for(int j = 0; j < layer->GetNLines(); j++){
                char buffer[64];
                getBinaryString(buffer, layer->GetAt(j, i));
                fprintf(of, "const WORD MSG_W_L%d_N%d_P%d = %s;\n", ii, i, j, buffer);
            }
            fprintf(of, "\n");
        }
    }

    for(int ii = 0; ii < (int) layers.msgB.size(); ii++){
        NeuralNetworkMatrixFixedPoint* layer = &layers.msgB[ii];
        for(int i = 0; i < layer->GetNCols(); i++){
            for(int j = 0; j < layer->GetNLines(); j++){
                char buffer[64];
                getBinaryString(buffer, layer->GetAt(j, i));
                fprintf(of, "const WORD MSG_B_L%d_N%d_P%d = %s;\n", ii, i, j, buffer);
            }
        }
        fprintf(of, "\n");
    }

    for(int ii = 0; ii < (int) layers.uptW.size(); ii++){
        NeuralNetworkMatrixFixedPoint* layer = &layers.uptW[ii];
        for(int i = 0; i < layer->GetNCols(); i++){
            for(int j = 0; j < layer->GetNLines(); j++){
                char buffer[64];
                getBinaryString(buffer, layer->GetAt(j, i));
                fprintf(of, "const WORD UPD_W_L%d_N%d_P%d = %s;\n", ii, i, j, buffer);
            }
            fprintf(of, "\n");
        }
    }

    for(int ii = 0; ii < (int) layers.uptB.size(); ii++){
        NeuralNetworkMatrixFixedPoint* layer = &layers.uptB[ii];
        for(int i = 0; i < layer->GetNCols(); i++){
            for(int j = 0; j < layer->GetNLines(); j++){
                char buffer[64];
                getBinaryString(buffer, layer->GetAt(j, i));
                fprintf(of, "const WORD UPD_B_L%d_N%d_P%d = %s;\n", ii, i, j, buffer);
            }
        }
        fprintf(of, "\n");
    }

    for(int ii = 0; ii < (int) layers.roW.size(); ii++){
        NeuralNetworkMatrixFixedPoint* layer = &layers.roW[ii];
        for(int i = 0; i < layer->GetNCols(); i++){
            for(int j = 0; j < layer->GetNLines(); j++){
                char buffer[64];
                getBinaryString(buffer, layer->GetAt(j, i));
                fprintf(of, "const WORD RO_W_L%d_N%d_P%d = %s;\n", ii, i, j, buffer);
            }
            fprintf(of, "\n");
        }
    }

    for(int ii = 0; ii < (int) layers.roB.size(); ii++){
        NeuralNetworkMatrixFixedPoint* layer = &layers.roB[ii];
        for(int i = 0; i < layer->GetNCols(); i++){
            for(int j = 0; j < layer->GetNLines(); j++){
                char buffer[64];
                getBinaryString(buffer, layer->GetAt(j, i));
                fprintf(of, "const WORD RO_B_L%d_N%d_P%d = %s;\n", ii, i, j, buffer);
            }
        }
        fprintf(of, "\n");
    }

    genHeaders(featureVecLen, of);

    genAuxFunctions(of, precision);

    //Registers
    fprintf(of, "register<WORD>(2) RegIteration;              //0 - Receive, 1 - Send\n");
    fprintf(of, "register<WORD>(%d) RegFV;                     //Feature Vector\n", featureVecLen);
    fprintf(of, "register<WORD>(%d) RegFACC;                   //Feature Accumulator\n", featureVecLen);
    fprintf(of, "register<WORD>(1) RegReceivedThisIteration;  //For average aggregation\n");
    fprintf(of, "register<bit<48>>(1) RegLastTimestamp;       //For trigerring next iteration\n");
    fprintf(of, "register<bit<1>>(1) RegGNNResult;            //Readout result\n\n");

    fprintf(of, "/**************** GNN MLPs ****************/\n\n");

    //MSG NN
    int maxNeurons = 0;
    for(int ii = 0; ii < (int) layers.msgB.size(); ii++)
        if(layers.msgB[ii].GetNCols() > maxNeurons)
            maxNeurons = layers.msgB[ii].GetNCols();

    fprintf(of, "//Max neurons is %d\n", maxNeurons);
    fprintf(of, "void NN_MSG_Apply(in gnn_feature_vec MyFV, inout gnn_feature_vec MsgFV) {\n");
    
    for(int ii = 0; ii < maxNeurons; ii++){
        fprintf(of, "    WORD activationN%d_0 = 0;\n", ii);
        fprintf(of, "    WORD activationN%d_1 = 0;\n", ii);
    }
    fprintf(of, "\n    //L0\n");
    for(int i = 0; i < layers.msgW[0].GetNCols(); i++){
        fprintf(of, "    activationN%d_0 = MSG_B_L0_N%d_P0;\n", i, i);
    }
    for(int ii = 0; ii < featureVecLen; ii++){
        for(int i = 0; i < layers.msgW[0].GetNCols(); i++){
            fprintf(of, "    activationN%d_0 = activationN%d_0 + FPMult(MsgFV.feature%d, MSG_W_L0_N%d_P%d);\n", i, i, ii, i, ii);
        }
        fprintf(of, "\n");
    }
    for(int ii = 0; ii < featureVecLen; ii++){
        for(int i = 0; i < layers.msgW[0].GetNCols(); i++){
            fprintf(of, "    activationN%d_0 = activationN%d_0 + FPMult(MyFV.feature%d, MSG_W_L0_N%d_P%d);\n", i, i, ii, i, ii + featureVecLen);
        }
        fprintf(of, "\n");
    }

    for(int i = 0; i < layers.msgW[0].GetNCols(); i++){
        fprintf(of, "    activationN%d_0 = RELu(activationN%d_0);\n", i, i);
    }
    fprintf(of, "\n");

    for(int ii = 1; ii < (int) layers.msgW.size(); ii++){
        fprintf(of, "    //L%d\n", ii);
        for(int i = 0; i < layers.msgW[ii].GetNCols(); i++){
            fprintf(of, "    activationN%d_%d = MSG_B_L%d_N%d_P0;\n", i, ii%2, ii, i);
        }

        for(int i = 0; i < layers.msgW[ii].GetNCols(); i++){
            for(int j = 0; j < layers.msgW[ii].GetNLines(); j++){
                fprintf(of, "    activationN%d_%d = activationN%d_%d + FPMult(activationN%d_%d, MSG_W_L%d_N%d_P%d);\n", i, ii%2, i, ii%2, j, (ii+1)%2, ii, i, j);
            }
            fprintf(of, "\n");
        }

        for(int i = 0; i < layers.msgW[ii].GetNCols(); i++){
            fprintf(of, "    activationN%d_%d = RELu(activationN%d_%d);\n", i, ii%2, i, ii%2);
        }
        fprintf(of, "\n");
    }
    for(int i = 0; i < featureVecLen; i++){
        fprintf(of, "    MsgFV.feature%d = activationN%d_%d;\n", i, i, ((int) layers.msgW.size()+1) % 2);
    }
    fprintf(of, "}\n\n");

    //UPD NN
    maxNeurons = 0;
    for(int ii = 0; ii < (int) layers.uptB.size(); ii++)
        if(layers.uptB[ii].GetNCols() > maxNeurons)
            maxNeurons = layers.uptB[ii].GetNCols();

    fprintf(of, "//Max neurons is %d\n", maxNeurons);
    fprintf(of, "void NN_UPD_Apply(in gnn_feature_vec MyFV, inout gnn_feature_vec AccFV) {\n");
    
    for(int ii = 0; ii < maxNeurons; ii++){
        fprintf(of, "    WORD activationN%d_0 = 0;\n", ii);
        fprintf(of, "    WORD activationN%d_1 = 0;\n", ii);
    }
    fprintf(of, "\n    //L0\n");
    for(int i = 0; i < layers.uptW[0].GetNCols(); i++){
        fprintf(of, "    activationN%d_0 = UPD_B_L0_N%d_P0;\n", i, i);
    }
    for(int ii = 0; ii < featureVecLen; ii++){
        for(int i = 0; i < layers.uptW[0].GetNCols(); i++){
            fprintf(of, "    activationN%d_0 = activationN%d_0 + FPMult(AccFV.feature%d, UPD_W_L0_N%d_P%d);\n", i, i, ii, i, ii);
        }
        fprintf(of, "\n");
    }
    for(int ii = 0; ii < featureVecLen; ii++){
        for(int i = 0; i < layers.uptW[0].GetNCols(); i++){
            fprintf(of, "    activationN%d_0 = activationN%d_0 + FPMult(MyFV.feature%d, UPD_W_L0_N%d_P%d);\n", i, i, ii, i, ii + featureVecLen);
        }
        fprintf(of, "\n");
    }

    for(int i = 0; i < layers.uptW[0].GetNCols(); i++){
        fprintf(of, "    activationN%d_0 = RELu(activationN%d_0);\n", i, i);
    }
    fprintf(of, "\n");

    for(int ii = 1; ii < (int) layers.uptW.size(); ii++){
        fprintf(of, "    //L%d\n", ii);
        for(int i = 0; i < layers.uptW[ii].GetNCols(); i++){
            fprintf(of, "    activationN%d_%d = UPD_B_L%d_N%d_P0;\n", i, ii%2, ii, i);
        }

        for(int i = 0; i < layers.uptW[ii].GetNCols(); i++){
            for(int j = 0; j < layers.uptW[ii].GetNLines(); j++){
                fprintf(of, "    activationN%d_%d = activationN%d_%d + FPMult(activationN%d_%d, UPD_W_L%d_N%d_P%d);\n", i, ii%2, i, ii%2, j, (ii+1)%2, ii, i, j);
            }
            fprintf(of, "\n");
        }

        for(int i = 0; i < layers.uptW[ii].GetNCols(); i++){
            fprintf(of, "    activationN%d_%d = RELu(activationN%d_%d);\n", i, ii%2, i, ii%2);
        }
        fprintf(of, "\n");
    }
    for(int i = 0; i < featureVecLen; i++){
        fprintf(of, "    AccFV.feature%d = activationN%d_%d;\n", i, i, ((int) layers.uptW.size()+1) % 2);
    }

    fprintf(of, "}\n\n");

    //RO NN
    maxNeurons = 0;
    for(int ii = 0; ii < (int) layers.roB.size(); ii++)
        if(layers.roB[ii].GetNCols() > maxNeurons)
            maxNeurons = layers.roB[ii].GetNCols();

    fprintf(of, "//Max neurons is %d\n", maxNeurons);
    fprintf(of, "bit<1> NN_RO_Apply(in gnn_feature_vec MyFV) {\n");
    
    for(int ii = 0; ii < maxNeurons; ii++){
        fprintf(of, "    WORD activationN%d_0 = 0;\n", ii);
        fprintf(of, "    WORD activationN%d_1 = 0;\n", ii);
    }
    fprintf(of, "\n    //L0\n");
    for(int i = 0; i < layers.roW[0].GetNCols(); i++){
        fprintf(of, "    activationN%d_0 = RO_B_L0_N%d_P0;\n", i, i);
    }
    for(int ii = 0; ii < featureVecLen; ii++){
        for(int i = 0; i < layers.roW[0].GetNCols(); i++){
            fprintf(of, "    activationN%d_0 = activationN%d_0 + FPMult(MyFV.feature%d, RO_W_L0_N%d_P%d);\n", i, i, ii, i, ii);
        }
        fprintf(of, "\n");
    }

    for(int i = 0; i < layers.roW[0].GetNCols(); i++){
        fprintf(of, "    activationN%d_0 = RELu(activationN%d_0);\n", i, i);
    }
    fprintf(of, "\n");

    for(int ii = 1; ii < (int) layers.roW.size(); ii++){
        fprintf(of, "    //L%d\n", ii);
        for(int i = 0; i < layers.roW[ii].GetNCols(); i++){
            fprintf(of, "    activationN%d_%d = RO_B_L%d_N%d_P0;\n", i, ii%2, ii, i);
        }

        for(int i = 0; i < layers.roW[ii].GetNCols(); i++){
            for(int j = 0; j < layers.roW[ii].GetNLines(); j++){
                fprintf(of, "    activationN%d_%d = activationN%d_%d + FPMult(activationN%d_%d, RO_W_L%d_N%d_P%d);\n", i, ii%2, i, ii%2, j, (ii+1)%2, ii, i, j);
            }
            fprintf(of, "\n");
        }

        if(ii != (int) layers.roW.size() - 1){
            for(int i = 0; i < layers.roW[ii].GetNCols(); i++){
                fprintf(of, "    activationN%d_%d = RELu(activationN%d_%d);\n", i, ii%2, i, ii%2);
            }
            fprintf(of, "\n");
        }
    }

    fprintf(of, "    return BooleanSigmoid(activationN0_%d);\n", ((int) layers.roW.size()+1) % 2);

    fprintf(of, "}\n\n");

    genGNNUpdate(featureVecLen, of);
    genParser(of);
    genVerifyChecksum(of);
    genIngress(featureVecLen, of);
    genEgress(featureVecLen, of);
    genComputeChecksum(of);
    genDeparser(of);
    genSwitch(of);

    if(strcmp(outFilePath, "NO_OUT"))
        fclose(of);

    return 0;
}