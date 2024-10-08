#include <core.p4>
#include <v1model.p4>

/* Define constants for types of packets */
#define PKT_INSTANCE_TYPE_NORMAL 0
#define PKT_INSTANCE_TYPE_INGRESS_CLONE 1
#define PKT_INSTANCE_TYPE_EGRESS_CLONE 2
#define PKT_INSTANCE_TYPE_COALESCED 3
#define PKT_INSTANCE_TYPE_INGRESS_RECIRC 4
#define PKT_INSTANCE_TYPE_REPLICATION 5
#define PKT_INSTANCE_TYPE_RESUBMIT 6

#define PRECISION 24
#define FEATURE_VEC_LEN 4

#define WORDSIZE 32
#define D_WORDSIZE 64
#define DWORD_NEGATIVE_PADDING 0b1111111111111111111111111111111100000000000000000000000000000000

typedef bit<WORDSIZE> WORD;
typedef bit<D_WORDSIZE> DWORD;

const bit<48> GNN_ITERATION_PERIOD = 100 * 1000;
const bit<32> N_GNN_ITERATIONS = 2;

const bit<16> TYPE_IPV4 = 0x800;
const bit<16> TYPE_GNN  = 0x801;

const WORD MSG_W_L0_N0_P0 = 0b11111111011110001001011010101000;
const WORD MSG_W_L0_N0_P1 = 0b00000000101110010011000100011000;
const WORD MSG_W_L0_N0_P2 = 0b00000000100111110011111101001110;
const WORD MSG_W_L0_N0_P3 = 0b00000000011010001001010100011011;

const WORD MSG_W_L0_N1_P0 = 0b11111111111010001000000101101101;
const WORD MSG_W_L0_N1_P1 = 0b00000000101100110000010000100000;
const WORD MSG_W_L0_N1_P2 = 0b11111111011100100001011010010110;
const WORD MSG_W_L0_N1_P3 = 0b00000000011000111110111000101110;

const WORD MSG_W_L0_N2_P0 = 0b11111111011110001100100010011101;
const WORD MSG_W_L0_N2_P1 = 0b11111111101001111100001111100001;
const WORD MSG_W_L0_N2_P2 = 0b11111111111100100101110101011101;
const WORD MSG_W_L0_N2_P3 = 0b11111111110101101100001000110010;

const WORD MSG_W_L0_N3_P0 = 0b00000000100000111110000001100111;
const WORD MSG_W_L0_N3_P1 = 0b11111111111001011101100110110100;
const WORD MSG_W_L0_N3_P2 = 0b11111111001001000011010000001011;
const WORD MSG_W_L0_N3_P3 = 0b00000000110101100010101100101100;

const WORD MSG_W_L1_N0_P0 = 0b11111111110110010010110000011001;
const WORD MSG_W_L1_N0_P1 = 0b00000000100010001101001111001011;
const WORD MSG_W_L1_N0_P2 = 0b11111111001111111100110010110101;
const WORD MSG_W_L1_N0_P3 = 0b11111111011000010100001100101100;

const WORD MSG_W_L1_N1_P0 = 0b11111111011101101000101101010101;
const WORD MSG_W_L1_N1_P1 = 0b11111111001001110100001101110100;
const WORD MSG_W_L1_N1_P2 = 0b00000000011110010011010001010011;
const WORD MSG_W_L1_N1_P3 = 0b11111111011011011110011111010000;

const WORD MSG_W_L2_N0_P0 = 0b11111111010110001000011000101001;
const WORD MSG_W_L2_N0_P1 = 0b11111111111110101100101101010101;

const WORD MSG_W_L2_N1_P0 = 0b11111111001000000011011001101110;
const WORD MSG_W_L2_N1_P1 = 0b00000000001100001001010101001110;

const WORD MSG_B_L0_N0_P0 = 0b00000000000000000000000000000000;
const WORD MSG_B_L0_N1_P0 = 0b00000000000000000000000000000000;
const WORD MSG_B_L0_N2_P0 = 0b00000000000000000000000000000000;
const WORD MSG_B_L0_N3_P0 = 0b00000000000000000000000000000000;

const WORD MSG_B_L1_N0_P0 = 0b00000000000000000000000000000000;
const WORD MSG_B_L1_N1_P0 = 0b00000000000000000000000000000000;

