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
}

void* temp_func(void* threadp)
{
	sleep(1);
	temp_queue_init();	
	srand(time(NULL));
	
	useconds_t garbage_sleep = 1;
	
	temp_data.debug_msg = malloc(30);
	
	while(1)
	{
		if(timer_flag[TEMP_THREAD_NUM] == 1)
		{
			timer_flag[TEMP_THREAD_NUM] = 0;
			temp_data.id = TEMP_THREAD_NUM; 
			temp_data.data = rand();
			clock_gettime(CLOCK_REALTIME, &(temp_data.time_stamp));
			temp_data.verbosity = 1;//(rand())%2;
			strcpy(temp_data.debug_msg, "GNU TEMPERATURE DEBUGGER!!!");
			rc_temp = mq_send(temp_queue_fd, (char*)&temp_data, sizeof(struct log_msg), 0);
			if(rc_temp == -1)
				handle_error("temp mq_send");
		}
		usleep(garbage_sleep);
		ack_heartbeat(TEMP_THREAD_NUM);
	}
	pthread_exit(NULL);
}

void temp_exit()
{
	if(!exit_flag[TEMP_THREAD_NUM])
	{
		rc_temp = pthread_cancel(temp_th);
		if(rc_temp != 0)
			handle_error("Error cancelling temperature thread");
		
		free(temp_data.debug_msg);
		rc_temp = mq_close(temp_queue_fd);
		if(rc_temp  == -1)
			handle_error("Error in closing temperature thread queue");
		
		printf("\nExiting temperature thread");
	}
}
