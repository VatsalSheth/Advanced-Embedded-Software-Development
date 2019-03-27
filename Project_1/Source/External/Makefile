CC = gcc

external : external.o external.h
	$(CC) -g -o external external.o -lpthread -lrt

external.o : external.c external.h
	$(CC) -c external.c

clean : 	
	rm -f external external.o