const WORD MSG_B_L2_N0_P0 = 0b00000000000000000000000000000000;
const WORD MSG_B_L2_N1_P0 = 0b00000000000000000000000000000000;

const WORD UPD_W_L0_N0_P0 = 0b00000000011011110101101011110000;
const WORD UPD_W_L0_N0_P1 = 0b00000000001101100011011011101000;
const WORD UPD_W_L0_N0_P2 = 0b00000000111110111010100010101010;
const WORD UPD_W_L0_N0_P3 = 0b11111111101100011010100100110111;

const WORD UPD_W_L0_N1_P0 = 0b11111111110001100111100100101001;
const WORD UPD_W_L0_N1_P1 = 0b00000000011001110101101100101100;
const WORD UPD_W_L0_N1_P2 = 0b00000000011000010000011101010101;
const WORD UPD_W_L0_N1_P3 = 0b00000000100000111110110011100010;

const WORD UPD_W_L0_N2_P0 = 0b11111111111111001001111110010011;
const WORD UPD_W_L0_N2_P1 = 0b11111111111010000010011110010111;
const WORD UPD_W_L0_N2_P2 = 0b11111111101101010101111111000101;
const WORD UPD_W_L0_N2_P3 = 0b11111111111010101011001010101111;

const WORD UPD_W_L0_N3_P0 = 0b11111111101010110100111000000111;
const WORD UPD_W_L0_N3_P1 = 0b11111111100000110100110011010101;
const WORD UPD_W_L0_N3_P2 = 0b11111111101011110010101010000100;
const WORD UPD_W_L0_N3_P3 = 0b00000000100011011001100111111100;

const WORD UPD_W_L1_N0_P0 = 0b00000000001101000010100011000110;
const WORD UPD_W_L1_N0_P1 = 0b00000000111110111101001101011011;
const WORD UPD_W_L1_N0_P2 = 0b11111111100111000001100110110000;
const WORD UPD_W_L1_N0_P3 = 0b00000000011101010001111111001110;

const WORD UPD_W_L1_N1_P0 = 0b11111111100101111001100100100000;
const WORD UPD_W_L1_N1_P1 = 0b00000000011001011110000111001001;
const WORD UPD_W_L1_N1_P2 = 0b11111111110011100001011111101000;
const WORD UPD_W_L1_N1_P3 = 0b00000000001100000110000111001100;

const WORD UPD_W_L2_N0_P0 = 0b11111111110001111011100010011011;
const WORD UPD_W_L2_N0_P1 = 0b00000000101111111001110111111010;

const WORD UPD_W_L2_N1_P0 = 0b00000000100001100000111001100111;
const WORD UPD_W_L2_N1_P1 = 0b00000010001010001011111111101000;

const WORD UPD_B_L0_N0_P0 = 0b11111110110100000010000011010110;
const WORD UPD_B_L0_N1_P0 = 0b11111110111001010110110001010011;
const WORD UPD_B_L0_N2_P0 = 0b00000000000000000000000000000000;
const WORD UPD_B_L0_N3_P0 = 0b11111110110001110010010110101000;

const WORD UPD_B_L1_N0_P0 = 0b11111110111101111111110000000110;
const WORD UPD_B_L1_N1_P0 = 0b11111110111101110001000110100001;

const WORD UPD_B_L2_N0_P0 = 0b00000000000001101000000100101000;
const WORD UPD_B_L2_N1_P0 = 0b11111111000011001101001101111110;

const WORD RO_W_L0_N0_P0 = 0b11111111101011011010001100011001;
const WORD RO_W_L0_N0_P1 = 0b11111111001001110111001001100110;

const WORD RO_W_L0_N1_P0 = 0b11111111000100000111010111100101;
const WORD RO_W_L0_N1_P1 = 0b00000010001011110110101001100011;

const WORD RO_W_L1_N0_P0 = 0b00000000010011011100000101000100;
const WORD RO_W_L1_N0_P1 = 0b00000001110011111110001110000110;

const WORD RO_W_L1_N1_P0 = 0b00000000111001101000011001010101;
const WORD RO_W_L1_N1_P1 = 0b00000001111100101010000100000000;

