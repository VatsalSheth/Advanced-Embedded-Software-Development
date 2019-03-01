CC = arm-linux-gcc

all: server client

server: server.c
	$(CC) server.c -o server -lpthread -lrt
	
client: client.c
	$(CC) client.c -o client -lpthread -lrt
	
clean:
	rm -f client server
