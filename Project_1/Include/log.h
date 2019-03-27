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

#define LOG_NONE (0)
#define LOG_DEBUG (1)

#define handle_error(msg) \
			{\
				perror(msg);\
				exit(1);\
			}
			
#define queue_name ("/log_fd")
#define socket_queue ("/socket_queue")

#define TEMP_THREAD_NUM 0
#define LIGHT_THREAD_NUM 1
#define LOG_THREAD_NUM 2
#define SOCKET_THREAD_NUM 3

#define REQUEST_TEMPERATURE_C 0
#define REQUEST_TEMPERATURE_F 2
#define REQUEST_TEMPERATURE_K 4
#define REQUEST_LIGHT 1
#define KILL_TEMPERATURE 11
#define KILL_LIGHT 12
#define KILL_LOGGER 21
#define KILL_SOCKET 22

#define NUM_OF_THREADS 4

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
}mon[NUM_OF_THREADS];

mqd_t queue_fd;
struct mq_attr queue_attr;
uint8_t socket_hb;
int rc_log;

uint32_t socket_req_id, socket_req_flag;

uint32_t data_avail;
uint32_t exit_cond;
uint32_t timer_flag[NUM_OF_THREADS - 2];
uint32_t exit_flag[NUM_OF_THREADS];
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

void queue_init(void);
void* logger_func(void*);
void log_exit(void);
void ack_heartbeat(uint32_t);
void set_notify_signal();
void notify_handler(union sigval sv);
void timer_init();
void timer_handle(union sigval sv);
