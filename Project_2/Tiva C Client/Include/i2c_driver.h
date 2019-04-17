/*
 * i2c_driver.h
 *
 *  Created on: Apr 17, 2019
 *      Author: vkshe
 */

#ifndef INCLUDE_I2C_DRIVER_H_
#define INCLUDE_I2C_DRIVER_H_

#include "basic.h"

#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "driverlib/i2c.h"

void ConfigureI2C2(void);
void I2Csend(uint8_t addr, uint8_t data);
uint32_t I2Creceive(uint8_t addr, uint8_t reg);

#endif /* INCLUDE_I2C_DRIVER_H_ */
