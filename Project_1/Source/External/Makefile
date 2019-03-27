external : external.o external.h
	gcc -g -o external external.o -lpthread -lrt

external.o : external.c external.h
	gcc -c external.c

clean : 	
	rm -f external external.o
