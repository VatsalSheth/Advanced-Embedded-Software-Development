/**
 * File: main.h
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file contains required include and declarations of variables and functions used by main.c
 * Date: 4/29/2019
 */

#ifndef INCLUDE_MAIN_H_
#define INCLUDE_MAIN_H_

#include "Include/basic.h"

#include "driverlib/interrupt.h"
#include "drivers/pinout.h"

#include "Include/actuator.h"
#include "Include/sensor.h"
#include "Include/rf_comm.h"

extern QueueHandle_t xQueue_rf, xQueue_a;
extern SemaphoreHandle_t xSemaphore_rf, xSemaphore_s;

#endif /* INCLUDE_MAIN_H_ */
