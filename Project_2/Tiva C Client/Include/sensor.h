/**
 * File: sensor.h
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file contains required include and declarations of variables and functions used by sensor.c
 * Date: 4/29/2019
 */

#ifndef INCLUDE_SENSOR_H_
#define INCLUDE_SENSOR_H_

#include <string.h>
#include "mpu6050.h"
#include "uart_driver.h"
#include "button.h"
#include "gps_driver.h"

#define Accident (0x01)
#define Time_exc (0x02)
#define N_ignore (0x04)
#define Continue (0x08)
#define GPS (0x10)
#define ACCEL (0x20)

extern QueueHandle_t xQueue_rf, xQueue_a;
extern SemaphoreHandle_t xSemaphore_rf, xSemaphore_s;
extern uint32_t time_exch_flag, button_flag;
extern struct tx_packet pk;
extern TaskHandle_t xHandle_s;

uint32_t timer_flag;

uint32_t Sensor_task();
void Sensor_fn(void* pvParameters);
void ConfigureTIMER(void);
uint32_t get_time();

#endif /* INCLUDE_SENSOR_H_ */
