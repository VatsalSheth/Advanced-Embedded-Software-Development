/***********************************************************************************
* @apds.h
* @This file contains dependent include files and variable declaration for apds.c
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

#define READ 		(0x01)
#define WRITE 		(0x00)
#define LIGHT_SLAVE_ADDRESS 	(0x39)
#define COMMAND 	(0x80)

#define CONTROL_REG 	(0x00)
#define TIMING_REG	(0x01)
#define INT_TH_LL_REG 	(0x02)
#define INT_TH_LH_REG 	(0x03)
#define INT_TH_HL_REG 	(0x04)
#define INT_TH_HH_REG 	(0x05)
#define INT_CTL_REG 	(0x06)
#define ID_REG 		(0x0A)
#define ADC_DATA0L_REG 	(0x0C)
#define ADC_DATA0H_REG 	(0x0D)
#define ADC_DATA1L_REG 	(0x0E)
#define ADC_DATA1H_REG 	(0x0F)

#define POWER_UP 	(0x03)
#define POWER_DOWN 	(0x00)

#define CONTROL_REG_MASK (0x03)

#define ADC0_ERROR	(-1)
#define ADC1_ERROR	(-2)

int lsense_fd, lsense_check, rc_lsense;

int light_sensor_init(void);
int write_command_reg(uint8_t);
int write_control_reg(uint8_t);
uint8_t read_control_reg();
int write_int_ctl_reg(uint8_t); 
uint8_t read_int_ctl_reg(void); 
int write_timing_reg(uint8_t);
uint8_t read_timing_reg(void);

int write_int_th_reg(uint16_t, uint8_t);
uint16_t read_int_th_reg(uint8_t);

int sensor_id(void);

uint16_t ch_ADC0(void);
uint16_t ch_ADC1(void);
float lux_calc(void);
