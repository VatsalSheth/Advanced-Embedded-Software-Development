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
	queue_init();
/*	if(len = mq_receive(queue_fd, (char*)&log_data, sizeof(struct log_msg), NULL) < 0)
	{
		handle_error("Error receiving from queue");
	}
*/
	file_log = fopen(logArg->file_name, "w");
	if(file_log == NULL)
	{
		handle_error("Error in creating file");
	}
	fprintf(file_log, "\nPROJECT 1 LOG FILE\n");
	fclose(file_log);
	
	while(1)
	{
		if(len = mq_receive(queue_fd, (char*)&log_data, sizeof(struct log_msg), NULL) < 0)
		{
			handle_error("Error receiving from queue");
		}
		clock_gettime(CLOCK_REALTIME, log_time);
		file_log = fopen(logArg->file_name, "w");
		if(file_log == NULL)
		{
			handle_error("Error in opening file");
		}

	}
	pthread_exit(NULL);
	
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
	if(check = fclose(file_log) != 0)
	{
		handle_error("Error in closing file");
	}
	if(check = pthread_cancel(log_th) != 0)
	{
		handle_error("Error cancelling logger thread");
	}
	printf("\nQueue and file closed");
	printf("\nExiting logger thread");
}