const WORD RO_W_L2_N0_P0 = 0b00000001010111100011001100001010;
const WORD RO_W_L2_N0_P1 = 0b00000001100010010011010000001011;

const WORD RO_B_L0_N0_P0 = 0b00000000000000000000000000000000;
const WORD RO_B_L0_N1_P0 = 0b11111111001110111011101101010010;

const WORD RO_B_L1_N0_P0 = 0b11111111001101001111011010001001;
const WORD RO_B_L1_N1_P0 = 0b11111111011011010110001000001100;

const WORD RO_B_L2_N0_P0 = 0b11110111110111111000010011001011;

/*************** H E A D E R S ***************/

typedef bit<9>  egressSpec_t;
typedef bit<48> macAddr_t;
typedef bit<32> ip4Addr_t;

header ethernet_t {
    macAddr_t dstAddr;
    macAddr_t srcAddr;
    bit<16> etherType;
}

header ipv4_t {
    bit<4> version;
    bit<4> ihl;
    bit<6> diffserv;
    bit<2> ecn;
    bit<16> totalLen;
    bit<16> identification;
    bit<3> flags;
    bit<13> fragOffset;
    bit<8> ttl;
    bit<8> protocol;
    bit<16> hdrChecksum;
    ip4Addr_t srcAddr;
    ip4Addr_t dstAddr;
}

struct gnn_feature_vec {
    WORD feature0;
    WORD feature1;
}

header gnn_t {
    gnn_feature_vec features;
    WORD iteration;
}

struct metadata {
    /* empty */
}

struct headers {
    ethernet_t ethernet;
    ipv4_t ipv4;
    gnn_t gnn;
}

/************** A U X   F U N C T I O N S **************/

WORD FPMult(in WORD opA, in WORD opB) {
    DWORD opAD = (DWORD) opA;
    DWORD opBD = (DWORD) opB;
    if (opAD & (1 << (WORDSIZE-1)) != 0) {
        opAD = opAD + DWORD_NEGATIVE_PADDING;
    }
    if (opBD & (1 << (WORDSIZE-1)) != 0) {
        opBD = opBD + DWORD_NEGATIVE_PADDING;
    }

    DWORD result = ((opAD * opBD) >> PRECISION);
    return (WORD) result;
}

WORD FixedDivision(in WORD numerator, in WORD denominator) {
    if (denominator == 2){
        return numerator >> 1;
    }
    else if (denominator == 3){
        return FPMult(numerator, 0b00000000010101010101010101001111); // * (0.333333)
    }
    else if (denominator == 4){
        return numerator >> 2;
    }
    else if (denominator == 5){
        return FPMult(numerator, 0b00000000001100110011001100110011); // * (0.2)
    }

    return numerator;
}

WORD RELu(in WORD op) {
    if (op & (1 << (WORDSIZE-1)) != 0) {
        return 0;
    }
    return op;
}

bit<1> BooleanSigmoid(in WORD op) {
    if (op & (1 << (WORDSIZE-1)) != 0) {
        return 0;
    }
    return 1;
}

register<WORD>(2) RegIteration;              //0 - Receive, 1 - Send
register<WORD>(2) RegFV;                     //Feature Vector
register<WORD>(2) RegFACC;                   //Feature Accumulator
register<WORD>(1) RegReceivedThisIteration;  //For average aggregation
register<bit<48>>(1) RegLastTimestamp;       //For trigerring next iteration;

register<WORD>(1) RegQDepth;

register<bit<1>>(1) RegGNNResult;

/**************** GNN MLPs ****************/

