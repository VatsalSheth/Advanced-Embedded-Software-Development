/****************************************************************************************************************************************
/*File Name : Inter-Process Communication using POSIX Message Queue, Process 2
/*Author : Vatsal Sheth											
/*Dated 2/28/2019												
/* Code description : Two way communicatin is being performed between two processes. Both processes send as well as receive data in the 
/* form of a structure of string, strings's length and a command for LED.							  	
/* ************************************************************************************************************************************/

#include <pthread.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>           
#include <sys/stat.h>        
#include <string.h>
#include <time.h>
#include <sys/syscall.h>
#include <signal.h>

#define queue_name "/ipc_queue"
#define log_file "log_signal.txt"
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
mqd_t q_id;

struct timespec start = {0,0};
struct timespec final = {0,0};
struct timespec diff = {0,0};
	
void time_diff(struct timespec *time2, struct timespec *time1, struct timespec *time_res);
void set_signal_handler(void);

int main()
{
	struct mq_attr q_attr;
	struct data send[10], receive[10];
	char tmp[32];
	int rc, i, rnd;
	
	clock_gettime(CLOCK_REALTIME, &start);
	
	set_signal_handler();
	srand(time(0));
	
	clock_gettime(CLOCK_REALTIME, &final);
	time_diff(&final, &start, &diff);
	
	fp = fopen(log_file, "a");
	if(fp == NULL)
	{
		printf("Error in opening file %s from main thread\n",log_file);
	}
	fprintf(fp, "\n[%ld : %ld : %ld : %ld] Process 2 Main Thread IPC: POSIX Queue:\nPOSIX thread ID: %ld\nLinux thread ID %ld\n\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec, pthread_self(), syscall(SYS_gettid));
	
	q_attr.mq_flags = 0;
    q_attr.mq_maxmsg = 10;
    q_attr.mq_msgsize = 50;
    q_attr.mq_curmsgs = 0;
	
	q_id = mq_open(queue_name, O_RDWR, 0664, &q_attr);
	if(q_id == -1)
		handle_error("mq_open");
	
	for(i=0; i<10; i++)
	{	
		rnd = rand();
		sprintf(tmp, "String with random number %d",rnd%100);
		strcpy(send[i].str, tmp);
		send[i].string_length = strlen(send[i].str);
		send[i].led = rnd%2;
		
		clock_gettime(CLOCK_REALTIME, &final);
		time_diff(&final, &start, &diff);
		
		rc = mq_send(q_id, (const char *)&send[i], sizeof(struct data), 0); 
		if(rc == -1)
			handle_error("mq_send");
		
		fprintf(fp, "[%ld : %ld : %ld : %ld] Sent: String = \"%s\"\tLength = %d\tLED = %d\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec, send[i].str, send[i].string_length, send[i].led);
		
		rc = mq_receive (q_id, (char*)&receive[i], 50, NULL);
		if(rc == -1)
			handle_error("mq_receive");
		
		clock_gettime(CLOCK_REALTIME, &final);
		time_diff(&final, &start, &diff);
		fprintf(fp, "[%ld : %ld : %ld : %ld] Received: String = \"%s\"\tLength = %d\tLED = %d\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec, receive[i].str, receive[i].string_length, receive[i].led);
	}
	
	rc = mq_close(q_id);
	if(rc == -1)
		handle_error("mq_close");
	
	rc = mq_unlink(queue_name);
	if(rc == -1)
		handle_error("mq_unlink");

    rc = fclose(fp);
	if(rc != 0)
		handle_error("fclose");
		
	return 0;
}

void time_diff(struct timespec *time2, struct timespec *time1, struct timespec *time_res)
{
  int dt_sec=time2->tv_sec - time1->tv_sec;
  int dt_nsec=time2->tv_nsec - time1->tv_nsec;

  if(dt_sec >= 0)
  {
    if(dt_nsec >= 0)
    {
      time_res->tv_sec=dt_sec;
      time_res->tv_nsec=dt_nsec;
    }
    else
    {
      time_res->tv_sec=dt_sec-1;
      time_res->tv_nsec=1000000000+dt_nsec;
    }
  }
}

void signal_handler(int signo, siginfo_t *info, void *extra) 
{
	int rc;
	
	clock_gettime(CLOCK_REALTIME, &final);
	time_diff(&final, &start, &diff);
	fprintf(fp, "[%ld : %ld : %ld : %ld] SIGINT encountered...!!!\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec); 
	
	rc = mq_close(q_id);
	if(rc == -1)
		handle_error("mq_close");
	
	rc = mq_unlink(queue_name);
	if(rc == -1)
		handle_error("mq_unlink");

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
