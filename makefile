CC=g++

INCLUDE =. 

CFLAGS =-g -Wall

OO = main.o getcloserV.o

HH = objects.hpp getcloserV.hpp

max-matching: main.o $(OO)   

	$(CC) $(OO) -o max-matching

main.o: main.cpp $(HH)  

	$(CC) -c -g -Wall main.cpp

getcloserV.o: getcloserV.cpp  $(HH)

	$(CC) -c -g -Wall getcloserV.cpp