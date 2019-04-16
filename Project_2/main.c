/**
 * File: main.c
 * Author: Vatsal Sheth
 * Description: Code creates 4 tasks which controls led toggle, TMP102 temp sensor interface,
 * logging mechanishm, temperature alert notification.
 * Date: 4/10/2019
 * Refrence: Tivaware demo codes,
 * Digikey TM4C123GXL I2C application note: https://www.digikey.com/eewiki/display/microcontroller/I2C+Communication+with+the+TI+Tiva+TM4C123GXL
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "drivers/pinout.h"
#include "drivers/buttons.h"


#define LED_TASK (1)
#define TEMP_TASK (2)
#define ALERT_TASK (3)
#define SLAVE_ADDR (0x48)
#define THRESHOLD (25)

volatile unsigned long g_vulRunTimeStatsCountValue;

struct log_data
{
    uint32_t id;
    float data;
    TickType_t stamp;
};

QueueHandle_t xQueue;
TaskHandle_t xHandle_alert;
TickType_t START;
//*****************************************************************************
//
// Global variable to hold the system clock speed.
//
//*****************************************************************************
uint32_t g_ui32SysClock;
volatile uint32_t led_timer_flag, temp_timer_flag;

uint32_t led_task();
void led_fn(void* pvParameters);

uint32_t log_task();
void log_fn(void* pvParameters);

uint32_t temp_task();
void temp_fn(void* pvParameters);

uint32_t alert_task();
void alert_fn(void* pvParameters);

void log_value(struct log_data val);
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


void Timer0IntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    temp_timer_flag = 1;
}

void Timer1IntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    led_timer_flag = 1;
}
//*****************************************************************************
//
// Configure and start the timer that will increment the variable used to
// track FreeRTOS task statistics.
//
//*****************************************************************************
void ConfigureTimer(void)
{
    //
    // Enable the peripherals used by this example.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    //
    // Configure the two 32-bit periodic timers.  The period of the timer for
    // FreeRTOS run time stats must be at least 10 times faster than the tick
    // rate.
    //
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock);
    ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, g_ui32SysClock / 10);

    //
    // Setup the interrupts for the timer timeouts.
    //
    ROM_IntEnable(INT_TIMER0A);
    ROM_IntEnable(INT_TIMER1A);
    ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerEnable(TIMER1_BASE, TIMER_A);

}

void ConfigureI2C2(void)
{
    //enable I2C module 2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);

    //reset module
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);

    //enable GPIO peripheral that contains I2C 2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    // Configure the pin muxing for I2C2 functions on port N4 and N5.
    GPIOPinConfigure(GPIO_PN5_I2C2SCL);
    GPIOPinConfigure(GPIO_PN4_I2C2SDA);

    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTN_BASE, GPIO_PIN_5);
    GPIOPinTypeI2C(GPIO_PORTN_BASE, GPIO_PIN_4);

    // Enable and initialize the I2C2 master module.  Use the system clock for
    // the I2C2 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(I2C2_BASE, SysCtlClockGet(), false);
}

void I2Csend(uint8_t addr, uint8_t data)
{
    I2CMasterSlaveAddrSet(I2C2_BASE, addr, false);
    I2CMasterDataPut(I2C2_BASE, data);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    //Overcome I2C error as per ERRATA
    while(!I2CMasterBusy(I2C2_BASE));
    while(I2CMasterBusy(I2C2_BASE));
}

uint32_t I2Creceive(uint8_t addr, uint8_t reg)
{
    uint32_t data;

    I2CMasterSlaveAddrSet(I2C2_BASE, addr, false);
    I2CMasterDataPut(I2C2_BASE, reg);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);

    //Overcome I2C error as per ERRATA
    while(!I2CMasterBusy(I2C2_BASE));
    while(I2CMasterBusy(I2C2_BASE));

    I2CMasterSlaveAddrSet(I2C2_BASE, addr, true);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

    //Overcome I2C error as per ERRATA
    while(!I2CMasterBusy(I2C2_BASE));
    while(I2CMasterBusy(I2C2_BASE));

    data = I2CMasterDataGet(I2C2_BASE);
    data = data<<8;

    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

    //Overcome I2C error as per ERRATA
    while(!I2CMasterBusy(I2C2_BASE));
    while(I2CMasterBusy(I2C2_BASE));

    data |= I2CMasterDataGet(I2C2_BASE);

    return data;
}

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
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
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

//*****************************************************************************
//
// Initialize FreeRTOS and start the initial set of tasks.
//
//*****************************************************************************
int
main(void)
{
    //
    // Configure the system frequency.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);

    START = xTaskGetTickCount();

    ConfigureUART();
    ConfigureTimer();
    ConfigureI2C2();

    xQueue = xQueueCreate(5, sizeof(struct log_data));
    if(xQueue == NULL)
    {
        UARTprintf("Failed to create queue ...!!!\n");
    }

    if(led_task() != 0)
    {
        while(1)
        {
        }
    }

    if(log_task() != 0)
    {
        while(1)
        {
        }
    }

    if(temp_task() != 0)
    {
        while(1)
        {
        }
    }

    if(alert_task() != 0)
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

uint32_t led_task()
{
    UARTprintf("LED task started...!!!\n");

    if(xTaskCreate(led_fn, (const char *)"LED", 256, NULL, tskIDLE_PRIORITY, NULL) != pdTRUE)
    {
        UARTprintf("LED task not created...!!!\n");
        return(1);
    }

    return(0);
}

uint32_t log_task()
{
    UARTprintf("Log task started...!!!\n");

    if(xTaskCreate(log_fn, (const char *)"LOG", 256, NULL, tskIDLE_PRIORITY, NULL) != pdTRUE)
    {
        UARTprintf("Log task not created...!!!\n");
        return(1);
    }

    return(0);
}

uint32_t temp_task()
{
    UARTprintf("Temp task started...!!!\n");

    if(xTaskCreate(temp_fn, (const char *)"TEMP", 256, NULL, tskIDLE_PRIORITY, NULL) != pdTRUE)
    {
        UARTprintf("Temp task not created...!!!\n");
        return(1);
    }

    return(0);
}

uint32_t alert_task()
{
    UARTprintf("Alert task started...!!!\n");

    if(xTaskCreate(alert_fn, (const char *)"ALERT", 256, NULL, tskIDLE_PRIORITY, &xHandle_alert) != pdTRUE)
    {
        UARTprintf("Alert task not created...!!!\n");
        return(1);
    }

    return(0);
}

void led_fn(void* pvParameters)
{
    static uint32_t cnt;
    TickType_t current;
    struct log_data tx_data;

    tx_data.id = LED_TASK;

    /* Already configured in I2C2 initialization
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }
    */

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    while(1)
    {
        if(led_timer_flag == 1)
        {
            led_timer_flag = 0;
            current = xTaskGetTickCount();
            cnt++;
            if(cnt & 0x01)
            {
                GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0x01);
            }
            else
            {
                GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0x02);
            }
            tx_data.data = cnt;
            tx_data.stamp = current - START;
            if(xQueueSendToBack(xQueue, (void*)&tx_data, pdMS_TO_TICKS(10)) != pdPASS)
            {
                UARTprintf("LED task queue send fail\n");
            }
        }
    }
}

