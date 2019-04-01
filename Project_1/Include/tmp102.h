/***********************************************************************************
* @tmp102.h
* @This file contains dependent include files and variable declaration for tmp102.c
*
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#ifndef LOG_H_
#define LOG_H_
#include "log.h"
#endif

#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <math.h>

#define TEMP_SLAVE_ADDRESS 	(0x48)
#define TEMP_REG 	(0x00)
#define CONFIG_REG 	(0x01)
#define TLOW_REG 	(0x02)
#define THIGH_REG 	(0x03)

#define SHUTDOWN_MODE 	(0x0100)
#define THERMOSTAT_MODE (0x0200)
#define FAULT_ONE 	(0x0000)
#define FAULT_TWO 	(0x0800)
#define FAULT_FOUR 	(0x1000)
#define FAULT_SIX 	(0x1800)
#define EXTENDED_MODE 	(0x0010)
#define RATE_QUARTER 	(0x0000)
#define RATE_ONE 	(0x0040)
#define RATE_FOUR 	(0x0080)
#define RATE_EIGHT 	(0x000C0)

#define CONV_RES_MASK 	(0x600)

#define FAULT 	(0)
#define EM 	(1)
#define CONV	(2)

int tsense_fd, tsense_check, rc_tsense;

int temp_sensor_init(void);
int write_pointer_reg(uint8_t);
int read_reg(uint8_t);
int write_config_reg(uint16_t);
uint16_t* read_config_reg(uint16_t*);
int write_tlow_reg(float);
int write_thigh_reg(float);
int read_temp_reg(void);

float temp_calc(void);
