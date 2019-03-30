#include "../Include/apds.h"

int write_command_reg(uint8_t reg)
{
	int buf = reg | COMMAND;
	lsense_check = write(lsense_fd, &buf, 1);
	if(lsense_check == -1)
	{
		char* string = malloc(30);
		sprintf(string, "Failed to write to %x register", reg);
		perror(string);
		free(string);
		exit(1);
	}
	return lsense_check;
}

int sensor_init(void)
{
	char *filename = "/dev/i2c-2";
	lsense_fd = open("/dev/i2c-2", O_RDWR);
//chec return type outside; followiing check is for testing
	if (lsense_fd < 0)
	{
   		perror("Failed to open the i2c bus");
    		exit(1);
	}

	int addr = SLAVE_ADDRESS;          // The I2C address of the ADC
	if (ioctl(lsense_fd, I2C_SLAVE, addr) < 0)
       	{
    		perror("Failed to acquire bus access");
		/* ERROR HANDLING; you can check errno to see what went wrong */
   		 exit(1);
	}
	return lsense_fd;
}

int write_control_reg(uint8_t val)
{
	write_command_reg(CONTROL_REG);
	
	int buf = val;
	lsense_check = write(lsense_fd, &buf, 1);
	if(lsense_check != 1)
	{
		perror("Failed to write in write_control_reg");
		exit(1);
	}
	return lsense_check;
}

uint8_t read_control_reg()
{
	write_command_reg(CONTROL_REG);

	uint8_t buf;
	lsense_check = read(lsense_fd, &buf, 1);
	if(lsense_check == -1)
	{
		perror("Failed to read in read_control_reg");
		exit(1);
	}
	return buf;
}

int write_int_ctl_reg(uint8_t val) 	//write 0b00010001 for last conversion resulting in value outside threshold
{
	write_command_reg(INT_CTL_REG);

	int buf = val;
	lsense_check = write(lsense_fd, &buf, 1);
	if(lsense_check == -1)
	{
		perror("Failed to write in write_int_ctl_reg");
		exit(1);
	}
	return lsense_check;
}

uint8_t read_int_ctl_reg(void) 
{
	write_command_reg(INT_CTL_REG);

	uint8_t buf;	
	lsense_check = read(lsense_fd, &buf, 1);
	if(lsense_check == -1)
	{
		perror("Failed to read in read_int_ctl_reg");
		exit(1);
	}
	return buf;
}

int sensor_id()
{
	write_command_reg(ID_REG);

	int buf;
	lsense_check = read(lsense_fd, &buf, 1);
	if(lsense_check == -1)
	{
		perror("Failed to read in read_control_reg");
		exit(1);
	}
	return buf;
}

int write_timing_reg(uint8_t val)
{
	write_command_reg(TIMING_REG);

	int buf = val;
	lsense_check = write(lsense_fd, &buf, 1);
	if(lsense_check == -1)
	{
		perror("Failed to write in write_timing_reg");
		exit(1);
	}
	return lsense_check;
}

uint8_t read_timing_reg(void)
{
	write_command_reg(TIMING_REG);

	uint8_t buf;
	lsense_check = read(lsense_fd, &buf, 1);
	if(lsense_check == -1)
	{
		perror("Failed to read in read_timing_reg");
		exit(1);
	}
	return buf;
}

int write_int_th_reg(uint16_t val, uint8_t reg)		//pass 1 for INT_TH_L and 2 for INT_TH_H registers
{
	uint8_t temp_val = val & 0x00FF;
	if(reg == 1)
	{
		write_command_reg(INT_TH_LL_REG);
		lsense_check = write(lsense_fd, &temp_val, 1);
		if(lsense_check == -1)
		{
			perror("Failed to write LL reg in int_th_reg");
			exit(1);
		}
	
		temp_val = val >> 8;
		write_command_reg(INT_TH_LH_REG);
		lsense_check = write(lsense_fd, &temp_val, 1);
		if(lsense_check == -1)
		{
			perror("Failed to write LH reg in int_th_reg");
			exit(1);
		}
	}
	else if(reg == 2)
	{
		write_command_reg(INT_TH_HL_REG);
		lsense_check = write(lsense_fd, &temp_val, 1);
		if(lsense_check == -1)
		{
			perror("Failed to write HL reg in int_th_reg");
			exit(1);
		}
	
		temp_val = val >> 8;
		write_command_reg(INT_TH_HH_REG);
		lsense_check = write(lsense_fd, &temp_val, 1);
		if(lsense_check == -1)
		{
			perror("Failed to write HH reg in int_th_reg");
			exit(1);
		}
	}
	return lsense_check;
}

