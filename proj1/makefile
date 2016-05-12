all: nfa

nfa: nfa.o main.o
	g++ nfa.o main.o -o nfa

main.o: main.cpp nfa.h
	g++ -c main.cpp

nfa.o: nfa.cpp nfa.h
	g++ -c nfa.cpp

clean:
	rm -f *.o nfa 
