/**
 * File: button.h
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file contains required include and declarations of variables and functions used by button.c
 * Date: 4/29/2019
 */

#ifndef INCLUDE_BUTTON_H_
#define INCLUDE_BUTTON_H_

#include "basic.h"

extern uint32_t button_flag;
extern struct tx_packet pk;

void Button_ISR(void);
void ConfigureButton();

#endif /* INCLUDE_BUTTON_H_ */
