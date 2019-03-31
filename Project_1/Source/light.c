#include "../Include/light.h"

void* light_func(void* threadp)
{
	struct command req, res;
	struct timespec timeout;
	
	sleep(1);
	srand(time(NULL));
	
	useconds_t garbage_sleep = 1;
	
	while(1)
	{
		if(timer_flag[LIGHT_THREAD_NUM] == 1)
		{
			timer_flag[LIGHT_THREAD_NUM] = 0;

			light_data = write_to_log_queue(LIGHT_THREAD_NUM,
											request_light(),
											LOG_DEBUG,
											"GNU LIGHT DEBUGGER");
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
					handle_error("mq send in light");
			}
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
								handle_error("mq send in light");
						}
					}
					else
						handle_error("socket queue receive in light sensor");
				}
				else
				{
					if(req.action == REQUEST_LIGHT)
					{
						res.sensor_data = request_light();
						res.action = req.action;
						clock_gettime(CLOCK_REALTIME, &timeout);
						timeout.tv_nsec += 100000000;
						rc_light = mq_timedsend(soc_queue_fd, (char*)&res, sizeof(struct command), 0, &timeout);
						if(rc_light == -1)
						{
							if(errno == ETIMEDOUT)
							{
								printf("Log queue full...!!!\n");
							}
							else
								handle_error("light socket mq_send");
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
	return rand();
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
