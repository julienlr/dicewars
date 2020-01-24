all: bin/linux/arbitre bin/linux/strategie.so

bin/linux/arbitre: bin/linux/temp/main.o
	g++ -o bin/linux/arbitre bin/linux/temp/main.o -ldl -fPIC

bin/linux/strategie.so: bin/linux/temp/strategie.o
	mkdir -p bin/linux/temp
	g++ -o bin/linux/strategie.so -shared bin/linux/temp/strategie.o -fPIC

bin/linux/temp/main.o: Arbitre/src/main.cpp Commun/interface.h Commun/library.h
	mkdir -p bin/linux/temp
	g++ -c Arbitre/src/main.cpp -o bin/linux/temp/main.o -fPIC

bin/linux/temp/strategie.o: Strategie/src/strategie.cpp Strategie/src/interface_lib.h Commun/interface.h Commun/library.h
	mkdir -p bin/linux/temp
	g++ -c Strategie/src/strategie.cpp -o bin/linux/temp/strategie.o -fPIC

clean:
	rm -rf bin/linux

run: bin/linux/arbitre bin/linux/strategie.so
	bin/linux/arbitre bin/linux/strategie.so
