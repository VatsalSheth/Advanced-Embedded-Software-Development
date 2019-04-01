/***********************************************************************************
* @tmp102.c
* @This file contains a C library for accessing and writing to the registers of 
* TMP102 temperature sensor. 
* 
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#include "../Include/tmp102.h"

/**
 * @brief 
 * Initializes temperature sensor
 */
int temp_sensor_init()
{
	char*filename = "/dev/i2c-2";
	tsense_fd = open(filename, O_RDWR);
	if(tsense_fd < 0)
	{		
		perror("Failed to open the i2c bus");
		exit(1);	
	}

	int addr = TEMP_SLAVE_ADDRESS;
	if(ioctl(tsense_fd, I2C_SLAVE, addr) < 0)
	{		
		perror("Failed to acquire bus access");
		exit(1);
	}
	return tsense_fd;
}

/**
 * @brief 
 * Writes to pointer register of sensor, to specify register address to be written to next
 *
 * @return 
 */
int write_pointer_reg(uint8_t reg)
{	
	tsense_check = write(tsense_fd, &reg, 1);
	if(tsense_check != 1)
		return -1;
	return tsense_check;
}

/**
 * @brief 
 * Function fo reading and accessing any register of sensor
 * @return 
 */
int read_reg(uint8_t reg)
{
	uint8_t buf[2];
	write_pointer_reg(reg);
	tsense_check = read(tsense_fd, buf, 2);
	if(tsense_check != 2)
	{
		char* string = malloc(30);
		sprintf(string, "Failed to read from  %x register", reg);
		handle_error(string);
		free(string);
	}
	int data = (buf[0] << 4) | (buf[1] >> 4);
	return data;
}

/**
 * @brief 
 * Writes to configuration register of sensor
 *
 * @return 
 */
int write_config_reg(uint16_t data)
{
	write_pointer_reg(CONFIG_REG);
	uint8_t temp_val[3];
	temp_val[0] = CONFIG_REG;
	temp_val[1] = (int)(data) >> 4;
	temp_val[2] = (int)data << 4;
	tsense_check = write(tsense_fd, temp_val, 3);
	if(tsense_check != 3)
		handle_error("Failed to write in write_config_reg");
	write_pointer_reg(TEMP_REG);

}


uint16_t* read_config_reg(uint16_t reads[])
{
	int config = read_reg(CONFIG_REG);
	reads[FAULT] = (config & FAULT_SIX) >> 11;
	reads[EM] = (config & EXTENDED_MODE) >> 4;
	reads[CONV] = (config & RATE_EIGHT) >> 6;
	return reads;	
}

/**
 * @brief 
 * Writes to tlow register of sensor, by first dividing the data by 0.0625, then wiriting into a buffer
 * the register address, MSG of data and LSB of data
 *
 * @return 
 */
int write_tlow_reg(float data)
{
	data = data/0.0625;
	write_pointer_reg(TLOW_REG);
	uint8_t temp_val[3];
	temp_val[0] = TLOW_REG;
	temp_val[1] = (int)(data) >> 4;
	temp_val[2] = (int)(data) << 4;
	tsense_check = write(tsense_fd, temp_val, 3);
	if(tsense_check != 3)
		handle_error("Failed to write in write_tlow_reg");
	write_pointer_reg(TEMP_REG);
}


int write_thigh_reg(float data)
{
	data = data/0.0625;
	write_pointer_reg(THIGH_REG);
	uint8_t temp_val[3];
	temp_val[0] = THIGH_REG;
	temp_val[1] = (int)(data) >> 4;
	temp_val[2] = (int)data << 4;
	tsense_check = write(tsense_fd, temp_val, 3);
	if(tsense_check != 3)
		handle_error("Failed to write in write_thigh_reg");
	write_pointer_reg(TEMP_REG);
}


int read_temp_reg(void)
{
	uint8_t buf[2];
	if(write_pointer_reg(TEMP_REG) == -1)
		return -1;
	tsense_check = read(tsense_fd, buf, 2);
	if(tsense_check != 2)
		return -1;
	int temp = (buf[0] << 4) | (buf[1] >> 4);
	return temp;
}

float temp_calc(void)
{
	float degrees;
	int temp = read_temp_reg();
	if(temp == -1)
		return -1;
	if((temp & 0x800) == 0)
	{
		degrees = temp*0.0625;
	}
	else 
	{
		temp = temp ^ 0xFFFF;
		degrees = temp*(-0.0625);		
	}
	if(temp == -1)
		return -1;
	return degrees;
}
