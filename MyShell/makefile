# makefile

all: main

Main.o : main.cpp
	g++ -c -g main.cpp

main: Main.o 
	g++ -o main Main.o

clean:
	rm *.o