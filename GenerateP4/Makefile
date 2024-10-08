all: genP4 fixedPoint

genP4: bin/genP4.o
	g++ -std=c++11 -O2 -o genP4 bin/genP4.o -lm -Wall

fixedPoint: bin/fixedPoint.o
	g++ -std=c++11 -O2 -o fixedPoint bin/fixedPoint.o -Wall

bin/genP4.o: src/genP4.cpp
	g++ -std=c++11 -O2 -o bin/genP4.o -c src/genP4.cpp -Wall

bin/fixedPoint.o: src/fixedPoint.cpp
	g++ -std=c++11 -O2 -o bin/fixedPoint.o -c src/fixedPoint.cpp -Wall

clean:
	rm -rf bin/*.o
	rm -f genP4 fixedPoint
