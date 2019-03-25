#ifndef LOG_H_
#define LOG_H_
#include "log.h"
#endif

mqd_t temp_queue_fd;
struct mq_attr temp_queue_attr;
pthread_t temp_th;
int rc_temp;
struct log_msg temp_data;

void* temp_func(void*);
void temp_exit(void);
void temp_queue_init(void);
