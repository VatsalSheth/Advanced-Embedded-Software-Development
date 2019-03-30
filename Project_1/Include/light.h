#ifndef LOG_H_
#define LOG_H_
#include "log.h"
#endif

#include "apds.h"

mqd_t light_queue_fd, light_soc_queue_fd;
struct mq_attr light_queue_attr, light_soc_queue_attr;
pthread_t light_th;
int rc_light;
struct log_msg light_data;

void* light_func(void*);
void light_exit(void);
void light_queue_init(void);
float request_light(void);
