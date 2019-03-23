#include "../Include/log.h"

#define Thread_num 0

void queue_init()
{
	queue_attr.mq_flags = 0;
	queue_attr.mq_maxmsg = 10;
	queue_attr.mq_msgsize = sizeof(struct log_msg);
	queue_attr.mq_curmsgs = 0;

	queue_fd = mq_open(queue_name, O_RDWR | O_CREAT | O_EXCL, 0664, &queue_attr);
	if(queue_fd == -1)
		handle_error("Error opening log queue");
}

void* logger_func(void* threadp)
{
	struct log_param* logArg = (struct log_param*)threadp;
	struct log_msg log_data;
	queue_init();
	
	file_log = fopen(logArg->file_name, "w");
	if(file_log == NULL)
		handle_error("Error in creating file");

	fprintf(file_log, "\nPROJECT 1 LOG FILE\n");
	
	while(1)
	{
		rc_log = mq_receive(queue_fd, (char*)&log_data, sizeof(struct log_msg), NULL);
		if(rc_log < 0)
			handle_error("Error receiving from queue");
		
		clock_gettime(CLOCK_REALTIME,&(log_data.time_stamp));
		fprintf(file_log, "TIMESTAMP: %lu secs and %lu nsecs ; ", log_data.time_stamp.tv_sec, log_data.time_stamp.tv_nsec);
		fprintf(file_log, "Log level: %s ; ", logArg->log_verbosity?"DEBUG":"NONE");
		fprintf(file_log, "Source thread ID: %u ; ", log_data.id);
		fprintf(file_log, "Data: %f ; ", log_data.data);
		fprintf(file_log, "Debug Message: %s.\n", log_data.verbosity?log_data.debug_msg:"none");	
		//differentiate print statement (data) based on which thread data comes from
		ack_heartbeat(Thread_num);
	}
	pthread_exit(NULL);
	
}

void log_exit()
{
	if(check = mq_close(queue_fd) == -1)
		handle_error("Error in closing logger thread queue");
	
	if(check = mq_unlink(queue_name) == -1)
		handle_error("Error in unlinking logger thread queue");
	
	if(check = fclose(file_log) != 0)
		handle_error("Error in closing file");
	
	if(check = pthread_cancel(log_th) != 0)
		handle_error("Error cancelling logger thread");
	
	printf("\nQueue and file closed");
	printf("\nExiting logger thread");
}

void ack_heartbeat(uint32_t th_num)
{
	pthread_mutex_lock(&mon[th_num].lock);
	pthread_cond_signal(&mon[th_num].cond);  
	pthread_mutex_unlock(&mon[th_num].lock);
}
