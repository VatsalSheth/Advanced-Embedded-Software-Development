/***********************************************************************************
* @temp.c
* @This file contains a thread spawned by main thread, and exit and entry functions 
* for the temperature thread.
* 
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#include "../Include/temp.h"

/**
 * @brief 
 * The temperature thread performs a number of functions. It collects data from the the sensor and writes the same via message queues 
 * to the logger thread.
 */
void* temp_func(void* threadp)
{
	struct command req, res;
	struct timespec timeout;
	float rf_temp;
	
	sleep(1);
	
	useconds_t garbage_sleep = 1;

	write_config_reg(read_reg(CONFIG_REG) | THERMOSTAT_MODE);
	
	temp_data = write_to_log_queue(TEMP_THREAD_NUM,
							read_reg(CONFIG_REG),
							LOG_DEBUG,
							"TMP102: Config register");
	rc_temp = mq_send(queue_fd, (char*)&temp_data, sizeof(struct log_msg), 0);
	if(rc_temp == -1)
		handle_error_print("mq send in temperature");
	
	write_tlow_reg(LOW_THRESHOLD);		//Low threshold as 23 C
	write_thigh_reg(HIGH_THRESHOLD);		//High threshold as 24 C
		
	while(1)
	{
		if(timer_flag[TEMP_THREAD_NUM] == 1)
		{
			timer_flag[TEMP_THREAD_NUM] = 0;
			
			rf_temp = request_temp();
			if(rf_temp >= 0)
			{
				temp_data = write_to_log_queue(TEMP_THREAD_NUM,
								rf_temp,
								LOG_NONE,
								"GNU TEMPERATURE DEBUGGER");
			}
			else if(rf_temp == -1)
			{
				temp_data = write_to_log_queue(TEMP_THREAD_NUM,
								0,
								ERROR_MESSAGE,
								"Error in fetching data from temperature sensor");
			}
			
			clock_gettime(CLOCK_REALTIME, &timeout);
			timeout.tv_nsec += 100000000;
			rc_temp = mq_timedsend(queue_fd, (char*)&temp_data, sizeof(struct log_msg), 0, &timeout);
			if(rc_temp == -1)
			{
				if(errno == ETIMEDOUT)
				{
					printf("Log queue full...!!!\n");
				}
				else
					handle_error_print("mq send in temperature");
			}
		}
		
		if(socket_req_flag == 1)
		{
			if(socket_req_id == TEMP_THREAD_NUM)
			{
				rc_temp = mq_receive(soc_queue_fd, (char*)&req, sizeof(struct command), NULL);
				if(rc_temp < 0)
				{
					handle_error_print("socket queue receive in temperature sensor");
				}
				else
				{
					if(req.action == REQUEST_TEMPERATURE_C)
					{
						res.sensor_data = request_temp();
						res.action = req.action;
						rc_temp = mq_send(soc_queue_fd, (char*)&res, sizeof(struct command), 0);
						if(rc_temp == -1)
							handle_error_print("temp socket mq_send");
					}
					else if(req.action == REQUEST_TEMPERATURE_F)
					{
						res.sensor_data = request_temp();
						res.sensor_data = conv_temp(res.sensor_data, 'F');
						res.action = req.action;
						rc_temp = mq_send(soc_queue_fd, (char*)&res, sizeof(struct command), 0);
						if(rc_temp == -1)
							handle_error_print("temp socket mq_send");
					}
					else if(req.action == REQUEST_TEMPERATURE_K)
					{
						res.sensor_data = request_temp();
						res.sensor_data = conv_temp(res.sensor_data, 'K');
						res.action = req.action;
						rc_temp = mq_send(soc_queue_fd, (char*)&res, sizeof(struct command), 0);
						if(rc_temp == -1)
							handle_error_print("temp socket mq_send");
					}
					else if(req.action == KILL_TEMPERATURE)
					{
						exit_flag[TEMP_THREAD_NUM] = 1;
						temp_exit();
					}
				}
				socket_req_flag = 0;
			}
		}
		
		usleep(garbage_sleep);
		ack_heartbeat(TEMP_THREAD_NUM);
	}
	pthread_exit(NULL);
}


float request_temp()
{
//	printf("Temperature acquired is %f degrees C\n", temp_calc());
	return temp_calc();//rand();
}


float conv_temp(float cel, char unit)
{
	if(unit == 'F')
	{
		cel = ((9*cel)/5) + 32;
	}
	else if(unit == 'K')
	{
		cel = cel+273;
	}
	return cel;
}


void temp_exit()
{
	if(!exit_flag[TEMP_THREAD_NUM])
	{
		rc_temp = pthread_cancel(temp_th);
		if(rc_temp != 0)
			handle_error("Error cancelling temperature thread");
		
		printf("Exiting temperature thread\n");
		exit_flag[TEMP_THREAD_NUM] = 2;
	}
	else if(exit_flag[TEMP_THREAD_NUM] == 1)
	{
		printf("Exiting temperature thread\n");
		exit_flag[TEMP_THREAD_NUM] = 2;
		pthread_exit(NULL);
	}
}


void temp_entry(void)
{
	exit_flag[TEMP_THREAD_NUM] = 0;
	
	rc_temp = pthread_create(&temp_th, (void *)0, temp_func, (void *)0);
	if(rc_temp != 0)
	{
		handle_error_exit("Error in creating temperature sensor thread");
	}
	else
	{
		rc_temp = pthread_cond_init(&mon[TEMP_THREAD_NUM].cond, NULL); 
		if(rc_temp!=0)
			handle_error_exit("pthread cond init");
			
		rc_temp = pthread_mutex_init(&mon[TEMP_THREAD_NUM].lock, NULL); 
		if(rc_temp!=0)
			handle_error_exit("pthread mutex init");
	}
}
