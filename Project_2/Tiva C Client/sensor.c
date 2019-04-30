/**
 * File: sensor.c
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file creates task for sensors which control GPS, MPU6050, button
 * Date: 4/29/2019
 */

#include "Include/sensor.h"

uint32_t button_flag, time_exch_flag;
struct tx_packet pk;
TaskHandle_t xHandle_s;

void Sensor_fn(void* pvParameters)
{
    TickType_t current, start;
    char cChar[255];
    int i;
    uint8_t data_avail;
    unsigned long age;
    float lat, lon;

    time_exch_flag = 0;
    button_flag = 0;

    ConfigureUART_4_GPS();
    ConfigureMPU();
    ConfigureButton();
    ConfigureTIMER();

    while(!time_exch_flag)
    {
        time_exch_flag = get_time();
    }
    xQueueSendToBack(xQueue_rf, (void*)&pk, pdMS_TO_TICKS(10));
    xQueueSendToBack(xQueue_rf, (void*)&pk, pdMS_TO_TICKS(10));
    xQueueSendToBack(xQueue_rf, (void*)&pk, pdMS_TO_TICKS(10));

    //xSemaphoreGive(xSemaphore_rf);
    start = xTaskGetTickCount();

    pk.status = 0x00;

//    while(!button_flag);

    while(1)
    {
        xSemaphoreTake(xSemaphore_s, portMAX_DELAY);

        for(i=0; i<255; i++)
        {
            //
            // Read a character using the blocking read function.  This function
            // will not return until a character is available.
            //

            cChar[i] = UARTCharGet(UART5_BASE);
            if(gps_encode(cChar[i]))
            {
                data_avail = true;
            }
        }
        if(data_avail)
        {
            data_avail = false;
            gps_f_get_position(&lat, &lon, &age);
            current = xTaskGetTickCount();
            pk.latitude = 40.0072704;//lat;
            pk.longitude = -105.2641923;//lon;
            pk.t_s = (current - start)/1000;
            pk.status |= 0x00;
            //xQueueOverwrite(xQueue_rf, (void*)&pk);
            xQueueSendToBack(xQueue_rf, (void*)&pk, pdMS_TO_TICKS(10));
            //xSemaphoreGive(xSemaphore_rf);
            pk.t_s = gps_satellites();
            xQueueSendToBack(xQueue_a, (void*)&pk, pdMS_TO_TICKS(10));
            //xQueueOverwrite(xQueue_a, (void*)&pk);

//            UARTprintf("\nLAT = %d.%d\n", (int)latitude, (int64_t)((latitude-(int)latitude)*10000000));
//            UARTprintf("LON = %d.%d\n", (int)longitude, (int64_t)((longitude-(int)longitude)*10000000));
//            UARTprintf("NUM OF SATELLITES = %d\n", satellite_num);
        }
    }
}

uint32_t Sensor_task()
{
    UARTprintf("Sensor Task started...!!!\n");

    if(xTaskCreate(Sensor_fn, (const char *)"Sensor", 256, NULL, configMAX_PRIORITIES-1, &xHandle_s) != pdTRUE)
    {
        UARTprintf("Sensor Task not created...!!!\n");
        return(1);
    }

    return(0);
}

void ConfigureTIMER(void)
{
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock*3);

    ROM_IntEnable(INT_TIMER0A);
    ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    ROM_TimerEnable(TIMER0_BASE, TIMER_A);
}

void Timer0IntHandler(void)
{
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    ROM_IntMasterDisable();
    xSemaphoreGive(xSemaphore_s);
    ROM_IntMasterEnable();
}

uint32_t get_time()
{
    uint32_t i;
    char tmp[30];

    for(i=0; i<30; i++)
    {
        tmp[i] = UARTCharGet(UART5_BASE);
        if(tmp[i] == 'R')
        {
            tmp[i] = UARTCharGet(UART5_BASE);
            if(tmp[i] == 'M')
            {
                tmp[i] = UARTCharGet(UART5_BASE);
                if(tmp[i] == 'C')
                {
                    for(i=0; i<7; i++)
                    {
                        tmp[i] = UARTCharGet(UART5_BASE);
                    }
                    pk.t_s = ((tmp[5]-0x30)*10) + (tmp[6]-0x30);
                    pk.longitude = ((tmp[3]-0x30)*10) + (tmp[4]-0x30);
                    pk.latitude = ((tmp[1]-0x30)*10) + (tmp[2]-0x30) - 6;   //UTC -6 Mountain time zone
                    if(pk.latitude < 0)
                    {
                        pk.latitude += 24;
                    }
                    pk.status = 0x00 | Time_exc;
                    return 1;
                }
            }
        }
    }
    return 0;
}
