/**
 * File: actuator.c
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file creates task for actuators which control LCD and LED
 * Date: 4/29/2019
 */


#include "Include/actuator.h"

float back_lat, back_lon;

uint32_t Actuator_task()
{
    UARTprintf("Actuator Task started...!!!\n");

    if(xTaskCreate(Actuator_fn, (const char *)"Actuator", 128, NULL, configMAX_PRIORITIES-1, NULL) != pdTRUE)
    {
        UARTprintf("Actuator Task not created...!!!\n");
        return(1);
    }

    return(0);
}

void Actuator_fn(void* pvParameters)
{
    uint32_t accident_flag, gps_flag, mpu_flag;
    struct tx_packet lcd_pk;
    float deg;

    ConfigureLED();
    ConfigureLCD();
    lcdgotoaddr(Satellite_Addr-2);
    lcdputstr("S:");
    lcdgotoaddr(Rf_Addr-3);
    lcdputstr("RF:");
    lcdgotoaddr(Lat_Addr-4);
    lcdputstr("LAT");
    lcdgotoaddr(Lon_Addr-4);
    lcdputstr("LON");
    lcdgotoaddr(Dir_Addr-3);
    lcdputstr("DIR");

    accident_flag = 0;
    gps_flag = 0;
    mpu_flag = 0;

    while(1)
    {
        if(xQueueReceive(xQueue_a, (void*)&lcd_pk, portMAX_DELAY) == pdPASS)
        {
            lcdgotoaddr(Rf_Addr);
            if(rf_link == 0)
            {
                lcdputstr("Down");
                deg = gps_course_to(lcd_pk.latitude, lcd_pk.longitude, back_lat, back_lon);
                lcdgotoaddr(Dir_Addr);
                lcdputfloat(deg);
            }
            else
            {
                lcdputstr("Up  ");
            }

            if(lcd_pk.status & 0x80)
            {
                if(lcd_pk.status & Reached)
                {
                    lcdgotoaddr(Dir_Addr-3);
                    lcdputstr("Dest Reached !!!");
                    vTaskDelete(xHandle_s);
                    vTaskDelete(xHandle_rf);
                    vTaskDelete(NULL);
                }

                if(lcd_pk.status & Misled)
                {
                    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x01);
                }
                else
                {
                    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x00);
                }

                back_lat = lcd_pk.latitude;
                back_lon = lcd_pk.longitude;

                lcdgotoaddr(Dir_Addr);
                if(lcd_pk.status & Fwd)
                {
                    lcdputstr(" F");
                }
                if(lcd_pk.status & Back)
                {
                    lcdputstr(" B");
                }
                if(lcd_pk.status & Right)
                {
                    lcdputstr(" R");
                }
                if(lcd_pk.status & Left)
                {
                    lcdputstr(" L");
                }


            }
            else
            {
                if((lcd_pk.status & 0x01) && (accident_flag==0))
                {
                    lcdgotoaddr(Accident_Addr);
                    lcdputch('A');
                    accident_flag = 1;
                }
                else if((!(lcd_pk.status & 0x01)) && (accident_flag==1))
                {
                    lcdgotoaddr(Accident_Addr);
                    lcdputch(' ');
                    accident_flag = 0;
                }

                if((lcd_pk.status & 0x10) && (gps_flag==0))
                {
                    lcdgotoaddr(GPS_Addr);
                    lcdputch('G');
                    gps_flag = 1;
                }
                else if((!(lcd_pk.status & 0x10)) && (gps_flag==1))
                {
                    lcdgotoaddr(GPS_Addr);
                    lcdputch(' ');
                    gps_flag = 0;
                }

                if((lcd_pk.status & 0x20) && (mpu_flag==0))
                {
                    lcdgotoaddr(MPU_Addr);
                    lcdputch('M');
                    mpu_flag = 1;
                }
                else if((!(lcd_pk.status & 0x20)) && (mpu_flag==1))
                {
                    lcdgotoaddr(MPU_Addr);
                    lcdputch(' ');
                    mpu_flag = 0;
                }
                lcdgotoaddr(Satellite_Addr);
                lcdputint(lcd_pk.t_s);
                lcdgotoaddr(Lat_Addr);
                lcdputfloat(lcd_pk.latitude);
                lcdgotoaddr(Lon_Addr);
                lcdputfloat(lcd_pk.longitude);
            }
        }
    }
}

void ConfigureLED(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x00);
}
