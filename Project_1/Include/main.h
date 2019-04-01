/***********************************************************************************
* @main.h
* @This file contains dependent include files and variable declaration for main.c
*
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#include <unistd.h>
#include <fcntl.h>           
#include <sys/stat.h>        
#include <string.h>
#include <sys/syscall.h>
#include <signal.h>
#include <time.h>

#include "temp.h"
#include "light.h"
#include "socket.h"

struct log_msg main_data;

pthread_t int_th; 
int rc;
int gpio_fd[2];
useconds_t usec;


int arg_init(char *arg1, char *arg2);
void thread_create(void);
void thread_join(void);

void light_sensor_bist(void);
void temp_sensor_bist(void);

void set_signal_handler();
void signal_handler(int signo, siginfo_t *info, void *extra);
void heartbeat_check(void);
void* int_func(void*);
void int_exit(void);
