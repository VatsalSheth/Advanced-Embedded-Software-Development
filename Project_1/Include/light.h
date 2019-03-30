#ifndef LOG_H_
#define LOG_H_
#include "log.h"
#endif

pthread_t light_th;
int rc_light;
struct log_msg light_data;

void* light_func(void*);
void light_exit(void);
float request_light(void);