//Max neurons is 4
void NN_MSG_Apply(in gnn_feature_vec MyFV, inout gnn_feature_vec MsgFV) {
    WORD activationN0_0 = 0;
    WORD activationN0_1 = 0;
    WORD activationN1_0 = 0;
    WORD activationN1_1 = 0;
    WORD activationN2_0 = 0;
    WORD activationN2_1 = 0;
    WORD activationN3_0 = 0;
    WORD activationN3_1 = 0;

    //L0
    activationN0_0 = MSG_B_L0_N0_P0;
    activationN1_0 = MSG_B_L0_N1_P0;
    activationN2_0 = MSG_B_L0_N2_P0;
    activationN3_0 = MSG_B_L0_N3_P0;
    activationN0_0 = activationN0_0 + FPMult(MsgFV.feature0, MSG_W_L0_N0_P0);
    activationN1_0 = activationN1_0 + FPMult(MsgFV.feature0, MSG_W_L0_N1_P0);
    activationN2_0 = activationN2_0 + FPMult(MsgFV.feature0, MSG_W_L0_N2_P0);
    activationN3_0 = activationN3_0 + FPMult(MsgFV.feature0, MSG_W_L0_N3_P0);

    activationN0_0 = activationN0_0 + FPMult(MsgFV.feature1, MSG_W_L0_N0_P1);
    activationN1_0 = activationN1_0 + FPMult(MsgFV.feature1, MSG_W_L0_N1_P1);
    activationN2_0 = activationN2_0 + FPMult(MsgFV.feature1, MSG_W_L0_N2_P1);
    activationN3_0 = activationN3_0 + FPMult(MsgFV.feature1, MSG_W_L0_N3_P1);

    activationN0_0 = activationN0_0 + FPMult(MyFV.feature0, MSG_W_L0_N0_P2);
    activationN1_0 = activationN1_0 + FPMult(MyFV.feature0, MSG_W_L0_N1_P2);
    activationN2_0 = activationN2_0 + FPMult(MyFV.feature0, MSG_W_L0_N2_P2);
    activationN3_0 = activationN3_0 + FPMult(MyFV.feature0, MSG_W_L0_N3_P2);

    activationN0_0 = activationN0_0 + FPMult(MyFV.feature1, MSG_W_L0_N0_P3);
    activationN1_0 = activationN1_0 + FPMult(MyFV.feature1, MSG_W_L0_N1_P3);
    activationN2_0 = activationN2_0 + FPMult(MyFV.feature1, MSG_W_L0_N2_P3);
    activationN3_0 = activationN3_0 + FPMult(MyFV.feature1, MSG_W_L0_N3_P3);

    activationN0_0 = RELu(activationN0_0);
    activationN1_0 = RELu(activationN1_0);
    activationN2_0 = RELu(activationN2_0);
    activationN3_0 = RELu(activationN3_0);

    //L1
    activationN0_1 = MSG_B_L1_N0_P0;
    activationN1_1 = MSG_B_L1_N1_P0;
    activationN0_1 = activationN0_1 + FPMult(activationN0_0, MSG_W_L1_N0_P0);
    activationN0_1 = activationN0_1 + FPMult(activationN1_0, MSG_W_L1_N0_P1);
    activationN0_1 = activationN0_1 + FPMult(activationN2_0, MSG_W_L1_N0_P2);
    activationN0_1 = activationN0_1 + FPMult(activationN3_0, MSG_W_L1_N0_P3);

    activationN1_1 = activationN1_1 + FPMult(activationN0_0, MSG_W_L1_N1_P0);
    activationN1_1 = activationN1_1 + FPMult(activationN1_0, MSG_W_L1_N1_P1);
    activationN1_1 = activationN1_1 + FPMult(activationN2_0, MSG_W_L1_N1_P2);
    activationN1_1 = activationN1_1 + FPMult(activationN3_0, MSG_W_L1_N1_P3);

    activationN0_1 = RELu(activationN0_1);
    activationN1_1 = RELu(activationN1_1);

    //L2
    activationN0_0 = MSG_B_L2_N0_P0;
    activationN1_0 = MSG_B_L2_N1_P0;
    activationN0_0 = activationN0_0 + FPMult(activationN0_1, MSG_W_L2_N0_P0);
    activationN0_0 = activationN0_0 + FPMult(activationN1_1, MSG_W_L2_N0_P1);

    activationN1_0 = activationN1_0 + FPMult(activationN0_1, MSG_W_L2_N1_P0);
    activationN1_0 = activationN1_0 + FPMult(activationN1_1, MSG_W_L2_N1_P1);

    activationN0_0 = RELu(activationN0_0);
    activationN1_0 = RELu(activationN1_0);

    MsgFV.feature0 = activationN0_0;
    MsgFV.feature1 = activationN1_0;
}

