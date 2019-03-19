#include "log.h"

mqd_t temp_queue_fd;
struct mq_attr temp_queue_attr;
uint32_t temp_len, temp_check;
pthread_t temp_th;

void* temp_func(void*);
void temp_exit(void);
void temp_queue_init(void);
