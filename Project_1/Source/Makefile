objects = main.o log.o
headers = ../Include/main.h ../Include/log.h 
CC = gcc

project_1 : $(objects) $(headers)
	$(CC) -o project_1 $(objects) -lpthread -lrt

main.o : main.c $(headers)
	$(CC) -c main.c

log.o : log.c $(headers)
	$(CC) -c log.c

clean : 	
	rm -f project_1 $(objects)
