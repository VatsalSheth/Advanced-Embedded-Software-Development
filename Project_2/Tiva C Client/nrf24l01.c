/*
 * nrf24l01.c
 *
 *  Created on: Apr 18, 2019
 *      Author: vkshe
 */

/*
 * SSI Module 2
 * SCLK: PD3
 * MOSI: PD1
 * MISO: PD0
 * CSN:  PL1
 * IRQ:  PL2
 * CE:   PL3
 */

#include "Include/nrf24l01.h"

uint32_t t[8],c=0;

/**
 * @brief Sets CSN pin of NRF24l01+ module
 */
void set_CSN()
{
    GPIOPinWrite(nrf_gpio_base, nrf_csn, nrf_csn);
}

/**
 * @brief Sets CE pin of NRF24l01+ module
 */
void set_CE()
{
    GPIOPinWrite(nrf_gpio_base, nrf_ce, nrf_ce);
}

/**
 * @brief Clears CE pin of NRF24l01+ module
 */
void clear_CE()
{
    GPIOPinWrite(nrf_gpio_base, nrf_ce, 0x00);
}

/**
 * @brief Clears CSN pin of NRF24l01+ module
 */
void clear_CSN()
{
    set_CSN();
    GPIOPinWrite(nrf_gpio_base, nrf_csn, 0x00);
}

/**
 * @brief Configures NRF24l01+ module
 */
void ConfigureNRF(void)
{
    uint8_t rc;
    uint32_t val[5];

    ConfigureSPI2();
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);    //update in case of port change
    GPIOPinTypeGPIOInput(nrf_gpio_base, nrf_irq);
    GPIOPinTypeGPIOOutput(nrf_gpio_base, nrf_ce);
    GPIOPinTypeGPIOOutput(nrf_gpio_base, nrf_csn);
    GPIOPadConfigSet(nrf_gpio_base, nrf_irq, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntRegister(nrf_gpio_base, NRF_ISR);
    GPIOIntTypeSet(nrf_gpio_base, nrf_irq, GPIO_FALLING_EDGE);
    GPIOIntEnable(nrf_gpio_base, GPIO_INT_PIN_2);       //Update in case of IRQ pin change

    SPI_rxfifo_flush();

    rc = NRF_read(R_REGISTER(CONFIG), &val[0], 1);
    rc = NRF_read(R_REGISTER(EN_AA), &val[0], 1);
    rc = NRF_read(R_REGISTER(EN_RXADDR), &val[0], 1);
    rc = NRF_read(R_REGISTER(SETUP_AW), &val[0], 1);
    rc = NRF_read(R_REGISTER(SETUP_RETR), &val[0], 1);
    rc = NRF_read(R_REGISTER(RF_CH), &val[0], 1);
    rc = NRF_read(R_REGISTER(RF_SETUP), &val[0], 1);
    rc = NRF_read(R_REGISTER(STATUS), &val[0], 1);
    rc = NRF_read(R_REGISTER(OBSERVE_TX), &val[0], 1);


    if(rc == 0x0e)
    {
        UARTprintf("NRF module success...!!! %x\n",rc);
    }
    else
    {
        UARTprintf("NRF module failed...!!! %x\n",rc);
    }

    val[0] = 0x01;
    rc = NRF_write(W_REGISTER(EN_RXADDR), &val[0], 1);    //Enable data pipe 0 Only
    val[0] = 0x06;
    val[1] = 0x00;
    val[2] = 0x00;
    val[3] = 0x00;
    val[4] = 0x00;
    rc = NRF_write(W_REGISTER(RX_ADDR_P(0)), val, 5);    //Data pipe 0 Rx addr is 0x06
    rc = NRF_write(W_REGISTER(TX_ADDR), val, 5);    //Tx addr is 0x06
    val[0] = 0x04;
    rc = NRF_write(W_REGISTER(RX_PW_P(0)), &val[0], 1);    //Static payload in Data pipe 0 is 0x04

    //val[0] = 0x0b;  //Rx
    val[0] = 0x0a;  //Tx
    rc = NRF_write(W_REGISTER(CONFIG), &val[0], 1);    //Power up mode & Tx/Rx mode
    val[0]=1;
    val[3]=2;
    rc = NRF_write(W_TX_PAYLOAD, &val[0], 4);
    val[0]=3;
    val[3]=4;
    rc = NRF_write(W_TX_PAYLOAD, &val[0], 4);
    set_CE();
}


/**
 * @brief Write to NRF24l01+ module
 * @param command NRF24l01+ command to send
 * @param data Pointer of data to send
 * @param count Number of bytes of data to send from above pointer. count=0 for read commands
 * @return Status register value
 */
uint8_t NRF_write(uint32_t command, uint32_t *data, uint8_t count)
{
    uint32_t i, temp[6];
    temp[0] = command;

    if(count)
    {
        for(i=1; i<=count; i++)
        {
            temp[i] = data[i-1];
        }
        clear_CSN();
    }

    SPIsend(temp, count+1);
    if(count)
    {
        set_CSN();
    }

    SPIreceive(temp, count+1);
    SPI_rxfifo_flush();

    return temp[0];
}

/**
 * @brief Read from NRF24l01+ module
 * @param command NRF24l01+ command to read
 * @param data Pointer of data to receive
 * @param count Number of bytes of data to receive from above pointer
 * @return Status register value
 */
uint8_t NRF_read(uint32_t command, uint32_t *data, uint8_t count)
{
    uint8_t rc;
    uint32_t tmp[5] = {NOP, NOP, NOP, NOP, NOP};

    clear_CSN();
    rc = NRF_write(command, 0, 0);
    SPIsend(tmp, count);
    SPIreceive(data, count);
    set_CSN();
    return rc;
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
    NRF_read(R_REGISTER(STATUS), &i, 1);
    NRF_write(W_REGISTER(STATUS), &i, 1);
    /*NRF_read(R_RX_PAYLOAD, &t[c], 4);
    c += 4;
    if(c>=8)
    {
        c=0;
    }*/
    for(i=0; i<1000; i++);
    GPIOIntEnable(nrf_gpio_base, GPIO_INT_PIN_2);
}
