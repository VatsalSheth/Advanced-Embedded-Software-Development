/**
 * File: main.c
 * Author: Vatsal Sheth
 * Description:
 * Date: 4/29/2019
 * Refrence: Tivaware demo codes
 */

#include "Include/main.h"

volatile unsigned long g_vulRunTimeStatsCountValue;

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
    uint32_t temp[3] = {0xaa, 0x55, 0x32};
    uint32_t *ans;
    //
    // Configure the system frequency.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);
    //ans = (uint32_t *)malloc(sizeof(uint32_t)*3);
    ConfigureUART();
    ConfigureSPI2();
    SPIsend(temp, 3);
    ans = SPIreceive(3);
    UARTprintf("Data is %d, %d, %d\n",ans[0], ans[1],ans[2]);

    //vTaskStartScheduler();

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
