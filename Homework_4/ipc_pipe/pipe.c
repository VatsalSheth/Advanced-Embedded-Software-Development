 /****************************************************************************************************************************************
/*File Name : Inter-Process Communication using Pipe
/*Author : Vatsal Sheth											
/*Dated 2/28/2019												
/* Code description : Two way communicatin is being performed between two processes. Both processes send as well as receive data in the 
/* form of a structure of string, strings's length and a command for LED.						  	
/* ************************************************************************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <wait.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>  

#define log_file "log.txt"
#define handle_error(msg) \
			{ perror(msg); \
			exit(EXIT_FAILURE); }

struct data
{
	char str[32];
	int string_length;
	int led;
};

FILE *fp;
struct timespec diff1 = {0,0};
struct timespec diff2 = {0,0};
useconds_t ut = 10000;

void set_signal_handler(void);

int main()
{
	struct data send1, send2, receive1, receive2;
	char tmp[32];
	int rc1, rc2, i, j, rnd, rnd1;
	int fd1[2], fd2[2];
	pid_t childpid;
	
	remove(log_file);	
	set_signal_handler();
	
	rc1 = pipe(fd1);
	if(rc1 == -1)
		handle_error("pipe1");
		
	rc1 = pipe(fd2);
	if(rc1 == -1)
		handle_error("pipe2");
	
	srand(time(0));
	
	fp = fopen(log_file, "a");
	if(fp == NULL)
	{
		printf("Error in opening file %s from main thread\n",log_file);
	}
		
	pthread_mutex_t lock;
	rc1 = pthread_mutex_init(&lock, NULL);
	if(rc1 != 0)
		handle_error("mutex init");
		
	childpid = fork();
	
	if(childpid > 0)
	{
		close(fd1[0]);
		close(fd2[1]);
		
		clock_gettime(CLOCK_REALTIME, &diff1);
		pthread_mutex_lock(&lock);
		fprintf(fp, "\n[%ld : %ld : %ld : %ld] Parent Main IPC: Pipe:\nProcess ID: %d\n\n",diff1.tv_sec, (diff1.tv_nsec / 1000000), (diff1.tv_nsec / 1000), diff1.tv_nsec, getpid());
		pthread_mutex_unlock(&lock);
		
		for(i = 0; i<10; i++)
		{
			rnd = rand();
			sprintf(tmp, "String with random number %d",rnd%100);
			strcpy(send1.str, tmp);
			send1.string_length = strlen(send1.str);
			send1.led = rnd%2;
			
			clock_gettime(CLOCK_REALTIME, &diff1);
			
			rc1 = write(fd1[1], (char *)&send1, sizeof(struct data));
			if(rc1 == -1)
				handle_error("write");
			
			pthread_mutex_lock(&lock);
			fprintf(fp, "[%ld : %ld : %ld : %ld] Sent from parent: String = \"%s\"\tLength = %d\tLED = %d\n",diff1.tv_sec, (diff1.tv_nsec / 1000000), (diff1.tv_nsec / 1000), diff1.tv_nsec, send1.str, send1.string_length, send1.led);
			pthread_mutex_unlock(&lock);
			
			usleep(ut);
			
			rc1 = read(fd2[0], (char *)&receive1, sizeof(struct data));
			if(rc1 == -1)
				handle_error("readp");
				
			clock_gettime(CLOCK_REALTIME, &diff1);
			pthread_mutex_lock(&lock);
			fprintf(fp, "[%ld : %ld : %ld : %ld] Received by parent: String = \"%s\"\tLength = %d\tLED = %d\n",diff1.tv_sec, (diff1.tv_nsec / 1000000), (diff1.tv_nsec / 1000), diff1.tv_nsec, receive1.str, receive1.string_length, receive1.led);
			pthread_mutex_unlock(&lock);
		}		
		rc1 = fclose(fp);
		if(rc1 != 0)
			handle_error("fclose");
	}
	else
	{
		close(fd1[1]);
		close(fd2[0]);
		clock_gettime(CLOCK_REALTIME, &diff2);
		pthread_mutex_lock(&lock);
		fprintf(fp, "\n[%ld : %ld : %ld : %ld] Child Main IPC: Pipe:\nProcess ID: %d\n\n",diff2.tv_sec, (diff2.tv_nsec / 1000000), (diff2.tv_nsec / 1000), diff2.tv_nsec, getpid());
		pthread_mutex_unlock(&lock);
		
		for(j=0; j<10; j++)
		{
			usleep(ut);
			
			rc2 = read(fd1[0], (char *)&receive2, sizeof(struct data));
			if(rc2 == -1)
				handle_error("readc");
				
			clock_gettime(CLOCK_REALTIME, &diff2);
			pthread_mutex_lock(&lock);
			fprintf(fp, "[%ld : %ld : %ld : %ld] Received by child: String = \"%s\"\tLength = %d\tLED = %d\n",diff2.tv_sec, (diff2.tv_nsec / 1000000), (diff2.tv_nsec / 1000), diff2.tv_nsec, receive2.str, receive2.string_length, receive2.led);
			pthread_mutex_unlock(&lock);
			
			rnd1 = rand();
			sprintf(tmp, "String with random number %d",rnd1%100);
			strcpy(send2.str, tmp);
			send2.string_length = strlen(send2.str);
			send2.led = rnd1%2;
			
			clock_gettime(CLOCK_REALTIME, &diff2);
			
			rc2 = write(fd2[1], (char *)&send2, sizeof(struct data));
			if(rc2 == -1)
				handle_error("write");
			
			pthread_mutex_lock(&lock);
			fprintf(fp, "[%ld : %ld : %ld : %ld] Sent from child: String = \"%s\"\tLength = %d\tLED = %d\n",diff2.tv_sec, (diff2.tv_nsec / 1000000), (diff2.tv_nsec / 1000), diff2.tv_nsec, send2.str, send2.string_length, send2.led);
			pthread_mutex_unlock(&lock);
		}		
	}
	
	rc1 = pthread_mutex_destroy(&lock);
	if(rc1 == -1)
		handle_error("mutex destroy");
	
	while(1) {}   // To check SIGINT
		
	return 0;
}

void signal_handler(int signo, siginfo_t *info, void *extra) 
{
	int rc;
	struct timespec diff = {0,0};
	
	clock_gettime(CLOCK_REALTIME, &diff);
	fprintf(fp, "[%ld : %ld : %ld : %ld] SIGINT encountered...!!!\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec); 
	
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

