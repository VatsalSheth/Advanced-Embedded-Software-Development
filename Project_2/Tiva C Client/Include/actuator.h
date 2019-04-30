/**
 * File: actuator.h
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file contains required include and declarations of variables and functions used by actuator.c
 * Date: 4/29/2019
 */

#ifndef INCLUDE_ACTUATOR_H_
#define INCLUDE_ACTUATOR_H_

#include "lcd_driver.h"

#define Satellite_Addr (0x02)
#define Rf_Addr (0x08)
#define Accident_Addr (0x0f)
#define GPS_Addr (0x0d)
#define MPU_Addr (0x0e)
#define Lat_Addr (0x44)
#define Lon_Addr (0x14)
#define Dir_Addr (0x53)

#define Fwd (0x01)
#define Back (0x02)
#define Right (0x04)
#define Left (0x08)
#define Misled (0x10)
#define Reached (0x20)

extern QueueHandle_t xQueue_rf, xQueue_a;
extern SemaphoreHandle_t xSemaphore_rf;
extern TaskHandle_t xHandle_s, xHandle_rf;
extern uint32_t rf_link;

uint32_t Actuator_task();
void Actuator_fn(void* pvParameters);
void ConfigureLED(void);
extern float gps_course_to (float lat1, float long1, float lat2, float long2);

#endif /* INCLUDE_ACTUATOR_H_ */
