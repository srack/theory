all: dpda

dpda: dpda.o main.o
	g++ dpda.o main.o -o dpda

main.o: main.cpp dpda.h
	g++ -c main.cpp

dpda.o: dpda.cpp dpda.h
	g++ -c dpda.cpp

clean:
	rm -f *.o dpda 