void log_fn(void* pvParameters)
{
    struct log_data rx_data;

    while(1)
    {
        if(xQueueReceive(xQueue, (void*)&rx_data, portMAX_DELAY) == pdPASS)
        {
            log_value(rx_data);
        }
    }
}

void temp_fn(void* pvParameters)
{
    TickType_t current;
    struct log_data tx_data;

    tx_data.id = TEMP_TASK;

    while(1)
    {
        if(temp_timer_flag == 1)
        {
            temp_timer_flag = 0;
            current = xTaskGetTickCount();
            tx_data.data = I2Creceive(SLAVE_ADDR, 0x00);
            tx_data.data = (uint32_t)tx_data.data >> 4;
            tx_data.data *= 0.0625;
            if(tx_data.data > THRESHOLD)
            {
                xTaskNotify(xHandle_alert, 0, eNoAction);
            }
            tx_data.stamp = current - START;
            if(xQueueSendToBack(xQueue, (void*)&tx_data, pdMS_TO_TICKS(10)) != pdPASS)
            {
                UARTprintf("TEMP task queue send fail\n");
            }
        }
    }
}

void alert_fn(void* pvParameters)
{
    TickType_t current;
    struct log_data tx_data;

    tx_data.id = ALERT_TASK;
    tx_data.data = 0;

    while(1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        current = xTaskGetTickCount();
        tx_data.stamp = current - START;
        if(xQueueSendToBack(xQueue, (void*)&tx_data, pdMS_TO_TICKS(10)) != pdPASS)
        {
            UARTprintf("ALERT task queue send fail\n");
        }
    }
}

void log_value(struct log_data val)
{
    if(val.id == LED_TASK)
    {
        UARTprintf("LED Task @ %d msec : Toggle count is %d\n",val.stamp, (uint32_t)val.data);
    }
    else if(val.id == TEMP_TASK)
    {
        UARTprintf("TEMP Task @ %d msec : Temperature is %d.%d C\n",val.stamp, (int)val.data, (((uint32_t)(val.data*100))%100));
    }
    else if(val.id == ALERT_TASK)
    {
        UARTprintf("ALERT Task @ %d msec : Temperature overshoot...!!!\n",val.stamp);
    }
}
