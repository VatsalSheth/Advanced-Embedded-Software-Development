/**
 * File: mpu6050.c
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file has driver API for MPU6050 sensor
 * Date: 4/29/2019
 */

#include "Include/mpu6050.h"

/**
 * @brief Configure I2C 2 block and Gpio pin for interrupt from MPU6050. Writes initialization registers in MPU and configures DMP engine.
 * @return 1 on BIST success and 0 on BIST failure
 */
uint8_t ConfigureMPU(void)
{
    uint8_t val[2];
    float tmp;

    ConfigureI2C2();
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);    //update in case of port change
    GPIOPinTypeGPIOInput(mpu_gpio_base, mpu_irq);
    GPIOPinTypeGPIOOutput(mpu_gpio_base, mpu_ad0);
    GPIOPinWrite(mpu_gpio_base, mpu_ad0, 0x00);        //AD0 = 0
    GPIOPadConfigSet(mpu_gpio_base, mpu_irq, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntRegister(mpu_gpio_base, MPU_ISR);
    GPIOIntTypeSet(mpu_gpio_base, mpu_irq, GPIO_FALLING_EDGE);
    GPIOIntEnable(mpu_gpio_base, GPIO_INT_PIN_2);       //Update in case of IRQ pin change

    val[0] = 0x6b;
    val[1] = 0x00;
    I2Csend(SLAVE_ADDR, val, 2);    //PWR_MGMT_1 = 0x00
    I2Creceive(SLAVE_ADDR, 0x41, val, 2);
    tmp = (val[0]<<8) | val[1];
    tmp = (((int16_t)tmp)/340) + 36.53;
    val[0] = 0x68;
    val[1] = 0x07;
    I2Csend(SLAVE_ADDR, val, 2);    //SIGNAL_PATH_RESET = 0x07  Reset gyro, accel, temp
    val[0] = 0x37;
    val[1] = 0xa0;
    I2Csend(SLAVE_ADDR, val, 2);    //INT_PIN_CFG = 0xa0  interrupt pin active low, push-pull, latch interrupt till cleared, cleared by reading INT_STATUS register
    val[0] = 0x1c;
    val[1] = 0x01;
    I2Csend(SLAVE_ADDR, val, 2);
    val[0] = 0x1f;
    val[1] = 0x0f;
    I2Csend(SLAVE_ADDR, val, 2);    //MOT_THR = 0x14 Motion Detection Threshold set to 80
    val[0] = 0x20;
    val[1] = 0x0f;
    I2Csend(SLAVE_ADDR, val, 2);    //MOT_DUR = 0xff Motion Detection Duration set to 255 ms
    val[0] = 0x69;
    val[1] = 0x30;
    I2Csend(SLAVE_ADDR, val, 2);    //MOT_DETECT_CTRL = 0x30 Additional accel wake up delay of 3 ms, reset motion detect counter to 0 if any non qualified sample detected
    val[0] = 0x38;
    val[1] = 0x40;
    I2Csend(SLAVE_ADDR, val, 2);    //INT_ENABLE = 0x40 Enable motion detection interrupt
    return BIST_mpu();
}

/**
 * @brief Reads the WHO AM I register on MPU6050 to verify the identity
 * @return 1 on success and 0 on failure
 */
uint8_t BIST_mpu()
{
    uint8_t val;
    I2Creceive(SLAVE_ADDR, 0x75, &val, 1);
    val &= 0x7e;
    if(val == (SLAVE_ADDR & 0x7e))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief ISR of MPU6050 module
 */
void MPU_ISR(void)
{
    uint32_t i;
    GPIOIntDisable(mpu_gpio_base, GPIO_INT_PIN_2);
    i = GPIOIntStatus(mpu_gpio_base, false);
    GPIOIntClear(mpu_gpio_base, i);
    I2Creceive(SLAVE_ADDR, 0x3a, (uint8_t *)&i, 1);
    if(time_exch_flag)
    {
        button_flag = 1;
        pk.status |= 0x01;
        xSemaphoreGive(xSemaphore_s);
    }
    GPIOIntEnable(mpu_gpio_base, GPIO_INT_PIN_2);
}
