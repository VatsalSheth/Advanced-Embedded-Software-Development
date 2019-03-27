#ifndef LOG_H_
#define LOG_H_
#include "log.h"
#endif

pthread_t socket_th;
int rc_socket;
mqd_t soc_queue_fd;
struct mq_attr soc_queue_attr;

void* socket_func(void*);
void socket_exit(void);
void soc_queue_init(void);
