/***********************************************************************************
* @main.c
* @This file contains main(), with thread creation, joining, heartbeat check function, 
* GPIO interrupt thread. built in self test function.
* Command to run the application project_1 <Log file name> <Log Verbosity>
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#include "../Include/main.h"

/**
 * @brief 
 * This is the main function where signals are set, threads are created, buil in self tests,
 * heartbeat checks are performed.
 * 
 * @param argc runtime arguments count
 * @param argv[] runtime argument for log file, log verbosity
 *
 * @return 
 */
int main(int argc, char *argv[])
{
	gpio_export(GPIO_LED);
	gpio_dir(GPIO_LED, "out");
	gpio_set_value(GPIO_LED, 0);
	
	set_signal_handler();
	exit_cond = 1;

	light_sensor_bist();
	temp_sensor_bist();

	if(!arg_init(argv[1], argv[2]))
	{
		return 0;
	}
		
	thread_create();
	usleep(20000);
	
	while(exit_cond)
	{
		heartbeat_check();
	}
	thread_join();
	printf("\n");
	return 0;
}

/**
 * @brief 
 * This function joins all the created threads
 */
void thread_join()
{
	rc = pthread_join(log_th, NULL);
	if(rc != 0)
	{
		handle_error("Error in joining logger thread");
	}
	else 
	{
		pthread_cond_destroy(&mon[LOG_THREAD_NUM].cond);
		pthread_mutex_destroy(&mon[LOG_THREAD_NUM].lock);
	}

	rc = pthread_join(temp_th, NULL);
	if(rc != 0)
	{
		handle_error("Error in joining temperature sensor thread");
	}
	else 
	{
		pthread_cond_destroy(&mon[TEMP_THREAD_NUM].cond);
		pthread_mutex_destroy(&mon[TEMP_THREAD_NUM].lock);
	}	
		
	rc = pthread_join(light_th, NULL);
	if(rc != 0)
	{
		handle_error("Error in joining light sensor thread");
	}
	else 
	{
		pthread_cond_destroy(&mon[LIGHT_THREAD_NUM].cond);
		pthread_mutex_destroy(&mon[LIGHT_THREAD_NUM].lock);
	}
	
	rc = pthread_join(socket_th, NULL);
	if(rc != 0)
	{
		handle_error("Error in joining socket sensor thread");
	}
	else 
	{
		pthread_cond_destroy(&mon[SOCKET_THREAD_NUM].cond);
		pthread_mutex_destroy(&mon[SOCKET_THREAD_NUM].lock);
	}
	
	rc = pthread_join(int_th, NULL);
	if(rc != 0)
		handle_error("Error in joining interrupt thread");
}

/**
 * @brief 
 * This function creates all the threads
 */
void thread_create()
{
	log_entry();
	sleep(1);
	socket_entry();
	sleep(1);
	temp_entry();	
	light_entry();
			
	rc = pthread_create(&int_th, (void *)0, int_func, (void *)0);
	if(rc != 0)
		handle_error("Error in creating interrupt thread");
}

/**
 * @brief 
 * This function sets the log file name and log verbosity provided runtime
 * @param arg1 log file name
 * @param arg2 log verbosity
 *
 * @return 
 */
int arg_init(char *arg1, char *arg2)
{
	if(arg1 == NULL)
	{
		printf("No file input given...!!!\n");
		return 0;
	}
	else if(!strcmp(arg1, "Debug"))
	{
		printf("File name cannot be debug...!!!\n");
		return 0;
	}
	
	log_file.file_name = arg1;
	
	if(!strcmp(arg2, "Debug"))
	{
		log_file.log_verbosity = LOG_DEBUG;
	}
	else
	{
		log_file.log_verbosity = LOG_NONE;
	}
	
	return 1;
}

/**
 * @brief 
 * This function performs built in self test on light sensor
 */
