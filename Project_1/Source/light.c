#include "../Include/light.h"

void* light_func(void* threadp)
{
	struct command req, res;
	
	sleep(1);
	srand(time(NULL));
	
	useconds_t garbage_sleep = 1;
	
	rc_lsense = sensor_init();
	rc_lsense = write_control_reg(0x03);
	rc_lsense = read_control_reg();
	printf("Control register: %d\n",rc_lsense);
	rc_lsense = write_int_th_reg(0b0000000001010000, 1);	//120 and 72
	rc_lsense = write_int_th_reg(0b0000001001011000, 2);	//55 and 15
	rc_lsense = write_int_ctl_reg(0b00010001);
	
write_command_reg(0b11000000);
	
	while(1)
	{
		if(timer_flag[LIGHT_THREAD_NUM] == 1)
		{
			timer_flag[LIGHT_THREAD_NUM] = 0;

			light_data = write_to_log_queue(LIGHT_THREAD_NUM,
											request_light(),
											LOG_DEBUG,
											"GNU LIGHT DEBUGGER");
			rc_light = mq_send(queue_fd, (char*)&light_data, sizeof(struct log_msg), 0);
			if(rc_light == -1)
				handle_error("light mq_send");
		}
		
		if(socket_req_flag == 1)
		{
			if(socket_req_id == LIGHT_THREAD_NUM)
			{
				rc_light = mq_receive(soc_queue_fd, (char*)&req, sizeof(struct command), NULL);
				if(rc_light < 0)
				{
					handle_error("socket queue receive in light sensor");
				}
				else
				{
					if(req.action == REQUEST_LIGHT)
					{
						res.sensor_data = request_light();
						res.action = req.action;
						rc_light = mq_send(soc_queue_fd, (char*)&res, sizeof(struct command), 0);
						if(rc_light == -1)
							handle_error("light socket mq_send");
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
	printf("\nADC channel 0 reading: %d adc0-\n", ch_ADC0());
	printf("ADC channel 1 reading: %d adc1-\n", ch_ADC1());
	printf("Low threshold set is: %u ", read_int_th_reg(1));
	printf("High threshold set is: %u ", read_int_th_reg(2));
	printf("Sensor reading: %f lux\n", lux_calc());
	return rand();
}

void light_exit()
{
	if(!exit_flag[LIGHT_THREAD_NUM])
	{
		rc_light = pthread_cancel(light_th);
		if(rc_light != 0)
			handle_error("Error cancelling light thread");
		
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
