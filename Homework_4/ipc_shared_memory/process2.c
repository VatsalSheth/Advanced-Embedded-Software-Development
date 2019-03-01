/****************************************************************************************************************************************
/*File Name : Inter-Process Communication using Shared Memory, Process 2
/*Author : Vatsal Sheth											
/*Dated 2/28/2019												
/* Code description : Two way communicatin is being performed between two processes. Both processes send as well as receive data in the 
/* form of a structure of string, strings's length and a command for LED.							  	
/* ************************************************************************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>           
#include <sys/stat.h>        
#include <string.h>
#include <time.h>
#include <sys/syscall.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <semaphore.h>

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

struct timespec diff = {0,0};
FILE *fp;
const char *mem_name = "/mem";
const char *sem1_name = "/sem1";
const char *sem2_name = "/sem2";
sem_t *sem1_fd, *sem2_fd;

void set_signal_handler(void);

int main()
{
	struct data send[10], receive[10];
	char tmp[32];
	int rc, i, rnd, shm_fd;
	void *ptr;
	
	set_signal_handler();
	
	srand(time(0));
	clock_gettime(CLOCK_REALTIME, &diff);
	
	fp = fopen(log_file, "a");
	if(fp == NULL)
	{
		printf("Error in opening file %s from main thread\n",log_file);
	}
	fprintf(fp, "\n[%ld : %ld : %ld : %ld] Process 2 Main Thread IPC: Shared Memory:\nProcess ID: %d\n\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec, getpid());
	
	shm_fd = shm_open(mem_name, O_RDWR, 0664);
	if(shm_fd == -1)
		handle_error("shm_open");
	
	sem1_fd = sem_open(sem1_name, O_EXCL, 0664, 0);
	if(sem1_fd == SEM_FAILED)
		handle_error("sem_open");
		
	sem2_fd = sem_open(sem2_name, O_EXCL, 0664, 0);
	if(sem2_fd == SEM_FAILED)
		handle_error("sem_open");
				
	rc = ftruncate(shm_fd, 256);
	if(rc == -1)
		handle_error("ftruncate");
	
	ptr = mmap(NULL, 256, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
	if(ptr == MAP_FAILED)
		handle_error("mmap");
	
	ptr += sizeof(struct data);
	
	for(i=0; i<10; i++)
	{	
		sem_wait(sem1_fd);
		
		ptr -= sizeof(struct data);
		memcpy((void*)&receive[i], ptr, sizeof(struct data));
				
		clock_gettime(CLOCK_REALTIME, &diff);
		fprintf(fp, "[%ld : %ld : %ld : %ld] Received: String = \"%s\"\tLength = %d\tLED = %d\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec, receive[i].str, receive[i].string_length, receive[i].led);

		rnd = rand();
		sprintf(tmp, "String with random number %d",rnd%100);
		strcpy(send[i].str, tmp);
		send[i].string_length = strlen(send[i].str);
		send[i].led = rnd%2;
		
		clock_gettime(CLOCK_REALTIME, &diff);
				
		memcpy(ptr, (const void*)&send[i], sizeof(struct data));
		ptr += sizeof(struct data);		
		fprintf(fp, "[%ld : %ld : %ld : %ld] Sent: String = \"%s\"\tLength = %d\tLED = %d\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec, send[i].str, send[i].string_length, send[i].led);
		
		sem_post(sem2_fd);
		
	}
	
	rc = sem_close(sem1_fd);
	if(rc == -1)
		handle_error("sem_close");
		
	rc = sem_close(sem2_fd);
	if(rc == -1)
		handle_error("sem_close");
				
	rc = sem_unlink(sem1_name);
	if(rc == -1)
		handle_error("sem_unlink");
	
	rc = sem_unlink(sem2_name);
	if(rc == -1)
		handle_error("sem_unlink");
	
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
	
	rc = sem_close(sem1_fd);
	if(rc == -1)
		handle_error("sem_close");
		
	rc = sem_close(sem2_fd);
	if(rc == -1)
		handle_error("sem_close");
	
	rc = sem_unlink(sem1_name);
	if(rc == -1)
		handle_error("sem_unlink");
	
	rc = sem_unlink(sem2_name);
	if(rc == -1)
		handle_error("sem_unlink");
		
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
