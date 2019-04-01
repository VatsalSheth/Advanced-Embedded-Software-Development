/***********************************************************************************
* @light.c
* @This file contains a thread spawned by main thread, and exit and entry functions 
* for the light sensor thread.
* 
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#include "../Include/light.h"

/**
 * @brief 
 * The light thread performs a number of functions. It collects data from the the sensor and writes the same via message queues 
 * to the logger thread.
 */
void* light_func(void* threadp)
{
	struct command req, res;
	struct timespec timeout;
	float rf_light;
	
	sleep(1);
	
	useconds_t garbage_sleep = 1;
	
	rc_lsense = read_control_reg();
	light_data = write_to_log_queue(LIGHT_THREAD_NUM,
					rc_lsense,
					LOG_DEBUG,
					"APDS9301: Control Register");
	rc_light = mq_send(queue_fd, (char*)&light_data, sizeof(struct log_msg), 0);
	if(rc_light == -1)
		handle_error_print("mq send in light");
	
	rc_lsense = write_int_th_reg(CH0_LOW_THRESHOLD, 1);	//Write low threshold as 40 lux
	rc_lsense = write_int_th_reg(CH0_HIGH_THRESHOLD, 2);	//Write high threshold as 600 lux
	
	rc_lsense = read_timing_reg();
	light_data = write_to_log_queue(LIGHT_THREAD_NUM,
					rc_lsense,
					LOG_DEBUG,
					"APDS9301: Timing Register");
	rc_light = mq_send(queue_fd, (char*)&light_data, sizeof(struct log_msg), 0);
	if(rc_light == -1)
		handle_error_print("mq send in light");

	rc_lsense = write_int_ctl_reg(0b00011010);		//persist = 10 integration time
	write_command_reg(0b11000000);					//clear interrupt first time
	
	while(1)
	{
		if(timer_flag[LIGHT_THREAD_NUM] == 1)
		{
			timer_flag[LIGHT_THREAD_NUM] = 0;

			rf_light = request_light();
			if(rf_light >= 0)
			{
				light_data = write_to_log_queue(LIGHT_THREAD_NUM,
								rf_light,
								LOG_NONE,
								"GNU LIGHT DEBUGGER");
			}
			else
			{
				if(rf_light == ADC0_ERROR)
				{
					light_data = write_to_log_queue(LIGHT_THREAD_NUM,
									0,
									ERROR_MESSAGE,
									"Error in fetching data from ADC channel 0");
				}
				else if(rf_light == ADC1_ERROR)
				{
					light_data = write_to_log_queue(LIGHT_THREAD_NUM,
									0,
									ERROR_MESSAGE,
									"Error in fetching data from ADC channel 1");
				}
				
			}
			
			clock_gettime(CLOCK_REALTIME, &timeout);
			timeout.tv_nsec += 100000000;
			rc_light = mq_timedsend(queue_fd, (char*)&light_data, sizeof(struct log_msg), 0, &timeout);
			if(rc_light == -1)
			{
				if(errno == ETIMEDOUT)
				{
					printf("Log queue full...!!!\n");
				}
				else
					handle_error_print("mq send in light");
			}
			light_status = (light_data.data>15)?STATUS_LIGHT:STATUS_DARK;
		}
		
		if(socket_req_flag == 1)
		{
			if(socket_req_id == LIGHT_THREAD_NUM)
			{
				clock_gettime(CLOCK_REALTIME, &timeout);
				timeout.tv_nsec += 100000000;
				rc_light = mq_timedreceive(soc_queue_fd, (char*)&req, sizeof(struct command), NULL, &timeout);
				if(rc_light < 0)
				{
					if(errno == ETIMEDOUT)
					{
						light_data = write_to_log_queue(LIGHT_THREAD_NUM,
										0,
										ERROR_MESSAGE,
										"Socket queue empty. NO request received...!!!");
						clock_gettime(CLOCK_REALTIME, &timeout);
						timeout.tv_nsec += 100000000;
						rc_light = mq_timedsend(queue_fd, (char*)&light_data, sizeof(struct log_msg), 0, &timeout);
						if(rc_light == -1)
						{
							if(errno == ETIMEDOUT)
							{
								printf("Log queue full...!!!\n");
							}
							else
								handle_error_print("mq send in light");
						}
					}
					else
						handle_error_print("socket queue receive in light sensor");
				}
				else
				{
					if(req.action == REQUEST_LIGHT)
					{
						res.sensor_data = request_light();
						res.action = (res.sensor_data>15)?STATUS_LIGHT:STATUS_DARK;
						light_status = res.action;
						clock_gettime(CLOCK_REALTIME, &timeout);
						timeout.tv_nsec += 100000000;
						rc_light = mq_timedsend(soc_queue_fd, (char*)&res, sizeof(struct command), 0, &timeout);
						if(rc_light == -1)
						{
							if(errno == ETIMEDOUT)
							{
								printf("Log-socket queue full...!!!\n");
							}
							else
								handle_error_print("light socket mq_send");
						}
					}
					else if(req.action == KILL_LIGHT)
					{
						exit_flag[LIGHT_THREAD_NUM] = 1;
						light_exit();
					}
				}
				socket_req_flag = 0;
			}
		}
		
		usleep(garbage_sleep);
		ack_heartbeat(LIGHT_THREAD_NUM);
	}
	pthread_exit(NULL);
}


float request_light()
{
//	rc_lsense = sensor_id();
//	printf("Register ID: %d\n",rc_lsense);
//	printf("\nADC channel 0 reading: %d adc0-\n", ch_ADC0());
//	printf("ADC channel 1 reading: %d adc1-\n", ch_ADC1());
//	printf("Low threshold set is: %u ", read_int_th_reg(1));
//	printf("High threshold set is: %u ", read_int_th_reg(2));
//	printf("Sensor reading: %f lux\n", lux_calc());
	return lux_calc();
}


void light_exit()
{
	if(!exit_flag[LIGHT_THREAD_NUM])
	{
		rc_light = pthread_cancel(light_th);
		if(rc_light != 0)
			printf("Light Thread: NO thread found with given thread ID...!!!\n");
		
		printf("\nExiting light thread\n");
		exit_flag[LIGHT_THREAD_NUM] = 2;
	}
	else if(exit_flag[LIGHT_THREAD_NUM] == 1)
	{
		printf("\nExiting light thread\n");
		exit_flag[LIGHT_THREAD_NUM] = 2;
		pthread_exit(NULL);
	}
}


void light_entry(void)
{
	exit_flag[LIGHT_THREAD_NUM] = 0;
	
	rc_light = pthread_create(&light_th, (void *)0, light_func, (void *)0);
	if(rc_light != 0)
	{
		handle_error_exit("Error in creating light sensor thread");
	}
	else
	{
		rc_light = pthread_cond_init(&mon[LIGHT_THREAD_NUM].cond, NULL); 
		if(rc_light!=0)
			handle_error_exit("pthread cond init");
			
		rc_light = pthread_mutex_init(&mon[LIGHT_THREAD_NUM].lock, NULL); 
		if(rc_light!=0)
			handle_error_exit("pthread mutex init");
	}
}