//Max neurons is 4
void NN_UPD_Apply(in gnn_feature_vec MyFV, inout gnn_feature_vec AccFV) {
    WORD activationN0_0 = 0;
    WORD activationN0_1 = 0;
    WORD activationN1_0 = 0;
    WORD activationN1_1 = 0;
    WORD activationN2_0 = 0;
    WORD activationN2_1 = 0;
    WORD activationN3_0 = 0;
    WORD activationN3_1 = 0;

    //L0
    activationN0_0 = UPD_B_L0_N0_P0;
    activationN1_0 = UPD_B_L0_N1_P0;
    activationN2_0 = UPD_B_L0_N2_P0;
    activationN3_0 = UPD_B_L0_N3_P0;
    activationN0_0 = activationN0_0 + FPMult(AccFV.feature0, UPD_W_L0_N0_P0);
    activationN1_0 = activationN1_0 + FPMult(AccFV.feature0, UPD_W_L0_N1_P0);
    activationN2_0 = activationN2_0 + FPMult(AccFV.feature0, UPD_W_L0_N2_P0);
    activationN3_0 = activationN3_0 + FPMult(AccFV.feature0, UPD_W_L0_N3_P0);

    activationN0_0 = activationN0_0 + FPMult(AccFV.feature1, UPD_W_L0_N0_P1);
    activationN1_0 = activationN1_0 + FPMult(AccFV.feature1, UPD_W_L0_N1_P1);
    activationN2_0 = activationN2_0 + FPMult(AccFV.feature1, UPD_W_L0_N2_P1);
    activationN3_0 = activationN3_0 + FPMult(AccFV.feature1, UPD_W_L0_N3_P1);

    activationN0_0 = activationN0_0 + FPMult(MyFV.feature0, UPD_W_L0_N0_P2);
    activationN1_0 = activationN1_0 + FPMult(MyFV.feature0, UPD_W_L0_N1_P2);
    activationN2_0 = activationN2_0 + FPMult(MyFV.feature0, UPD_W_L0_N2_P2);
    activationN3_0 = activationN3_0 + FPMult(MyFV.feature0, UPD_W_L0_N3_P2);

    activationN0_0 = activationN0_0 + FPMult(MyFV.feature1, UPD_W_L0_N0_P3);
    activationN1_0 = activationN1_0 + FPMult(MyFV.feature1, UPD_W_L0_N1_P3);
    activationN2_0 = activationN2_0 + FPMult(MyFV.feature1, UPD_W_L0_N2_P3);
    activationN3_0 = activationN3_0 + FPMult(MyFV.feature1, UPD_W_L0_N3_P3);

    activationN0_0 = RELu(activationN0_0);
    activationN1_0 = RELu(activationN1_0);
    activationN2_0 = RELu(activationN2_0);
    activationN3_0 = RELu(activationN3_0);

    //L1
    activationN0_1 = UPD_B_L1_N0_P0;
    activationN1_1 = UPD_B_L1_N1_P0;
    activationN0_1 = activationN0_1 + FPMult(activationN0_0, UPD_W_L1_N0_P0);
    activationN0_1 = activationN0_1 + FPMult(activationN1_0, UPD_W_L1_N0_P1);
    activationN0_1 = activationN0_1 + FPMult(activationN2_0, UPD_W_L1_N0_P2);
    activationN0_1 = activationN0_1 + FPMult(activationN3_0, UPD_W_L1_N0_P3);

    activationN1_1 = activationN1_1 + FPMult(activationN0_0, UPD_W_L1_N1_P0);
    activationN1_1 = activationN1_1 + FPMult(activationN1_0, UPD_W_L1_N1_P1);
    activationN1_1 = activationN1_1 + FPMult(activationN2_0, UPD_W_L1_N1_P2);
    activationN1_1 = activationN1_1 + FPMult(activationN3_0, UPD_W_L1_N1_P3);

    activationN0_1 = RELu(activationN0_1);
    activationN1_1 = RELu(activationN1_1);

    //L2
    activationN0_0 = UPD_B_L2_N0_P0;
    activationN1_0 = UPD_B_L2_N1_P0;
    activationN0_0 = activationN0_0 + FPMult(activationN0_1, UPD_W_L2_N0_P0);
    activationN0_0 = activationN0_0 + FPMult(activationN1_1, UPD_W_L2_N0_P1);

    activationN1_0 = activationN1_0 + FPMult(activationN0_1, UPD_W_L2_N1_P0);
    activationN1_0 = activationN1_0 + FPMult(activationN1_1, UPD_W_L2_N1_P1);

    activationN0_0 = RELu(activationN0_0);
    activationN1_0 = RELu(activationN1_0);

    AccFV.feature0 = activationN0_0;
    AccFV.feature1 = activationN1_0;
}

