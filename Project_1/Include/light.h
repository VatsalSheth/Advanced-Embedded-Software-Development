/***********************************************************************************
* @light.h
* @This file contains dependent include files and variable declaration for light.c
*
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#ifndef LOG_H_
#define LOG_H_
#include "log.h"
#endif

#include "apds.h"

#define CH0_LOW_THRESHOLD 	(40)
#define CH0_HIGH_THRESHOLD 	(600)

mqd_t light_queue_fd, light_soc_queue_fd;
struct mq_attr light_queue_attr, light_soc_queue_attr;
pthread_t light_th;
int rc_light, light_status;
struct log_msg light_data;

void* light_func(void*);
void light_exit(void);
void light_entry(void);
float request_light(void);
