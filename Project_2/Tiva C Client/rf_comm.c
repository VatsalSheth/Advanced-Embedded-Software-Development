/**
 * File: rf_comm.c
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file creates task for rf comunication control
 * Date: 4/29/2019
 */


#include "Include/rf_comm.h"

TaskHandle_t xHandle_rf;
uint32_t tx_flag, rx_flag, max_flag, tx_cont, rf_link;
struct tx_packet rx_pk;

uint32_t Comm_task()
{
    UARTprintf("RF Communication Task started...!!!\n");

    if(xTaskCreate(Comm_fn, (const char *)"RF Comm", 128, NULL, configMAX_PRIORITIES, &xHandle_rf) != pdTRUE)
    {
        UARTprintf("RF Communication Task not created...!!!\n");
        return(1);
    }

    return(0);
}

void Comm_fn(void* pvParameters)
{
    struct tx_packet tx_pk;
    UBaseType_t uxNumberOfFreeSpaces;

    tx_flag = 1;
    rx_flag = 0;
    max_flag = 0;
    tx_cont = 0;
    rf_link = 0;

    ConfigureTIMER1();
    ConfigureNRF();
    set_txmode();
    clear_CE();

    while(1)
    {
        if(tx_flag == 1)
        {
            tx_flag = 0;
            if((xQueueReceive(xQueue_rf, (void*)&tx_pk, portMAX_DELAY) == pdPASS))
            {
                tx_cont = 0;
                uxNumberOfFreeSpaces = uxQueueSpacesAvailable(xQueue_rf);
                if(uxNumberOfFreeSpaces < 10)
                {
                    if(!(tx_pk.status & 0x02))
                    {
                        tx_pk.status |= Continue;
                        tx_cont = 1;
                    }
                }
                NRF_write(W_TX_PAYLOAD, (uint8_t *)&tx_pk, sizeof(struct tx_packet));
                set_CE();
            }
        }

        if(rx_flag == 1)
        {
            rx_flag = 0;
            NRF_read(R_RX_PAYLOAD, (uint8_t *)&rx_pk, sizeof(struct tx_packet)-4);
            if(rx_pk.status == 0x40)
            {
                set_CE();
            }
            else
            {
                rx_pk.status |= 0x80;
                xQueueSendToBack(xQueue_a, (void*)&rx_pk, pdMS_TO_TICKS(10));
                set_txmode();
                tx_flag = 1;
            }
        }
    }
}

/**
 * @brief ISR of NRF24l01+ module
 */
void NRF_ISR(void)
{
    uint32_t i;
    GPIOIntDisable(nrf_gpio_base, GPIO_INT_PIN_2);
    i = GPIOIntStatus(nrf_gpio_base, false);
    GPIOIntClear(nrf_gpio_base, i);
    i = NRF_read(R_REGISTER(STATUS), (uint8_t *)&i, 1);
    NRF_write(W_REGISTER(STATUS), (uint8_t *)&i, 1);

    if(i & 0x10)
    {
        if(max_flag == 0)
        {
            setTIMER1(9);
        }
        max_flag = 1;
    }
    else if(i & 0x20)
    {
        rf_link = 1;
        if(tx_cont)
        {
            tx_flag = 1;
        }
        else
        {
            clear_CE();
            set_rxmode();
            setTIMER1(8);
            set_CE();
        }
    }
    else if(i & 0x40)
    {
        rf_link = 1;
        rx_flag = 1;
        clear_CE();
    }

    for(i=0; i<100; i++);
    GPIOIntEnable(nrf_gpio_base, GPIO_INT_PIN_2);
}

void ConfigureTIMER1(void)
{
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    ROM_IntEnable(INT_TIMER1A);
    ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
}

void setTIMER1(uint8_t sec)
{
    ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, g_ui32SysClock*sec);
    ROM_TimerEnable(TIMER1_BASE, TIMER_A);
}

void Timer1IntHandler(void)
{
    ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    ROM_IntMasterDisable();

    if(max_flag == 1)
    {
        max_flag = 0;
        rf_link = 0;
    }
    else if((tx_flag==0) && (rx_flag==0))
    {
        clear_CE();
        ROM_TimerDisable(TIMER1_BASE, TIMER_A);
        set_txmode();
        tx_flag = 1;
        setTIMER1(2);
    }

    ROM_IntMasterEnable();
}
