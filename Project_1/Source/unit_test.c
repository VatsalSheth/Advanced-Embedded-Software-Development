#include "../Include/temp.h"
#include "../Include/light.h"

uint32_t total;
uint32_t pass;

void temp_init()
{
	temp_sensor_init();
}

void light_init()
{
	light_sensor_init();
	write_control_reg(0x03);		//Power on light sensor
}

void test_temp_conv()
{
	float ip;
	uint32_t t=5, p=0;
	
	for(uint32_t i=0; i<t; i++)
	{
		ip = ((float)rand())/rand();
		printf("%f\n",ip);
		//conv_temp
	}
}

void main()
{
	FILE *fp;
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
	
}
