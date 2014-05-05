CC=g++

INCLUDE =. 

CFLAGS =-g -Wall

OO = main.o getcloserV.o MV.o

HH = objects.hpp getcloserV.hpp MV.hpp

max-matching: main.o $(OO)   

	$(CC) $(OO) -o max-matching

main.o: main.cpp $(HH)  

	$(CC) -c -g -Wall main.cpp

getcloserV.o: getcloserV.cpp  $(HH)

	$(CC) -c -g -Wall getcloserV.cpp

MV.o: MV.cpp $(HH)
	$(CC) -c -g -Wall MV.cpp