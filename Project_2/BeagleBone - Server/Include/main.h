/***********************************************************************************
* @main.h
* @This file contains dependent include files and variable declaration for main.c
*
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#include <unistd.h>
#include <fcntl.h>           
#include <sys/stat.h>        
#include <string.h>
#include <sys/syscall.h>
#include <signal.h>
#include <time.h>
#include <poll.h>
#include <pthread.h>
#include "nrf24L01.h"
#include "map.h"
#include "log.h"

#define IN_RANGE	(1)
#define OUT_RANGE	(0)
#define NO_VALID_DATA	(-1)

#define DEPT_PHY_LATI	(40.0082101)
#define DEPT_PHY_LONGI	(-105.2676004)
#define C4C_LATI	(40.0043916)
#define C4C_LONGI	(-105.2649423)

void* NRF_ISR(void* threadp);
void update_tiva_time(uint32_t);
uint8_t compare_time(void);

void time_timer_init(void);
void time_timer_handle(union sigval sv);
void watchdog_timer_init(void);
void watchdog_timer_handle(union sigval sv);

int gpio_nrf_fd;
float backup_lati, backup_longi;
pthread_t int_th;

timer_t time_timer_id;
struct itimerspec time_trigger;
timer_t watchdog_timer_id;
struct itimerspec watchdog_trigger;

struct timetrack
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
}local, tiva_t;