//Max neurons is 2
bit<1> NN_RO_Apply(in gnn_feature_vec MyFV) {
    WORD activationN0_0 = 0;
    WORD activationN0_1 = 0;
    WORD activationN1_0 = 0;
    WORD activationN1_1 = 0;

    //L0
    activationN0_0 = RO_B_L0_N0_P0;
    activationN1_0 = RO_B_L0_N1_P0;
    activationN0_0 = activationN0_0 + FPMult(MyFV.feature0, RO_W_L0_N0_P0);
    activationN1_0 = activationN1_0 + FPMult(MyFV.feature0, RO_W_L0_N1_P0);

    activationN0_0 = activationN0_0 + FPMult(MyFV.feature1, RO_W_L0_N0_P1);
    activationN1_0 = activationN1_0 + FPMult(MyFV.feature1, RO_W_L0_N1_P1);

    activationN0_0 = RELu(activationN0_0);
    activationN1_0 = RELu(activationN1_0);

    //L1
    activationN0_1 = RO_B_L1_N0_P0;
    activationN1_1 = RO_B_L1_N1_P0;
    activationN0_1 = activationN0_1 + FPMult(activationN0_0, RO_W_L1_N0_P0);
    activationN0_1 = activationN0_1 + FPMult(activationN1_0, RO_W_L1_N0_P1);

    activationN1_1 = activationN1_1 + FPMult(activationN0_0, RO_W_L1_N1_P0);
    activationN1_1 = activationN1_1 + FPMult(activationN1_0, RO_W_L1_N1_P1);

    activationN0_1 = RELu(activationN0_1);
    activationN1_1 = RELu(activationN1_1);

    //L2
    activationN0_0 = RO_B_L2_N0_P0;
    activationN0_0 = activationN0_0 + FPMult(activationN0_1, RO_W_L2_N0_P0);
    activationN0_0 = activationN0_0 + FPMult(activationN1_1, RO_W_L2_N0_P1);

    return BooleanSigmoid(activationN0_0);
}

gnn_feature_vec GNNUpdate() {
    gnn_feature_vec MyFV;
    gnn_feature_vec MyFA;

    RegFV.read(MyFV.feature0, 0);
    RegFV.read(MyFV.feature1, 1);

    RegFACC.read(MyFA.feature0, 0);
    RegFACC.read(MyFA.feature1, 1);

    WORD receivedThisIT;
    RegReceivedThisIteration.read(receivedThisIT, 0);
    if (MyFA.feature0 & (1 << (WORDSIZE - 1)) != 0) { //Negative number
        MyFA.feature0 = ~ MyFA.feature0 + 1;
        MyFA.feature0 = FixedDivision(MyFA.feature0, receivedThisIT);
        MyFA.feature0 = ~ MyFA.feature0 + 1;
    } else {
        MyFA.feature0 = FixedDivision(MyFA.feature0, receivedThisIT);
    }

    if (MyFA.feature1 & (1 << (WORDSIZE - 1)) != 0) { //Negative number
        MyFA.feature1 = ~ MyFA.feature1 + 1;
        MyFA.feature1 = FixedDivision(MyFA.feature1, receivedThisIT);
        MyFA.feature1 = ~ MyFA.feature1 + 1;
    } else {
        MyFA.feature1 = FixedDivision(MyFA.feature1, receivedThisIT);
    }

    // MyFA will contain updated feature vector
    NN_UPD_Apply(MyFV, MyFA);

    return MyFA;
}

/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser MyParser(packet_in packet,
                out headers hdr,
                inout metadata meta,
                inout standard_metadata_t standard_metadata) {

    state start {
        transition parse_ethernet;
    }

    state parse_ethernet {
        packet.extract(hdr.ethernet);
        transition select(hdr.ethernet.etherType) {
            TYPE_IPV4: parse_ipv4;
            TYPE_GNN:  parse_gnn;
            default: accept;
        }
    }

    state parse_ipv4 {
        
        packet.extract(hdr.ipv4);
        // Continue to the next state without doing anything extra
        transition accept;
    }

    state parse_gnn {
        packet.extract(hdr.gnn);
        transition accept;
    }
}


