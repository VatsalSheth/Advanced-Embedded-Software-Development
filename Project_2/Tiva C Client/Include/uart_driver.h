/**
 * File: uart_driver.h
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file contains required include and declarations of variables and functions used by uart_driver.c
 * Date: 4/29/2019
 */

/*
 * Rx PC6
 * Tx PC7
 */

#ifndef INCLUDE_UART_DRIVER_H_
#define INCLUDE_UART_DRIVER_H_

#include "basic.h"
#include "driverlib/uart.h"

void ConfigureUART(void);
void ConfigureUART_4_GPS(void);

#endif /* INCLUDE_UART_DRIVER_H_ */
