#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mqueue.h>
#include <pthread.h>
#include <errno.h>

#define LOG_NONE (0)
#define LOG_DEBUG (1)

#define handle_error(msg) \
			{\
				perror(msg); \
				exit(1);\
			}
#define queue_name ("/log_queue")

mqd_t queue_fd;
struct mq_attr queue_attr;
FILE* file_log;
pthread_t log_th;
uint32_t len, check;

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

void queue_init(void);
void* logger_func(void*);
void log_exit(void);
