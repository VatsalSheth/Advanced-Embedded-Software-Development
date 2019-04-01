objects = main.o log.o temp.o light.o socket.o gpio.o apds.o tmp102.o 
headers = ../Include/main.h ../Include/log.h ../Include/temp.h ../Include/light.h ../Include/socket.h ../Include/apds.h ../Include/gpio.h ../Include/tmp102.h

objects_test = log.o temp.o light.o socket.o gpio.o apds.o tmp102.o unit_test.o
CC = gcc


project_1 : $(objects) $(headers)
	$(CC) -g -o project_1 $(objects) -lpthread -lrt -lm

unit_test:  $(objects_test) $(headers)
	$(CC) -g -o unit_test $(objects_test) -lpthread -lrt -lm
	
unit_test.o: unit_test.c $(headers)
	$(CC) -c unit_test.c 
		
main.o : main.c $(headers)
	$(CC) -c main.c

log.o : log.c $(headers)
	$(CC) -c log.c

temp.o : temp.c $(headers)
	$(CC) -c temp.c

light.o : light.c $(headers)
	$(CC) -c light.c

socket.o : socket.c $(headers)
	$(CC) -c socket.c

apds.o : apds.c $(headers)
	$(CC) -c apds.c

tmp102.o : tmp102.c $(headers)
	$(CC) -c tmp102.c

gpio.o : gpio.c $(headers)
	$(CC) -c gpio.c 

clean : 	
	rm -f project_1 unit_test unit_test.o $(objects)
