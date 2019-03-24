#ifndef LOG_H_
#define LOG_H_
#include "log.h"
#endif

pthread_t socket_th;
int rc_socket;

void* socket_func(void*);
void socket_exit(void);
