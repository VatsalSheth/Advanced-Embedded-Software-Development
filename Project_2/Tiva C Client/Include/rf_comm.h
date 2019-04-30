/**
 * File: rf_comm.h
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file contains required include and declarations of variables and functions used by rf_comm.c
 * Date: 4/29/2019
 */

#ifndef INCLUDE_RF_COMM_H_
#define INCLUDE_RF_COMM_H_

#include "nrf24l01.h"

#define Accident (0x01)
#define Time_exc (0x02)
#define N_ignore (0x04)
#define Continue (0x08)
#define GPS (0x10)
#define ACCEL (0x20)

extern QueueHandle_t xQueue_rf, xQueue_a;
extern SemaphoreHandle_t xSemaphore_rf;
extern TaskHandle_t xHandle_rf;
extern uint32_t rf_link;

void Comm_fn(void* pvParameters);
uint32_t Comm_task();
extern void NRF_ISR(void);
void ConfigureTIMER1();
void setTIMER1(uint8_t sec);

#endif /* INCLUDE_RF_COMM_H_ */
