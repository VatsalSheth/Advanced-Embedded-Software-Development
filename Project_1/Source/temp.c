#include "../Include/temp.h"

void* temp_func(void* threadp)
{
	struct command req, res;
	struct timespec timeout;
	sleep(1);
	srand(time(NULL));
	
	useconds_t garbage_sleep = 1;

	rc_tsense = temp_sensor_init();
//	printf("Config register holds %x\n", read_reg(CONFIG_REG));
	write_config_reg(read_reg(CONFIG_REG) | THERMOSTAT_MODE);
	write_tlow_reg(LOW_THRESHOLD);		//Low threshold as 23 C
	write_thigh_reg(HIGH_THRESHOLD);		//High threshold as 24 C
		
	while(1)
	{
		if(timer_flag[TEMP_THREAD_NUM] == 1)
		{
			timer_flag[TEMP_THREAD_NUM] = 0;
			
			temp_data = write_to_log_queue(TEMP_THREAD_NUM,
							request_temp(),
							LOG_DEBUG,
							"GNU TEMPERATURE DEBUGGER");
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
					handle_error("mq send in temperature");
			}
		}
		
		if(socket_req_flag == 1)
		{
			if(socket_req_id == TEMP_THREAD_NUM)
			{
				rc_temp = mq_receive(soc_queue_fd, (char*)&req, sizeof(struct command), NULL);
				if(rc_temp < 0)
				{
					handle_error("socket queue receive in temperature sensor");
				}
				else
				{
					if(req.action == REQUEST_TEMPERATURE_C)
					{
						res.sensor_data = request_temp();
						res.action = req.action;
						rc_temp = mq_send(soc_queue_fd, (char*)&res, sizeof(struct command), 0);
						if(rc_temp == -1)
							handle_error("temp socket mq_send");
					}
					else if(req.action == REQUEST_TEMPERATURE_F)
					{
						res.sensor_data = request_temp();
						res.sensor_data = conv_temp(res.sensor_data, 'F');
						res.action = req.action;
						rc_temp = mq_send(soc_queue_fd, (char*)&res, sizeof(struct command), 0);
						if(rc_temp == -1)
							handle_error("temp socket mq_send");
					}
					else if(req.action == REQUEST_TEMPERATURE_K)
					{
						res.sensor_data = request_temp();
						res.sensor_data = conv_temp(res.sensor_data, 'K');
						res.action = req.action;
						rc_temp = mq_send(soc_queue_fd, (char*)&res, sizeof(struct command), 0);
						if(rc_temp == -1)
							handle_error("temp socket mq_send");
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
		cel = (5*(cel-32))/9;
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