void light_sensor_bist(void)
{
	rc_lsense = light_sensor_init();
	int buf = CONTROL_REG | COMMAND;
	lsense_check = write(lsense_fd, &buf, 1);
	buf = POWER_UP;
	rc_lsense = write(lsense_fd, &buf, 1);

	buf = CONTROL_REG | COMMAND;
	rc_lsense = write(lsense_fd, &buf, 1);
	lsense_check = read(lsense_fd, &rc_lsense, 1);

	if((rc_lsense & CONTROL_REG_MASK) == POWER_UP)
	{	
		bist_buffer[0] = malloc(30);
		strcpy(bist_buffer[0], "Light sensor initialized\n");
	}
	else 
	{
		printf("Light sensor initialization failed, exiting..\n");
		gpio_set_value(GPIO_LED, 1);
		exit(1);
	}
}

/**
 * @brief 
 * This function performs built in self test on temperature sensor
 */
void temp_sensor_bist(void)
{
	rc_tsense = temp_sensor_init();
	uint8_t buf[2];
	uint8_t reg = CONFIG_REG;
	rc_tsense = write(tsense_fd, &reg, 1);
	rc_tsense = read(tsense_fd, buf, 2);
	rc_tsense = (buf[0] << 4) | (buf[1] >> 4);
	if((rc_tsense & CONV_RES_MASK) == CONV_RES_MASK)
	{
		bist_buffer[1] = malloc(30);
		strcpy(bist_buffer[1], "Temperature sensor initialized\n");
	}
	else
	{
		printf("Temperature sensor initialization failed, exiting..\n");
		gpio_set_value(GPIO_LED, 1);	
		exit(1);
	}
}

/**
 * @brief 
 *	This is the signal handler funnction for SIGINT signal
 * @param signo
 * @param info
 * @param extra
 */
void signal_handler(int signo, siginfo_t *info, void *extra) 
{	
	timer_delete(timer_id);
	log_exit();
	temp_exit();
	light_exit();
	socket_exit();
	int_exit();
	exit_cond = 0;
}

/**
 * @brief 
 * This function initializes and sets the signal handler for SIGINT signal
 */
void set_signal_handler(void)
{
	struct sigaction action;
 
    action.sa_flags = SA_SIGINFO; 
    action.sa_sigaction = signal_handler;
 
    if (sigaction(SIGINT, &action, NULL) == -1)
		handle_error("SIGINT: sigaction")
}

/**
 * @brief 
 * This function checks the heartbeat using pthread conditional variables using monitor structure
 * On 3 successive heartbeat fails for a thread, it is canceled and re-spawned. LED is blinked during
 * heartbeat failure
 */
void heartbeat_check(void)
{
	uint32_t i;
	struct log_msg error_data;
	
	for(i=0; i<(NUM_OF_THREADS - socket_hb); i++)
	{	
		if(!exit_cond)
			break;
			
		clock_gettime(CLOCK_REALTIME, &mon[i].timeout);
		mon[i].timeout.tv_sec += 3;
	
		pthread_mutex_lock(&mon[i].lock);
		rc = pthread_cond_timedwait(&mon[i].cond, &mon[i].lock, &mon[i].timeout);  
		if(rc == ETIMEDOUT)
		{
			if(!mon[i].count)
				gpio_blink(GPIO_LED);
				
			printf("Thread fail: %d\n",i);
			mon[i].count++;
			if(mon[i].count >=3)
			{
				mon[i].count = 0;
				gpio_blink_off(GPIO_LED);
				gpio_set_value(GPIO_LED, 0);
				
				if(i==0)
				{
					temp_exit();
					temp_entry();
				}
				else if(i==1)
				{
					light_exit();
					light_entry();
				}
				else if(i==2)
				{
					log_exit();
					log_entry();
				}
				else if(i==3)
				{
					socket_exit();
					socket_entry();
				}
			}
			
			if(!exit_flag[LOG_THREAD_NUM]) 
			{
				error_data = write_to_log_queue(i,
								0,
								ERROR_MESSAGE,
								"Heartbeat failed");
				
				clock_gettime(CLOCK_REALTIME, &mon[i].timeout);
				mon[i].timeout.tv_sec += 1;
				rc = mq_timedsend(queue_fd, (char*)&error_data, sizeof(struct log_msg), 0, &mon[i].timeout);
				if(rc == -1)
					handle_error_print("heartbeat mq_send");
			}
		}
		pthread_mutex_unlock(&mon[i].lock);
	}
}

