CC = arm-linux-gcc

all: process1 process2

process1: process1.c
	$(CC) process1.c -o process1 -lpthread -lrt
	
process2: process2.c
	$(CC) process2.c -o process2 -lpthread -lrt
	
clean:
	rm -f process1 process2
