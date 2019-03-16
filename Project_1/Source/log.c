#include "../Include/log.h"

void queue_init()
{
	queue_attr.mq_flags = 0;
	queue_attr.mq_maxmsg = 10;
	queue_attr.mq_msgsize = sizeof(struct log_msg);
	queue_attr.mq_curmsgs = 0;

	if(queue_fd = mq_open(queue_name, O_RDWR | O_CREAT | O_EXCL, 0664, &queue_attr) == -1)
	{
		handle_error("Error opening log queue");
	}
}

void* logger_func(void* threadp)
{
	struct log_param* logArg = (struct log_param*)threadp;
	struct log_msg log_data;
	if(len = mq_receive(queue_fd, (char*)&log_data, sizeof(struct log_msg), NULL) < 0)
	{
		handle_error("Error receiving from queue");
	}
}

void log_exit()
{
	if(check = mq_close(queue_fd) == -1)
	{
		handle_error("Error in closing queue");
	}
	if(check = mq_unlink(queue_name) == -1)
	{
		handle_error("Error in unlinking queue");
	}
	printf("\nQueue gracefully closed");
	printf("\nExiting logger thread");
}
