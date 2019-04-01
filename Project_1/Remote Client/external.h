/***********************************************************************************
* @external.h
* @This file contains dependent include files and variable declaration for external.c
*
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

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

			
#define REQUEST_FAIL 		(0) 
#define REQUEST_TEMPERATURE_C 	(2)
#define REQUEST_TEMPERATURE_F 	(4)
#define REQUEST_TEMPERATURE_K 	(6)
#define REQUEST_LIGHT 	(1)
#define KILL_TEMPERATURE (11)
#define KILL_LIGHT 	(12)
#define KILL_LOGGER 	(21)
#define KILL_SOCKET 	(22)
#define STATUS_LIGHT 	(3)
#define STATUS_DARK 	(5)

#define SERV_TCP_PORT 	(8010)
#define MAX_SIZE 	(80)

struct command
{
	float sensor_data;
	uint32_t action;
}user;

struct sockaddr_in serv_addr;
struct hostent *host_ptr;
int client_fd, clilen, port, len;
char* option;
useconds_t sleepy;
uint32_t conn_flag, res_flag, conn_attempt_flag;
int rc;
char *serv_host;

char *strlwr(char *ip);
