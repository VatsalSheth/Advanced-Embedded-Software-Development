/**
 * File: spi_driver.h
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file contains required include and declarations of variables and functions used by spi_driver.c
 * Date: 4/29/2019
 */

#ifndef INCLUDE_SPI_DRIVER_H_
#define INCLUDE_SPI_DRIVER_H_

#include "basic.h"

#include "driverlib/ssi.h"

void ConfigureSPI2(void);
void SPIsend(uint32_t *data, uint32_t count);
void SPIreceive(uint32_t *data, uint32_t count);
void SPI_rxfifo_flush(void);

#endif /* INCLUDE_SPI_DRIVER_H_ */
