OPTIMIZATION_FLAG = -O0 -g

all: gnn

gnn: bin/main.o bin/MLPFeedF.o bin/GNN.o bin/Graph.o
	gcc $(OPTIMIZATION_FLAG) -o gnn bin/main.o bin/MLPFeedF.o bin/GNN.o bin/Graph.o -lm -Wall

bin/main.o: include/pcheaders.h include/Graph.h include/MLPFeedF.h include/GNN.h src/main.c
	gcc $(OPTIMIZATION_FLAG) -o bin/main.o -c src/main.c -Wall

bin/MLPFeedF.o: include/pcheaders.h include/MLPFeedF.h src/MLPFeedF.c
	gcc $(OPTIMIZATION_FLAG) -o bin/MLPFeedF.o -c src/MLPFeedF.c -Wall

bin/GNN.o: include/pcheaders.h include/Graph.h include/MLPFeedF.h include/GNN.h src/GNN.c
	gcc $(OPTIMIZATION_FLAG) -o bin/GNN.o -c src/GNN.c -Wall

bin/Graph.o: include/pcheaders.h include/Graph.h src/Graph.c
	gcc $(OPTIMIZATION_FLAG) -o bin/Graph.o -c src/Graph.c -Wall

clean:
	rm -f gnn
	rm -rf bin/*.o
