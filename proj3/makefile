all: tm

tm: tm.o main.o
	g++ tm.o main.o -o tm

main.o: main.cpp tm.h
	g++ -c main.cpp

tm.o: tm.cpp tm.h
	g++ -c tm.cpp

clean:
	rm -f *.o tm 
