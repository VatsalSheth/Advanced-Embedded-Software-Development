/**
 * File: uart_driver.c
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file contains API for UART
 * Date: 4/29/2019
 */
#include "Include/uart_driver.h"

/**
 * @brief Configure the UART and its pins.  This must be called before UARTprintf().
 */
void ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

/**
 * @brief Configure the UART and its pins for GPS sensor.
 */
void ConfigureUART_4_GPS(void)
{
    //
    // Enable the GPIO Peripheral used by the GPS sensor.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    //
    // Enable UART5
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);

    //
    // Configure GPIO Pins C6 and C7 for GPS UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PC6_U5RX);
    ROM_GPIOPinConfigure(GPIO_PC7_U5TX);
    ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    //
    // Use the system 120MHz clock as the UART clock source.
    //
    UARTClockSourceSet(UART5_BASE, UART_CLOCK_SYSTEM);
    UARTConfigSetExpClk(UART5_BASE, g_ui32SysClock, 9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
    UARTEnable(UART5_BASE);
}


