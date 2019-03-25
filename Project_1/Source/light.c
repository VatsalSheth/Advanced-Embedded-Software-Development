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
}

void* light_func(void* threadp)
{
	sleep(1);
	light_queue_init();	
	srand(time(NULL));
	
	useconds_t garbage_sleep = 1;
	
	light_data.debug_msg = malloc(30);
	
	while(1)
	{
		if(timer_flag[LIGHT_THREAD_NUM] == 1)
		{
			timer_flag[LIGHT_THREAD_NUM] = 0;
			light_data.id = LIGHT_THREAD_NUM; 
			light_data.data = rand();
			clock_gettime(CLOCK_REALTIME, &(light_data.time_stamp));
			light_data.verbosity = 1;//(rand())%2;
			strcpy(light_data.debug_msg, "GNU LIGHT DEBUGGER!!!");
			rc_light = mq_send(light_queue_fd, (char*)&light_data, sizeof(struct log_msg), 0);
		}
		usleep(garbage_sleep);
		ack_heartbeat(LIGHT_THREAD_NUM);
	}
	pthread_exit(NULL);
}

void light_exit()
{
	free(light_data.debug_msg);
	rc_light = mq_close(light_queue_fd);
	if(rc_light  == -1)
		handle_error("Error in closing light thread queue");

	rc_light = pthread_cancel(light_th);
	if(rc_light != 0)
		handle_error("Error cancelling light thread");
	printf("\nExiting light thread");
}
