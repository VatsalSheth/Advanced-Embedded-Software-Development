#include "../Include/temp.h"

void temp_queue_init()
{
	temp_queue_attr.mq_flags = 0;	
	temp_queue_attr.mq_maxmsg = 10;	
	temp_queue_attr.mq_msgsize = sizeof(struct log_msg);	
	temp_queue_attr.mq_curmsgs = 0;

	temp_queue_fd = mq_open(queue_name, O_RDWR, 0664, &temp_queue_attr);
	if(temp_queue_fd  == -1)
		handle_error("Error opening temperature thread queue");
	
	temp_soc_queue_attr.mq_flags = 0;
	temp_soc_queue_attr.mq_maxmsg = 1;
	temp_soc_queue_attr.mq_msgsize = sizeof(struct command);
	temp_soc_queue_attr.mq_curmsgs = 0;

	temp_soc_queue_fd = mq_open(socket_queue, O_RDWR, 0664, &temp_soc_queue_attr);
	if(temp_soc_queue_fd == -1)
		handle_error("Error opening socket queue in temperature sensor");
}

void* temp_func(void* threadp)
{
	struct command req, res;
	sleep(1);
	temp_queue_init();	
	srand(time(NULL));
	
	useconds_t garbage_sleep = 1;
	
	while(1)
	{
		if(timer_flag[TEMP_THREAD_NUM] == 1)
		{
			timer_flag[TEMP_THREAD_NUM] = 0;
			
			temp_data = write_to_log_queue(TEMP_THREAD_NUM,
											request_temp(),
											LOG_DEBUG,
											"GNU TEMPERATURE DEBUGGER");
			rc_temp = mq_send(temp_queue_fd, (char*)&temp_data, sizeof(struct log_msg), 0);
			if(rc_temp == -1)
				handle_error("temp mq_send");
		}
		
		if(socket_req_flag == 1)
		{
			if(socket_req_id == TEMP_THREAD_NUM)
			{
				rc_temp = mq_receive(temp_soc_queue_fd, (char*)&req, sizeof(struct command), NULL);
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
						rc_temp = mq_send(temp_soc_queue_fd, (char*)&res, sizeof(struct command), 0);
						if(rc_temp == -1)
							handle_error("temp socket mq_send");
					}
					else if(req.action == REQUEST_TEMPERATURE_F)
					{
						res.sensor_data = request_temp();
						res.sensor_data = conv_temp(res.sensor_data, 'F');
						res.action = req.action;
						rc_temp = mq_send(temp_soc_queue_fd, (char*)&res, sizeof(struct command), 0);
						if(rc_temp == -1)
							handle_error("temp socket mq_send");
					}
					else if(req.action == REQUEST_TEMPERATURE_K)
					{
						res.sensor_data = request_temp();
						res.sensor_data = conv_temp(res.sensor_data, 'K');
						res.action = req.action;
						rc_temp = mq_send(temp_soc_queue_fd, (char*)&res, sizeof(struct command), 0);
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
	return rand();
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
		
		rc_temp = mq_close(temp_queue_fd);
		if(rc_temp  == -1)
			handle_error("Error in closing temperature thread queue");
		
		rc_temp = mq_close(temp_soc_queue_fd);
		if(rc_temp  == -1)
			handle_error("Error in closing temperature thread socket queue");
		
		printf("Exiting temperature thread\n");
		exit_flag[TEMP_THREAD_NUM] = 2;
	}
	else if(exit_flag[TEMP_THREAD_NUM] == 1)
	{
		//rc_temp = mq_close(temp_queue_fd);
		//if(rc_temp  == -1)
		//	handle_error("Error in closing temperature thread queue");
		
		//rc_temp = mq_close(temp_soc_queue_fd);
		//if(rc_temp  == -1)
		//	handle_error("Error in closing temperature thread socket queue");
		
		printf("Exiting temperature thread\n");
		exit_flag[TEMP_THREAD_NUM] = 2;
		pthread_exit(NULL);
	}
}