/*************************************************************************
************   C H E C K S U M    V E R I F I C A T I O N   *************
*************************************************************************/

control MyVerifyChecksum(inout headers hdr, inout metadata meta) {   
    apply {  }
}


/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {

    action drop() {
        mark_to_drop(standard_metadata);
    }
    
    action ipv4_forward(macAddr_t dstAddr, egressSpec_t port) {
        standard_metadata.egress_spec = port;
        // Update src and dst MACs according to the current switch
        hdr.ethernet.srcAddr = hdr.ethernet.dstAddr;
        hdr.ethernet.dstAddr = dstAddr;
        hdr.ipv4.ttl = hdr.ipv4.ttl - 1;
    }

    action send_gnn_packets() {
        const bit<32> REPORT_MIRROR_SESSION_ID = 500;
        // Clone from ingress to egress pipeline
        clone(CloneType.I2E, REPORT_MIRROR_SESSION_ID);
    }

    table ipv4_forward_table {
        key = {
            hdr.ipv4.dstAddr: lpm;
        }
        actions = {
            ipv4_forward;
            drop;
            NoAction;
        }
        size = 1024;
        default_action = NoAction();
    }

    apply {
        @atomic {

        if (hdr.ipv4.isValid()) {
            ipv4_forward_table.apply();

            bit<48> MyLastTS;
            RegLastTimestamp.read(MyLastTS, 0);

            if(standard_metadata.ingress_global_timestamp - GNN_ITERATION_PERIOD > MyLastTS){
                RegLastTimestamp.write(0, standard_metadata.ingress_global_timestamp);

                bit<32> iterationSendVal;
                RegIteration.read(iterationSendVal, 1);

                RegIteration.write(0, iterationSendVal);
                iterationSendVal = iterationSendVal + 1;
                RegIteration.write(1, iterationSendVal);

                gnn_feature_vec MyFA;
                MyFA = GNNUpdate();

                RegFACC.write(0, MyFA.feature0);
                RegFACC.write(1, MyFA.feature1);

                if (((iterationSendVal + 1) % N_GNN_ITERATIONS) == 0) {
                    bit<1> resultRO = NN_RO_Apply(MyFA);

                    RegGNNResult.write(0, resultRO);

                    RegQDepth.read(MyFA.feature0, 0);
                    RegFV.write(0, MyFA.feature0);
                    RegFV.write(1, 0);
                }
                else {
                    RegFV.write(0, MyFA.feature0);
                    RegFV.write(1, MyFA.feature1);
                }

                RegFACC.write(0, 0);
                RegFACC.write(1, 0);

                RegReceivedThisIteration.write(0, 0);

                send_gnn_packets();
            }
        }
        else if(hdr.gnn.isValid()){
            if(standard_metadata.instance_type == PKT_INSTANCE_TYPE_INGRESS_RECIRC){
                standard_metadata.mcast_grp = 1;
            }
            else{
                WORD iterationReceiveVal;
                WORD iterationSendVal;
                RegIteration.read(iterationReceiveVal, 0);
                RegIteration.read(iterationSendVal, 1);

                if(hdr.gnn.iteration == iterationReceiveVal) {
                    gnn_feature_vec MyFV;
                    RegFV.read(MyFV.feature0, 0);
                    RegFV.read(MyFV.feature1, 1);

                    //hdr.gnn.features gets updated here
                    NN_MSG_Apply(MyFV, hdr.gnn.features);

                    //Accumulate
                    WORD feature;
                    RegFACC.read(feature, 0);
                    RegFACC.write(0, feature + hdr.gnn.features.feature0);
                    RegFACC.read(feature, 1);
                    RegFACC.write(1, feature + hdr.gnn.features.feature1);

                    WORD receivedThisIT;
                    RegReceivedThisIteration.read(receivedThisIT, 0);
                    RegReceivedThisIteration.write(0, receivedThisIT + 1);

                    //If we did not send our packets yet, send now
                    if (iterationReceiveVal == iterationSendVal) {
                        RegIteration.write(1, iterationSendVal + 1);
                        send_gnn_packets();
                    }
                }
                else if(hdr.gnn.iteration < iterationReceiveVal) {
                    //Ignore earlier iteration messages
                }
                else{ //hdr.gnn.iteration > iterationReceiveVal
                    iterationReceiveVal = hdr.gnn.iteration;
                    iterationSendVal = hdr.gnn.iteration + 1;
                    RegIteration.write(0, iterationReceiveVal);
                    RegIteration.write(1, iterationSendVal);

                    gnn_feature_vec MyFA;
                    gnn_feature_vec MyFV;
                    MyFA = GNNUpdate();

                    if (((iterationSendVal + 1) % N_GNN_ITERATIONS) == 0) {
                        bit<1> resultRO = NN_RO_Apply(MyFA);

                        RegGNNResult.write(0, resultRO);

                        RegQDepth.read(MyFV.feature0, 0);
                        RegFV.write(0, MyFV.feature0);
                        RegFV.write(1, 0);

                        MyFV.feature0 = 0;
                        MyFV.feature1 = 0;
                    }
                    else {
                        RegFV.write(0, MyFA.feature0);
                        RegFV.write(1, MyFA.feature1);

                        MyFV.feature0 = MyFA.feature0;
                        MyFV.feature1 = MyFA.feature1;
                    }

                    NN_MSG_Apply(MyFV, hdr.gnn.features);

                    RegFACC.write(0, hdr.gnn.features.feature0);
                    RegFACC.write(1, hdr.gnn.features.feature1);

                    RegReceivedThisIteration.write(0, 1);

                    send_gnn_packets();
                }
            }
        }

    }
    
    }
}

