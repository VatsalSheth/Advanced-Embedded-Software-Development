/**
 * File: lcd_driver.h
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file contains required include and declarations of variables and functions used by lcd_driver.c
 * Date: 4/29/2019
 */

/*
 *  D0-7: M0-7
 *  EN: F1
 *  RS: F2
 *  RW: F3
 */

#ifndef INCLUDE_LCD_DRIVER_H_
#define INCLUDE_LCD_DRIVER_H_

#include "basic.h"

#define data_gpio_base   GPIO_PORTM_BASE
#define ctrl_gpio_base   GPIO_PORTF_BASE

#define EN     GPIO_PIN_1
#define RS     GPIO_PIN_2
#define RW     GPIO_PIN_3

extern uint8_t row_addr[4];

void ConfigureLCD(void);
void WR_CMD(uint8_t data);
uint8_t RD_CMD(void);
void WR_DATA(uint8_t data);
uint8_t RD_DATA(void);
void lcdbusywait();
void lcdgotoaddr(uint8_t addr);
void lcdgotoxy(uint8_t row, uint8_t column);
void lcdputch(char cc);
void lcdputstr(char *ss);
void lcdcreatechar(uint8_t ccode, uint8_t row_vals[8]);
void lcdclear();
void lcdputfloat(float a);
void lcdputint(int a);

#endif /* INCLUDE_LCD_DRIVER_H_ */
