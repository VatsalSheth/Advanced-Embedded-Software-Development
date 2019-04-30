/***********************************************************************************
* @log.c
* @This file contains message queues for writing to logger thread.
* 
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#include "../Include/log.h"

/**
 * @brief 
 *The queue_init() function initializes a common queue for all threads to write to the logger thread queue.
 *A structure log_msg is commonly defined in log.h, which has fields of thread id, sensor data, debug messgaes 
 *and verbosity.
 */
void queue_init()
{
//	rc_log = mq_unlink(queue_name);
//		if(rc_log == -1)
//			handle_error("Error in unlinking logger thread queue");
	queue_attr.mq_flags = 0;
	queue_attr.mq_maxmsg = 10;
	queue_attr.mq_msgsize = sizeof(struct log_msg);
	queue_attr.mq_curmsgs = 0;

	queue_fd = mq_open(queue_name, O_RDWR | O_CREAT | O_EXCL, 0664, &queue_attr);
	if(queue_fd == -1)
		handle_error("Error opening log queue");
}

/**
 * @brief 
 *The logger thread function, spawned by main thread. A log file is opened, and messages received from the queue are checked basis the verbosity flag.
 *If flag is error message, an error with the source is printed, else the data with timestamp and thread ids printed out. mq_notify is used, along with
 *a flag to check if the queue has been written to or not.
 * 
 * @param threadp
 *A structure is passed as argument, containing the default verbosity and log file name as entered by user on prompt by main thread.
 *
 * @return 
 */
void* logger_func(void* threadp)
{
	struct log_msg log_data;
	struct timespec rx_timeout;
	useconds_t garbage_sleep = 1;
	struct mq_attr get_attr;
	
	queue_init();
	
	file_log = fopen(filename, "a");
	if(file_log == NULL)
		handle_error("Error in creating file");

	fprintf(file_log, "\nPROJECT 2 LOG FILE\n");
	while(1)
	{	
			rc_log = mq_receive(queue_fd, (char*)&log_data, sizeof(struct log_msg), NULL);
			if(log_data.id == POLL_IRQ)
			{
				fprintf(file_log, "\nLongitude: %f; Latitude: %f", log_data.lat, log_data.lon);
				if(log_data.status & 0x01)
				{
					fprintf(file_log, "\nACCIDENT OCCURRED");
				}	
				if(log_data.status & 0x10)
				{
					fprintf(file_log, "\nGPS module error");
				}	
				if(log_data.status & 0x20)
				{
					fprintf(file_log, "\nAccelerometer sensor error");
				}	
				fprintf(file_log, "\n***********************************\n");
			}
			else if(log_data.id == TIMER_ISR) 
			{
				fprintf(file_log, "\n[TIMESTAMP: %d %d %u]", log_data.hours, log_data.minutes, log_data.seconds);
				fprintf(file_log, "\n***********************************\n");
			}
		usleep(garbage_sleep);
	}
	pthread_exit(NULL);
	
}



