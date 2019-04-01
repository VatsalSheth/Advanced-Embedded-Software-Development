/***********************************************************************************
* @socket.h
* @This file contains dependent include files and variable declaration for socket.c
*
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#ifndef LOG_H_
#define LOG_H_
#include "log.h"
#endif

#define _GNU_SOURCE 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERV_TCP_PORT 	(8010)
#define MAX_SIZE 	(80)
#define BACKLOG 	(5)

struct sockaddr_in serv_addr, cli_addr;
pthread_t socket_th;
int rc_socket, len;
int server_fd, newserver_fd, clilen, port;

void* socket_func(void*);
void socket_exit(void);
void socket_entry(void);
void soc_queue_init(void);
