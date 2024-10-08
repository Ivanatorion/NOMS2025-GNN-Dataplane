#include "../include/pcheaders.h"
#include "../include/Graph.h"

#define HIDDEN_STATE_DIM 2

Graph* Graph_Create(){
    Graph* graph = (Graph *) malloc(sizeof(Graph));

    graph->hiddenStateDim = 0;
    graph->nEdges = 0;
    graph->nNodes = 0;
    graph->nodes = NULL;
    graph->edges = NULL;

    return graph;
}

void Graph_Destroy(Graph* graph){
    int i;

    if(graph->nodes != NULL){
        for(i = 0; i < graph->nNodes; i++){
            free(graph->nodes[i].hiddenState);
            free(graph->nodes[i].inEdgesIndexes);
            free(graph->nodes[i].outEdgesIndexes);
        }    
        free(graph->nodes);
    }
    if(graph->edges != NULL){
        free(graph->edges);
    }

    free(graph);
}

void Graph_LoadFromFile(Graph* graph, const char *file, const char *partitionFile){
    int i, rb = 0;
    FILE *fp = fopen(file, "r");

    if(!fp){
        fprintf(stderr, "Could not open file: %s\n", file);
        exit(1);
    }

    rb += fscanf(fp, "%d\n%d\n", &graph->nNodes, &graph->nEdges);

    if(graph->nodes != NULL)
        free(graph->nodes);
    if(graph->edges != NULL)
        free(graph->edges);

    graph->nodes = (GraphNode *) malloc(sizeof(GraphNode) * graph->nNodes);
    graph->edges = (GraphEdge *) malloc(sizeof(GraphEdge) * graph->nEdges);

    graph->hiddenStateDim = HIDDEN_STATE_DIM;
    const int HIDDEN_STATE_BYTES = graph->hiddenStateDim * sizeof(double);
    FILE *pf = fopen(partitionFile, "r");
    if(!pf){
        fprintf(stderr, "Could not open file: %s\n", partitionFile);
        exit(1);
    }
    for(i = 0; i < graph->nNodes; i++){
        graph->nodes[i].hiddenState = (double *) malloc(HIDDEN_STATE_BYTES);
        graph->nodes[i].nInEdges = 0;
        graph->nodes[i].nOutEdges = 0;
        memset(graph->nodes[i].hiddenState, 0, HIDDEN_STATE_BYTES);
        double part;
        rb += fscanf(pf, "%lf\n", &part);
        graph->nodes[i].hiddenState[0] = (double) part;
    }
    fclose(pf);

    for(i = 0; i < graph->nEdges; i++){
        rb += fscanf(fp, "%d %d\n", &graph->edges[i].n1, &graph->edges[i].n2);
        graph->nodes[graph->edges[i].n1].nOutEdges++;
        graph->nodes[graph->edges[i].n2].nInEdges++;
    }

    fclose(fp);

    for(i = 0; i < graph->nNodes; i++){
        graph->nodes[i].inEdgesIndexes = (int *) malloc(sizeof(int) * graph->nodes[i].nInEdges);
        graph->nodes[i].outEdgesIndexes = (int *) malloc(sizeof(int) * graph->nodes[i].nOutEdges);
    }

    int *totalInEdges = (int *) malloc(sizeof(int) * graph->nNodes);
    int *totalOutEdges = (int *) malloc(sizeof(int) * graph->nNodes);
    memset(totalInEdges, 0, sizeof(int) * graph->nNodes);
    memset(totalOutEdges, 0, sizeof(int) * graph->nNodes);

    for(i = 0; i < graph->nEdges; i++){
        int node;
        node = graph->edges[i].n1;
        graph->nodes[node].outEdgesIndexes[totalOutEdges[node]] = i;
        totalOutEdges[node]++;

        node = graph->edges[i].n2;
        graph->nodes[node].inEdgesIndexes[totalInEdges[node]] = i;
        totalInEdges[node]++;
    }

    free(totalInEdges);
    free(totalOutEdges);
}

void Graph_Print(Graph* graph){
    int i, j;
    printf("Nodes: %d\nEdges: %d\n", graph->nNodes, graph->nEdges);
    for(i = 0; i < graph->nEdges; i++)
        printf("%d %d\n", graph->edges[i].n1, graph->edges[i].n2);
    printf("InEdges:\n");
    for(i = 0; i < graph->nNodes; i++){
        printf("%d: ", i);
        for(j = 0; j < graph->nodes[i].nInEdges; j++)
            printf("%d ", graph->nodes[i].inEdgesIndexes[j]);
        printf("\n");
    }
    printf("OutEdges:\n");
    for(i = 0; i < graph->nNodes; i++){
        printf("%d: ", i);
        for(j = 0; j < graph->nodes[i].nOutEdges; j++)
            printf("%d ", graph->nodes[i].outEdgesIndexes[j]);
        printf("\n");
    }
}