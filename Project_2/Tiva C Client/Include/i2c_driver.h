/**
 * File: i2c_driver.h
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file contains required include and declarations of variables and functions used by i2c_driver.c
 * Date: 4/29/2019
 */

#ifndef INCLUDE_I2C_DRIVER_H_
#define INCLUDE_I2C_DRIVER_H_

#include "basic.h"

#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "driverlib/i2c.h"

void ConfigureI2C2(void);
void I2Csend(uint8_t addr, uint8_t *data, uint8_t count);
void I2Creceive(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t count);

#endif /* INCLUDE_I2C_DRIVER_H_ */
