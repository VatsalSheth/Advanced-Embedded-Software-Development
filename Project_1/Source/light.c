#include "../Include/light.h"

#define THREAD_NUM 2

void light_queue_init()
{
	light_queue_attr.mq_flags = 0;	
	light_queue_attr.mq_maxmsg = 10;	
	light_queue_attr.mq_msgsize = sizeof(struct log_msg);	
	light_queue_attr.mq_curmsgs = 0;

	light_queue_fd = mq_open(queue_name, O_RDWR, 0664, &light_queue_attr);
	if(light_queue_fd  == -1)
		handle_error("Error opening light thread queue");
		
	light_soc_queue_attr.mq_flags = 0;
	light_soc_queue_attr.mq_maxmsg = 1;
	light_soc_queue_attr.mq_msgsize = sizeof(struct command);
	light_soc_queue_attr.mq_curmsgs = 0;

	light_soc_queue_fd = mq_open(socket_queue, O_RDWR, 0664, &light_soc_queue_attr);
	if(light_soc_queue_fd == -1)
		handle_error("Error opening socket queue in light sensor");
}

void* light_func(void* threadp)
{
	struct command req, res;
	
	sleep(1);
	light_queue_init();	
	srand(time(NULL));
	
	useconds_t garbage_sleep = 1;
	
	while(1)
	{
		if(timer_flag[LIGHT_THREAD_NUM] == 1)
		{
			timer_flag[LIGHT_THREAD_NUM] = 0;

			light_data = write_to_log_queue(LIGHT_THREAD_NUM,
											request_light(),
											1,
											"GNU LIGHT DEBUGGER");
			rc_light = mq_send(light_queue_fd, (char*)&light_data, sizeof(struct log_msg), 0);
			if(rc_light == -1)
				handle_error("light mq_send");
		}
		
		if(socket_req_flag == 1)
		{
			if(socket_req_id == LIGHT_THREAD_NUM)
			{
				rc_light = mq_receive(light_soc_queue_fd, (char*)&req, sizeof(struct command), NULL);
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
						rc_light = mq_send(light_soc_queue_fd, (char*)&res, sizeof(struct command), 0);
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
		
		rc_light = mq_close(light_queue_fd);
		if(rc_light  == -1)
			handle_error("Error in closing light thread queue");
		
		rc_light = mq_close(light_soc_queue_fd);
		if(rc_light  == -1)
			handle_error("Error in closing light thread socket queue");
			
		printf("\nExiting light thread\n");
	}
	else if(exit_flag[LIGHT_THREAD_NUM] == 1)
	{
		exit_flag[LIGHT_THREAD_NUM] = 2;
		rc_light = mq_close(light_queue_fd);
		if(rc_light  == -1)
			handle_error("Error in closing light thread queue");
		
		rc_light = mq_close(light_soc_queue_fd);
		if(rc_light  == -1)
			handle_error("Error in closing light thread socket queue");
			
		printf("\nExiting light thread\n");
		pthread_exit(NULL);
	}
}
