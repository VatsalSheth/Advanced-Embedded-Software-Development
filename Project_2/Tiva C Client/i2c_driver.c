#include "Include/i2c_driver.h"

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
