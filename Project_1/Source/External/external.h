#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define REQUEST_TEMPERATURE 0
#define REQUEST_LIGHT 1
#define KILL_TEMPERATURE 11
#define KILL_LIGHT 12
#define KILL_LOGGER 21
#define KILL_SOCKET 22

#define SERV_TCP_PORT 8001 
#define MAX_SIZE 80

struct command
{
	float sensor_data;
	uint32_t action;
};

struct sockaddr_in serv_addr;
struct hostent *host_ptr;
int client_fd, newclient_fd, clilen, port, len;
char* option;
useconds_t sleepy;
