objects = main.o 
headers = ../Include/main.h 
CC = gcc

project_1 : $(objects) $(headers)
	$(CC) -o project_1 $(objects)

main.o : main.c $(headers)
	$(CC) -c main.c

clean : 	
	rm -f project_1 $(objects)
