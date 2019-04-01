/***********************************************************************************
* @log.h
* @This file contains dependent include files and variable declaration for log.c
*
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <mqueue.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <poll.h>

#include "gpio.h"

#define LOG_NONE 	(0)
#define LOG_DEBUG 	(1)
#define ERROR_MESSAGE 	(2)

#define TEMP_THREAD_NUM 	(0)
#define LIGHT_THREAD_NUM 	(1)
#define LOG_THREAD_NUM 		(2)
#define SOCKET_THREAD_NUM 	(3)
#define MAIN_THREAD_NUM 	(99)

#define REQUEST_FAIL 		(0) 
#define REQUEST_TEMPERATURE_C 	(2)
#define REQUEST_TEMPERATURE_F 	(4)
#define REQUEST_TEMPERATURE_K 	(6)
#define REQUEST_LIGHT 		(1)
#define KILL_TEMPERATURE 	(11)
#define KILL_LIGHT 		(12)
#define KILL_LOGGER 		(21)
#define KILL_SOCKET 		(22)
#define STATUS_LIGHT 		(3)
#define STATUS_DARK 		(5)

#define NUM_OF_THREADS 		(4)

#define handle_error(msg) \
			{\
				perror(msg);\
				kill(getpid(), SIGINT);\
			}

//#define log_error(msg) \
//			{\
				
			
#define handle_error_exit(msg) \
			{\
				perror(msg);\
				exit(0);\
			}
			
#define handle_error_print(msg) \
			{\
				perror(msg);\
			}
					
#define queue_name 	("/log_fd")
#define socket_queue 	("/socket_queue")

struct command
{
	float sensor_data;
	uint32_t action;
};

struct heartbeat_monitor
{
	pthread_cond_t cond;
	pthread_mutex_t lock;
	struct timespec timeout;
	uint32_t count;
}mon[NUM_OF_THREADS];

mqd_t queue_fd, soc_queue_fd;
struct mq_attr queue_attr, soc_queue_attr;
uint8_t socket_hb;
int rc_log;

uint32_t socket_req_id, socket_req_flag;

uint32_t data_avail;
uint32_t exit_cond;
uint32_t timer_flag[NUM_OF_THREADS - 2];
uint32_t exit_flag[NUM_OF_THREADS];
char* bist_buffer[2];
timer_t timer_id;

FILE* file_log;
pthread_t log_th;
struct sigevent sev;

struct log_msg
{
	uint32_t id;
	float data;
	struct timespec time_stamp;
	uint32_t verbosity;
	char* debug_msg;
};
 
struct log_param
{
	uint32_t log_verbosity;
	char* file_name;
};

struct log_param log_file;

void queue_init(void);
void* logger_func(void*);
void log_exit(void);
void log_entry(void);
void ack_heartbeat(uint32_t);
void set_notify_signal();
void notify_handler(union sigval sv);
void timer_init();
void timer_handle(union sigval sv);
struct log_msg write_to_log_queue(uint32_t thread_id,
				    float thread_data,
				    uint32_t msg_verbosity,
				    char* msg);
