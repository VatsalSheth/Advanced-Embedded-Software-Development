/***********************************************************************************
* @temp.h
* @This file contains dependent include files and variable declaration for temp.c
*
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#ifndef LOG_H_
#define LOG_H_
#include "log.h"
#endif

#include "tmp102.h"

#define LOW_THRESHOLD 	(23)
#define HIGH_THRESHOLD 	(24)

pthread_t temp_th;
int rc_temp;
struct log_msg temp_data;

void* temp_func(void*);
void temp_exit(void);
void temp_entry(void);
float request_temp(void);
float conv_temp(float, char);
