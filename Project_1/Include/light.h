#ifndef LOG_H_
#define LOG_H_
#include "log.h"
#endif

mqd_t light_queue_fd;
struct mq_attr light_queue_attr;
pthread_t light_th;
uint32_t rc_light;

void* light_func(void*);
void light_exit(void);
void light_queue_init(void);