/*************************************************************************
****************  E G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyEgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {

    action change_to_gnn() {
        hdr.ethernet.etherType = TYPE_GNN;
        hdr.gnn.setValid();
        hdr.ipv4.setInvalid();

        RegFV.read(hdr.gnn.features.feature0, 0);
        RegFV.read(hdr.gnn.features.feature1, 1);
        RegIteration.read(hdr.gnn.iteration, 0);
    }

    action mark_ecn() {
        hdr.ipv4.ecn = 3;
    }

    apply {
        
        // Check if this is a cloned packet used to trigger multicast via recirculate
        if (standard_metadata.instance_type == PKT_INSTANCE_TYPE_INGRESS_CLONE) {
            change_to_gnn();
            recirculate_preserving_field_list(0);
        }

        else if(hdr.ipv4.isValid()){
            if (hdr.ipv4.ecn == 1 || hdr.ipv4.ecn == 2){
                bit<1> GNNResult;
                RegGNNResult.read(GNNResult, 0);
                if (GNNResult == 1){
                    mark_ecn();
                }

                /*
                if (standard_metadata.enq_qdepth >= 30){
                    mark_ecn();
                }
                */
            }
        }

        else if(hdr.gnn.isValid()){
            truncate(32);
        }

        if(standard_metadata.egress_port == 10){
            RegQDepth.write(0, ((WORD) standard_metadata.enq_qdepth) << PRECISION);
        }
    }
}

/*************************************************************************
*************   C H E C K S U M    C O M P U T A T I O N   **************
*************************************************************************/

control MyComputeChecksum(inout headers hdr, inout metadata meta) {
     apply {
	update_checksum(
	    hdr.ipv4.isValid(),
            { hdr.ipv4.version,
	          hdr.ipv4.ihl,
              hdr.ipv4.diffserv,
              hdr.ipv4.ecn,
              hdr.ipv4.totalLen,
              hdr.ipv4.identification,
              hdr.ipv4.flags,
              hdr.ipv4.fragOffset,
              hdr.ipv4.ttl,
              hdr.ipv4.protocol,
              hdr.ipv4.srcAddr,
              hdr.ipv4.dstAddr },
            hdr.ipv4.hdrChecksum,
            HashAlgorithm.csum16);
    }
}


/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control MyDeparser(packet_out packet, in headers hdr) {
    apply {
        packet.emit(hdr.ethernet);
        packet.emit(hdr.ipv4);
        packet.emit(hdr.gnn);
    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

V1Switch(
MyParser(),
MyVerifyChecksum(),
MyIngress(),
MyEgress(),
MyComputeChecksum(),
MyDeparser()
) main;
