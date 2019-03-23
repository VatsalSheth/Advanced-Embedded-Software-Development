//#ifndef LOG_H_
//#define LOG_H_
//#include "log.h"
//#endif
#include "light.h"
mqd_t temp_queue_fd;
struct mq_attr temp_queue_attr;
uint32_t rc_temp;
pthread_t temp_th;

void* temp_func(void*);
void temp_exit(void);
void temp_queue_init(void);
