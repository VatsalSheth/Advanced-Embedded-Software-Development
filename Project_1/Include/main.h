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

#include "log.h" 

pthread_t log_th, temp_th, light_th, socket_th;
int rc;
struct log_param log_file;

int arg_init(char *arg1, char *arg2);
void thread_create();
void set_signal_handler();
void signal_handler(int signo, siginfo_t *info, void *extra);
