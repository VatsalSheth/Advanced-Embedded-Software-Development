/***********************************************************************************
* @gpio.c
* @This file APIs to manipulate GPIO
* 
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#include "../Include/gpio.h"

/**
 * @brief 
 *
 * @param pin
 *
 * @return 
 */
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

/**
 * @brief 
 *
 * @param pin
 * @param dir
 *
 * @return 
 */
int gpio_dir(uint32_t pin, char *dir)
{
	FILE *fp;
	char path[50];

	snprintf(path, 50, GPIO_DIR "/gpio%d/direction",pin);

	fp = fopen(path, "w");
	if(fp == NULL)
		return -1;

	fprintf(fp, "%s",dir);

	fclose(fp);
	return 0;
} 

/**
 * @brief 
 *
 * @param pin
 * @param val
 *
 * @return 
 */
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

/**
 * @brief 
 *
 * @param pin
 *
 * @return 
 */
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

/**
 * @brief 
 *
 * @param pin
 * @param edge
 *
 * @return 
 */
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

/**
 * @brief 
 *	Returns the file descriptor of input file
 * @param pin
 * @param file
 *
 * @return 
 */
int gpio_open(uint32_t pin, char *file)
{
	int fp;
	char path[50];

	snprintf(path, 50, GPIO_DIR "/gpio%d/%s",pin,file);

	fp = open(path, O_RDONLY | O_NONBLOCK );
	if(fp < 0)
		return -1;

	return fp;
}

/**
 * @brief 
 *
 * @param fp
 *
 * @return 
 */
int gpio_close(int fp)
{
	return close(fp);
}

/**
 * @brief 
 * Toggles the input GPIO pins at 1 HZ frequency
 * @param pin
 *
 * @return 
 */
int gpio_blink(uint32_t pin)
{
	blink_timer_init(pin);
} 

/**
 * @brief 
 * Turns off GPIO toggle
 * @param pin
 *
 * @return 
 */
int gpio_blink_off(uint32_t pin)
{
	timer_delete(blink_timer_id);
	gpio_set_value(pin, 0);

	return 0;
}

/**
 * @brief 
 * Timer for GPIO blink
 * @param pin
 */
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

/**
 * @brief 
 * Timer handle
 * @param sv
 */
void blink_timer_handle(union sigval sv)
{	
	uint32_t led;

	led = gpio_get_value(sv.sival_int);
	gpio_set_value(sv.sival_int, !led);
}
