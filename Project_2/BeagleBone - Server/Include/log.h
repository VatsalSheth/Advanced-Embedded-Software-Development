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

#define POLL_IRQ	(99)
#define TIMER_ISR	(100)

#define handle_error(msg) \
			{\
				perror(msg);\
				exit(1);\
			}
					
#define queue_name 	("/log_fd")
#define filename 	("/debug.txt")

mqd_t queue_fd, soc_queue_fd;
struct mq_attr queue_attr, soc_queue_attr;
int rc_log;

FILE* file_log;
pthread_t log_th;

struct log_msg
{
	uint8_t id;
	float lat, lon;
	uint8_t hours, minutes;
	uint32_t seconds;
	uint8_t status;

};

void queue_init(void);
void* logger_func(void*);
void log_exit(void);
void log_entry(void);
struct log_msg write_to_log_queue(uint32_t thread_id,
				    float thread_data,
				    uint32_t msg_verbosity,
				    char* msg);