uint16_t read_int_th_reg(uint8_t reg)
{
	uint16_t val, temp_val;
	if(reg == 1)
	{
		write_command_reg(INT_TH_LL_REG);
		lsense_check = read(lsense_fd, &temp_val, 1);
		if(lsense_check == -1)
		{
			perror("Failed to read LL reg in int_th_reg");
			exit(1);
		}
		write_command_reg(INT_TH_LH_REG);
		lsense_check = read(lsense_fd, &val, 1);
		if(lsense_check == -1)
		{
			perror("Failed to read LH reg in int_th_reg");
			exit(1);
		}
		val = val << 8;
		val = val | temp_val;
		return val;
	}
	else if(reg == 2)
	{
		write_command_reg(INT_TH_HL_REG);
		lsense_check = read(lsense_fd, &temp_val, 1);
		if(lsense_check == -1)
		{
			perror("Failed to read HL reg in int_th_reg");
			exit(1);
		}
	
		write_command_reg(INT_TH_HH_REG);
		lsense_check = read(lsense_fd, &val, 1);
		if(lsense_check == -1)
		{
			perror("Failed to read HH reg in int_th_reg");
			exit(1);
		}
		val = val << 8;
		val = val | temp_val;
		return val;
	}
}

uint16_t ch_ADC0(void)		//min 69	 max 124
{
	uint16_t adc0;
	uint8_t lsb, msb;

	write_command_reg(ADC_DATA0L_REG);
	
	lsense_check = read(lsense_fd, &lsb, 1);
	if(lsense_fd == -1)
	{
		perror("Failed to read in ch_ADC0L");
		exit(1);
	}

	write_command_reg(ADC_DATA0H_REG);
	
	lsense_check = read(lsense_fd, &msb, 1);
	if(lsense_fd == -1)
	{
		perror("Failed to read in ch_ADC0H");
		exit(1);
	}

	msb = msb << 8;
	adc0 = msb | lsb;
	return adc0;
}


uint16_t ch_ADC1(void)		//min 12	max 58
{
	uint16_t adc1;
	uint8_t lsb, msb;

	write_command_reg(ADC_DATA1L_REG);
	
	lsense_check = read(lsense_fd, &lsb, 1);
	if(lsense_fd == -1)
	{
		perror("Failed to read in ch_ADC1L");
		exit(1);
	}

	write_command_reg(ADC_DATA1H_REG);
	
	lsense_check = read(lsense_fd, &msb, 1);
	if(lsense_fd == -1)
	{
		perror("Failed to read in ch_ADC1H");
		exit(1);
	}

	msb = msb << 8;
	adc1 = msb | lsb;
	return adc1;
}

float lux_calc(void)
{
	float lux;
	uint16_t adc0, adc1;
	adc0 = ch_ADC0();
	adc1 = ch_ADC1();

	float adc_div = ((float)adc1)/((float)adc0);
	if((0 < adc_div) && (adc_div <= 0.5))
		lux = (0.0304*adc0) - (0.062*adc1*pow(adc_div, 1.4));
	else if((0.5 < adc_div) && (adc_div <= 0.61))
		lux = (0.0224*adc0) - (0.031*adc1);
	else if((0.61 < adc_div) && (adc_div <= 0.80))
		lux = (0.0128*adc0) - (0.0153*adc1);
	else if((0.80 < adc_div) && (adc_div <= 1.30))
		lux = (0.00146*adc0) - (0.00112*adc1);
	else if(adc_div > 1.3)
		lux = 0;

	return lux;
}