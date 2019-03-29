#include "../Include/gpio.h"

int gpio_export(uint32_t pin)
{
	FILE *fp;
	
	fp = fopen(GPIO_DIR "/export", "w");
	if(fp == NULL)
		return -1;
	
	fprintf(fp, "%d",pin);
	
	fclose(fp);
	return 0;
} 

int gpio_dir(uint32_t pin)
{
	FILE *fp;
	char path[50];
	
	snprintf(path, 50, GPIO_DIR "/gpio%d/direction",pin);
	
	fp = fopen(path, "w");
	if(fp == NULL)
		return -1;
	
	fprintf(fp, "out");
	
	fclose(fp);
	return 0;
} 

int gpio_set_value(uint32_t pin, uint32_t val)
{
	FILE *fp;
	char path[50];
	
	snprintf(path, 50, GPIO_DIR "/gpio%d/value",pin);
	
	fp = fopen(path, "w");
	if(fp == NULL)
		return -1;
	
	fprintf(fp, "%d",val);
	
	fclose(fp);
	return 0;
} 

int gpio_get_value(uint32_t pin)
{
	FILE *fp;
	char path[50];
	uint32_t val;
	
	snprintf(path, 50, GPIO_DIR "/gpio%d/value",pin);
	
	fp = fopen(path, "r");
	if(fp == NULL)
		return -1;
	
	val = (uint32_t)fgetc(fp);
	val -= 48;
		
	fclose(fp);
	return val;
} 

int gpio_edge(uint32_t pin, char *edge)
{
	FILE *fp;
	char path[50];
	
	snprintf(path, 50, GPIO_DIR "/gpio%d/edge",pin);
	
	fp = fopen(path, "w");
	if(fp == NULL)
		return -1;
	
	fprintf(fp, "%s",edge);
	
	fclose(fp);
	return 0;
}

FILE *gpio_open(uint32_t pin, char *file)
{
	FILE *fp;
	char path[50];
	
	snprintf(path, 50, GPIO_DIR "/gpio%d/%s",pin,file);
	
	fp = fopen(path, "w");
	if(fp == NULL)
		return (FILE*)-1;
		
	return fp;
}

int gpio_close(FILE *fp)
{
	fclose(fp);
			
	return 0;
}

int gpio_blink(uint32_t pin)
{
	blink_timer_init(pin);
} 

void blink_timer_init(uint32_t pin)
{
	struct sigevent sev;
	struct itimerspec trigger;
	
	memset(&sev, 0, sizeof(struct sigevent));
    memset(&trigger, 0, sizeof(struct itimerspec));
    
    sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = blink_timer_handle;
	sev.sigev_value.sival_int = pin;
	
	timer_create(CLOCK_REALTIME, &sev, &blink_timer_id);
	
	trigger.it_interval.tv_sec = 1;
	trigger.it_value.tv_sec = 1;
		
	timer_settime(blink_timer_id, 0, &trigger, NULL);
}

void blink_timer_handle(union sigval sv)
{	
	uint32_t led;
	
	led = gpio_get_value(sv.sival_int);
	gpio_set_value(sv.sival_int, !led);
}
