objects = main.o 
headers = ../Include/main.h 

CC = gcc


project_2 : $(objects) $(headers)
	$(CC) -g -o project_2 $(objects) -lpthread -lrt -lm

main.o : main.c $(headers)
	$(CC) -c main.c

clean : 	
	rm -f project_2 $(objects)
