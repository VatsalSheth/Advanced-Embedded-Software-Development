/**
 * File: mpu6050.h
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file contains required include and declarations of variables and functions used by mpu6050.c
 * Date: 4/29/2019
 */
/*
 *  SCL PN5
 *  SDA PN4
 *  IRQ PB2
 *  AD0 PB3     //AD0 = 0
 */

#ifndef INCLUDE_MPU6050_H_
#define INCLUDE_MPU6050_H_

#include "basic.h"
#include "i2c_driver.h"

#define SLAVE_ADDR (0x68)    // ADDR = (0x68 | (AD0 & 0x01))

#define mpu_gpio_base (GPIO_PORTB_BASE)
#define mpu_irq  (GPIO_PIN_2)
#define mpu_ad0  (GPIO_PIN_3)

extern uint32_t button_flag, time_exch_flag;
extern struct tx_packet pk;
extern SemaphoreHandle_t xSemaphore_s;

uint8_t ConfigureMPU(void);
void MPU_ISR(void);
uint8_t BIST_mpu(void);

#endif /* INCLUDE_MPU6050_H_ */
