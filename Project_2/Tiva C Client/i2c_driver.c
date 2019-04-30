/**
 * File: i2c_driver.c
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file has driver API for I2C
 * Date: 4/29/2019
 */

#include "Include/i2c_driver.h"

/**
 * @brief Configures I2C 2 Block
 */
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

/**
 * @brief Writes data on I2C 2 Block
 * @param addr Slave addrres
 * @param data Pointer to data to be sent
 * @param count Number of bytes to be sent from data pointer
 */
void I2Csend(uint8_t addr, uint8_t *data, uint8_t count)
{
    uint8_t i;

    I2CMasterSlaveAddrSet(I2C2_BASE, addr, false);

    for(i=0; i<count; i++)
    {
        I2CMasterDataPut(I2C2_BASE, data[i]);
        if((i==0) && (count == 1))
        {
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
        }
        else if(i==0)
        {
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);
        }
        else if(i<(count-1))
        {
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
        }
        else if(i==(count-1))
        {
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        }

        //Overcome I2C error as per ERRATA
        while(!I2CMasterBusy(I2C2_BASE));
        while(I2CMasterBusy(I2C2_BASE));
    }
}

/**
 * @brief Receives specified bytes of data from given register through I2C 2 Block
 * @param addr Slave address
 * @param reg  Register to read data from
 * @param data Pointer to received data
 * @param count Number of bytes to be received
 */
void I2Creceive(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t count)
{
    uint8_t i;

    I2CMasterSlaveAddrSet(I2C2_BASE, addr, false);
    I2CMasterDataPut(I2C2_BASE, reg);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);

    //Overcome I2C error as per ERRATA
    while(!I2CMasterBusy(I2C2_BASE));
    while(I2CMasterBusy(I2C2_BASE));

    I2CMasterSlaveAddrSet(I2C2_BASE, addr, true);

    for(i=0; i<count; i++)
    {
        if((i==0) && (count == 1))
        {
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        }
        else if(i==0)
        {
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
        }
        else if(i<(count-1))
        {
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
        }
        else if(i==(count-1))
        {
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
        }


        //Overcome I2C error as per ERRATA
        while(!I2CMasterBusy(I2C2_BASE));
        while(I2CMasterBusy(I2C2_BASE));

        data[i] = I2CMasterDataGet(I2C2_BASE);
    }
}
