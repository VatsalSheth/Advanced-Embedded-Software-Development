/***********************************************************************************
* @unit_test.c
* @This file contains test for temperature conversion, request temperature and sensor
* data function
* 
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#include "../Include/temp.h"
#include "../Include/light.h"

uint32_t total;
uint32_t pass;
FILE *fp;

/**
 * @brief 
 * Initialize thmperatuer sensor
 */
void temp_init()
{
	temp_sensor_init();
}

/**
 * @brief 
 * Initialize light sensor
 */
void light_init()
{
	light_sensor_init();
	write_control_reg(0x03);		//Power on light sensor
}

/**
 * @brief 
 * This function supplies random floating input in celcius and randomly selects coversion between 
 * kelvin and fahrenheit. It compares the coutput and logs test result.
 */
void test_temp_conv()
{
	float ip, op;
	uint32_t t=5, p=0;
	
	for(uint32_t i=0; i<t; i++)
	{
		ip = (((float)1)/rand()) + (rand()%100);
		if(rand()%2)
		{
			op = conv_temp(ip, 'K');
			if(op == ip + 273)
				p++;
		}
		else
		{
			op = conv_temp(ip, 'F');
			if(op == (((9*ip)/5) + 32))
				p++;
		}
	}
	total += t;
	pass += p;
	fprintf(fp, "Temperature conversion test: %d PASS out of %d\n",p,t);
}

/**
 * @brief 
 * This function reads temperature sensor data every 1 seconds and checks whether the data is 
 * in valid range. It logs the results
 */
void test_temp_data()
{
	uint32_t t=5, p=0;
	float op;
	
	for(uint32_t i=0; i<t; i++)
	{
		op=request_temp();
		if(op > -40 && op < 100)
			p++;
		sleep(1);
	}
	total += t;
	pass += p;
	fprintf(fp, "Temperature data request test: %d PASS out of %d\n",p,t);
}

/**
 * @brief 
 * This function reads light sensor data every 1 seconds and checks whether the data is 
 * in valid range. It logs the results
 */
void test_light_data()
{
	uint32_t t=5, p=0;
	float op;
	
	for(uint32_t i=0; i<t; i++)
	{
		op=request_light();
		if(op >= 0 && op < 500)
			p++;
		sleep(1);
	}
	total += t;
	pass += p;
	fprintf(fp, "Light data request test: %d PASS out of %d\n",p,t);
}

/**
 * @brief 
 * It runs 3 test suite and logs final results. 
 */
void main()
{
	fp = fopen("unittest_log.txt", "w");
	if(fp == NULL)
	{
		printf("Unable to create log file\n");
		exit(0);
	}
	
	srand(time(0));
	
	temp_init();
	light_init();
	
	fprintf(fp, "Unit Test\n");
	
	test_temp_conv();
	test_light_data();
	test_temp_data();
	
	if(pass == total)
		fprintf(fp, "TEST SUCCESS...!!!\n");
	else
		fprintf(fp, "TEST FAIL...!!!\n");
	
	fprintf(fp, "Total test: %d PASS out of %d\n",pass,total);
	fclose(fp);	
}
