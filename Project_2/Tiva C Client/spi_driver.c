/*
 * spi_driver.c
 *
 *  Created on: Apr 17, 2019
 *      Author: Vatsal Sheth
 */

/*
 * SSI Module 2
 * SCLK: PD3
 * MOSI: PD1
 * MISO: PD0
 * CS: PD7
 */

#include "Include/spi_driver.h"

void ConfigureSPI2(void)
{
    uint32_t tmp;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    GPIOPinConfigure(GPIO_PD3_SSI2CLK);
    GPIOPinConfigure(GPIO_PD2_SSI2FSS);
    GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);
    GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);

    GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_2);

    SSIConfigSetExpClk(SSI2_BASE, g_ui32SysClock, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 1000000, 8);

    SSIEnable(SSI2_BASE);

    while(SSIDataGetNonBlocking(SSI2_BASE, &tmp))
    {
    }
}

void SPIsend(uint32_t *data, uint32_t count)
{
    uint32_t i;

    for(i=0; i<count; i++)
    {
        SSIDataPut(SSI2_BASE, data[i]);
    }

    while(SSIBusy(SSI2_BASE))
    {
    }
}

uint32_t * SPIreceive(uint32_t count)
{
    uint32_t i;
    uint32_t data[4];

    for(i=0; i<count; i++)
    {
        SSIDataGet(SSI2_BASE, &data[i]);
        data[i] &= 0xff;
    }
}

void SPIslaveselect()
{

}
