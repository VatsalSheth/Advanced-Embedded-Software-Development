/**
 * File: main.c
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file calls functions for task, semaphore and queu creation.
 * Date: 4/29/2019
 * Refrence: Tivaware demo codes
 */

#include "Include/main.h"

volatile unsigned long g_vulRunTimeStatsCountValue;
QueueHandle_t xQueue_rf, xQueue_a;
SemaphoreHandle_t xSemaphore_rf, xSemaphore_s;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif


//*****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// Initialize FreeRTOS and start the initial set of tasks.
//
//*****************************************************************************
int main(void)
{
    //
    // Configure the system frequency.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);

    ConfigureUART();

    xSemaphore_rf = xSemaphoreCreateBinary();
    if(xSemaphore_rf == NULL)
    {
        UARTprintf("Binary semaphore not created...!!!\n");
    }

    xSemaphore_s = xSemaphoreCreateBinary();
    if(xSemaphore_s == NULL)
    {
        UARTprintf("Binary semaphore not created...!!!\n");
    }

    xQueue_rf = xQueueCreate(10, sizeof(struct tx_packet));
    if(xQueue_rf == NULL)
    {
        UARTprintf("Failed to create RF queue ...!!!\n");
    }

    xQueue_a = xQueueCreate(5, sizeof(struct tx_packet));
    if(xQueue_a == NULL)
    {
        UARTprintf("Failed to create actuator queue ...!!!\n");
    }

    if(Comm_task() != 0)
    {
        while(1)
        {
        }
    }

    if(Sensor_task() != 0)
    {
        while(1)
        {
        }
    }

    if(Actuator_task() != 0)
    {
        while(1)
        {
        }
    }

    vTaskStartScheduler();

    //
    // In case the scheduler returns for some reason, print an error and loop
    // forever.
    //
    UARTprintf("RTOS scheduler returned unexpectedly.\n");
    while(1)
    {
        //
        // Do Nothing.
        //
    }
}
