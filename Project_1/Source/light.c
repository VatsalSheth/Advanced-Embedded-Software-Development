#include "../Include/light.h"

void* light_func(void* threadp)
{
	struct command req, res;
	
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
