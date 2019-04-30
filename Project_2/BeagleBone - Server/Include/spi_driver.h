/***********************************************************************************
* @spi_driver.h
* @This file contains dependent include files and variable declaration for spi_driver.c
*
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <unistd.h>
#include "gpio.h"

#define WRITE		(0x20)
#define READ		(0x00)

#define handle_error(msg) \
			{\
				perror(msg);\
				exit(1);\
			}
//				kill(getpid(), SIGINT);\

static const char *device = "/dev/spidev1.0";
static uint32_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 500000;
int spi_fd;
int rc_check;
uint8_t byte[5];

void spi_dev_init(void);
uint8_t spi_write(uint8_t, uint8_t*, uint8_t);
uint8_t spi_read(uint8_t, uint8_t*, uint8_t);

#endif
