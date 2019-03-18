objects = main.o log.o temp.o
headers = ../Include/main.h ../Include/log.h ../Include/temp.h 
CC = gcc

project_1 : $(objects) $(headers)
	$(CC) -o project_1 $(objects) -lpthread -lrt

main.o : main.c $(headers)
	$(CC) -c main.c

log.o : log.c $(headers)
	$(CC) -c log.c

temp.o : temp.c $(headers)
	$(CC) -c temp.c

clean : 	
	rm -f project_1 $(objects)
