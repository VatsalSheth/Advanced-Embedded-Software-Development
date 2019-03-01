 /****************************************************************************************************************************************
/*File Name : Inter-Process Communication using Socket, Client
/*Author : Vatsal Sheth											
/*Dated 2/28/2019												
/* Code description : Two way communicatin is being performed between two processes. Both processes send as well as receive data in the 
/* form of a structure of string, strings's length and a command for LED.	
 * Reference: http://www.it.uom.gr/teaching/distrubutedSite/dsIdaLiu/labs/lab2_1/sockets.html						  	
/* ************************************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <fcntl.h>           
#include <sys/stat.h> 

#define handle_error(msg) \
			{ perror(msg); \
			exit(EXIT_FAILURE); }

#define SERVER_PORT 4000 
#define log_file "log.txt"

struct data
{
	char str[32];
	int string_length;
	int led;
};

FILE *fp;
int sock_fd;

struct timespec diff = {0,0};
	
void set_signal_handler(void);

int main()
{
	struct sockaddr_in server_addr;
	char *server_host = "localhost";
	struct hostent *host_p;
	int port, i, rnd, rc;
	struct data send[10], receive[10];
	char tmp[32];
	
	port = SERVER_PORT;
	
	set_signal_handler();
	srand(time(0));
	
	clock_gettime(CLOCK_REALTIME, &diff);
	
	fp = fopen(log_file, "a");
	if(fp == NULL)
	{
		printf("Error in opening file %s from main thread\n",log_file);
	}
	fprintf(fp, "\n[%ld : %ld : %ld : %ld] Client Main Thread IPC: Sockets:\nProcess ID: %d\n\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec, getpid());
	
	host_p = gethostbyname(server_host);
	if(host_p == NULL) 
		handle_error("gethostbyname");
  
	if(host_p->h_addrtype !=  AF_INET) 
		handle_error("unknown address type");
	
	memset(&server_addr, 0, sizeof(struct sockaddr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = ((struct in_addr *)host_p->h_addr_list[0])->s_addr;
	server_addr.sin_port = htons(port);
  
	rc = sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(rc < 0) 
		handle_error("can't open stream socket");
    
    rc = connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if(rc < 0) 
		handle_error("can't connect to server");
    
    for(i=0; i<10; i++)
	{	
		rnd = rand();
		sprintf(tmp, "String with random number %d",rnd%100);
		strcpy(send[i].str, tmp);
		send[i].string_length = strlen(send[i].str);
		send[i].led = rnd%2;
		
		clock_gettime(CLOCK_REALTIME, &diff);
		
		rc = write(sock_fd, (char *)&send[i], sizeof(struct data));
		if(rc == -1)
			handle_error("write");
				
		fprintf(fp, "[%ld : %ld : %ld : %ld] Sent: String = \"%s\"\tLength = %d\tLED = %d\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec, send[i].str, send[i].string_length, send[i].led);
		
		rc = read(sock_fd, (char *)&receive[i], 50);
		if(rc == -1)
			handle_error("read");
				
		clock_gettime(CLOCK_REALTIME, &diff);
		fprintf(fp, "[%ld : %ld : %ld : %ld] Received: String = \"%s\"\tLength = %d\tLED = %d\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec, receive[i].str, receive[i].string_length, receive[i].led);
	} 
	 
	rc = close(sock_fd); 
	if(rc == -1)
		handle_error("close socket");
	
	rc = fclose(fp);
	if(rc != 0)
		handle_error("fclose");
			
	return 0;
}

void signal_handler(int signo, siginfo_t *info, void *extra) 
{
	int rc;
	
	clock_gettime(CLOCK_REALTIME, &diff);
	fprintf(fp, "[%ld : %ld : %ld : %ld] SIGINT encountered...!!!\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec); 
	
	rc = close(sock_fd); 
	if(rc == -1)
		handle_error("close socket");
	
	rc = fclose(fp);
	if(rc != 0)
		handle_error("fclose");
	
	exit(0);
}

void set_signal_handler(void)
{
        struct sigaction action;
 
        action.sa_flags = SA_SIGINFO; 
        action.sa_sigaction = signal_handler;
 
        if (sigaction(SIGINT, &action, NULL) == -1) { 
            perror("sigusr1: sigaction");
            _exit(1);
        }
}

