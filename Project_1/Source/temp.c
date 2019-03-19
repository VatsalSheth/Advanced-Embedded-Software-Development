#include "../Include/temp.h"

void temp_queue_init()
{
	temp_check = mq_close(temp_queue_fd);
	temp_check = mq_unlink(queue_name);

	temp_queue_attr.mq_flags = 0;	
	temp_queue_attr.mq_maxmsg = 10;	
	temp_queue_attr.mq_msgsize = sizeof(struct log_msg);	
	temp_queue_attr.mq_curmsgs = 0;

	if(temp_queue_fd = mq_open(queue_name, O_RDWR, 0664, &queue_attr) == -1)
	{
		handle_error("Error opening temperature thread queue");
	}	
}

void* temp_func(void* threadp)
{
	sleep(1);
//	temp_queue_init();	
	srand(time(NULL));
}

void temp_exit()
{
	if(temp_check = mq_close(temp_queue_fd) == -1)
	{
		handle_error("Error in closing temperature thread queue");
	}
	if(temp_check = mq_unlink(queue_name) == -1)
	{
		handle_error("Error in unlinking temperature thread queue");
	}
	if(temp_check = pthread_cancel(temp_th) != 0)
	{
		handle_error("Error cancelling temperature thread queue");
	}
	printf("\nExiting temperature thread");
}
