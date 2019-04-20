/*
 * button.c
 *
 *  Created on: Apr 18, 2019
 *      Author: vkshe
 */

#include "Include/button.h"

/**
 * @brief Configure On board user button on PJ0
 */
void ConfigureButton()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntRegister(GPIO_PORTJ_BASE, Button_ISR);
    GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_INT_PIN_0);
}

/**
 * @brief Button click
 */
void Button_ISR(void)
{
    uint32_t i;
    GPIOIntDisable(GPIO_PORTJ_BASE, GPIO_INT_PIN_0);
    i = GPIOIntStatus(GPIO_PORTJ_BASE, false);
    GPIOIntClear(GPIO_PORTJ_BASE, i);
    for(i=0; i<1000; i++);
    GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_INT_PIN_0);
}