/**
 * @brief 
 * This is the thread function which polls GPIO pins for sensor interrupts and logs on interrupt
 * @param threadp
 *
 * @return 
 */
void* int_func(void* threadp)
{
	struct pollfd fdset[2];
	struct log_msg interrupt_data;
	int nfds, rc_int;
	char val[5];
	gpio_export(GPIO_TEMP);
	gpio_dir(GPIO_TEMP, "in");
	gpio_edge(GPIO_TEMP, "both");
	gpio_fd[TEMP_THREAD_NUM] = gpio_open(GPIO_TEMP, "value");
	
	gpio_export(GPIO_LIGHT);
	gpio_dir(GPIO_LIGHT, "in");
	gpio_edge(GPIO_LIGHT, "falling");
	gpio_fd[LIGHT_THREAD_NUM] = gpio_open(GPIO_LIGHT, "value");
	
	nfds = 2;
	
	while(1)
	{
		memset((void*)fdset, 0, sizeof(fdset));
		
		fdset[TEMP_THREAD_NUM].fd = gpio_fd[TEMP_THREAD_NUM]; 
		fdset[TEMP_THREAD_NUM].events = POLLPRI;
		
		fdset[LIGHT_THREAD_NUM].fd = gpio_fd[LIGHT_THREAD_NUM]; 
		fdset[LIGHT_THREAD_NUM].events = POLLPRI;
		
		rc_int = poll(fdset, nfds, 1000);
		if(rc_int < 0)
		{
			handle_error("poll");
		}
		else if(rc_int == 0)
		{
			continue;
		}
			
		if (fdset[TEMP_THREAD_NUM].revents & POLLPRI) 
		{
			lseek(fdset[TEMP_THREAD_NUM].fd, 0, SEEK_SET);
			read(fdset[TEMP_THREAD_NUM].fd, val, 5);
			interrupt_data = write_to_log_queue(	TEMP_THREAD_NUM,
								0,
								ERROR_MESSAGE,
								"Interrupt, crossing recommended levels");
			rc = mq_send(queue_fd, (char*)&interrupt_data, sizeof(struct log_msg), 0); 
			if(rc == -1)
			{
				handle_error("interrupt mq_send");
			}
			printf("Temperature interrupt occurred, crossing recommended levels \n");
		}
		
		if (fdset[LIGHT_THREAD_NUM].revents & POLLPRI) 
		{
			lseek(fdset[LIGHT_THREAD_NUM].fd, 0, SEEK_SET);
			read(fdset[LIGHT_THREAD_NUM].fd, val, 5);
			interrupt_data = write_to_log_queue(	LIGHT_THREAD_NUM,
								0,
								ERROR_MESSAGE,
								(light_status == STATUS_LIGHT)?"Interrupt, ambience is BRIGHT":"Interrupt, ambience is DARK");
			rc = mq_send(queue_fd, (char*)&interrupt_data, sizeof(struct log_msg), 0); 
			if(rc == -1)
			{
				handle_error("interrupt mq_send");
			}
			printf("Light interrupt occurred, ambience is %s \n",(light_status == STATUS_LIGHT)?"BRIGHT":"DARK");
			write_command_reg(0b11000000);
		}
	}
}

/**
 * @brief 
 * This function safely exits interrupt thread on kill
 */
void int_exit(void)
{
	static uint32_t flag;
	
	if(flag == 0)
	{
		gpio_close(gpio_fd[TEMP_THREAD_NUM]);
		gpio_close(gpio_fd[LIGHT_THREAD_NUM]);
	
		rc = pthread_cancel(int_th);
		if(rc != 0)
			handle_error("Error cancelling interrupt thread");
	}
	flag = 1;
}
