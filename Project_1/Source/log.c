/***********************************************************************************
* @log.c
* @This file contains message queues for writing to logger thread, timer 
* initialization and handler, log entry and exit functions and an acknowledgement
* heartbeat function, used by other threads to notify main they are still alive.
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
	struct log_param* logArg = (struct log_param*)threadp;
	struct log_msg log_data;
	struct timespec rx_timeout;
	useconds_t garbage_sleep = 1;
	struct mq_attr get_attr;
	
	queue_init();
	set_notify_signal();

	timer_init();
	
	file_log = fopen(logArg->file_name, "w");
	if(file_log == NULL)
		handle_error("Error in creating file");

	fprintf(file_log, "\nPROJECT 1 LOG FILE\n");
	fprintf(file_log, "%s",bist_buffer[0]);
	fprintf(file_log, "%s",bist_buffer[1]);
	while(1)
	{	
		if(data_avail == 1)
		{
			clock_gettime(CLOCK_REALTIME, &rx_timeout);
			rx_timeout.tv_nsec += 100000;
			rc_log = mq_timedreceive(queue_fd, (char*)&log_data, sizeof(struct log_msg), NULL, &rx_timeout);
			if(rc_log < 0)
			{
				if(errno == ETIMEDOUT)
				{
					data_avail = 0;
					if(mq_notify(queue_fd, &sev) == -1)
						handle_error("mq_notify");	
				}
				else
					handle_error("mq receive");
			}
			
			clock_gettime(CLOCK_REALTIME,&(log_data.time_stamp));
			if((log_data.verbosity) != ERROR_MESSAGE)
			{
				fprintf(file_log, "\n[TIMESTAMP: %lu secs and %lu nsecs]", log_data.time_stamp.tv_sec, log_data.time_stamp.tv_nsec);
				fprintf(file_log, "\nLog level: %s", (logArg->log_verbosity==1)?"DEBUG":"NONE");
				fprintf(file_log, "\nSource: %s", log_data.id==TEMP_THREAD_NUM?"TEMPERATURE SENSOR":
								 (log_data.id==LIGHT_THREAD_NUM?"LIGHT SENSOR":
								 (log_data.id==SOCKET_THREAD_NUM?"SOCKET THREAD":
								 (log_data.id==MAIN_THREAD_NUM?"MAIN THREAD":"FAULTY THREAD"))));
				fprintf(file_log, "\nData: %f %s", log_data.data, log_data.id==TEMP_THREAD_NUM?"degrees Celsius":"Lumens");
				fprintf(file_log, "\nDebug Message: %s", logArg->log_verbosity?(log_data.verbosity?log_data.debug_msg:"none"):"none");	
				fprintf(file_log, "\n***********************************\n");
			}
			else if(log_data.verbosity == ERROR_MESSAGE) 
			{
//				printf("error\n");
				fprintf(file_log, "\n[TIMESTAMP: %lu secs and %lu nsecs]", log_data.time_stamp.tv_sec, log_data.time_stamp.tv_nsec);
			//	fprintf(file_log, "\nLog level: %s", (logArg->log_verbosity==1)?"DEBUG":"NONE");
				fprintf(file_log, "\nSource: %s", log_data.id==TEMP_THREAD_NUM?"TEMPERATURE SENSOR":
								 (log_data.id==LIGHT_THREAD_NUM?"LIGHT SENSOR":
								 (log_data.id==SOCKET_THREAD_NUM?"SOCKET THREAD":
								 (log_data.id==MAIN_THREAD_NUM?"MAIN THREAD":"FAULTY THREAD"))));
				fprintf(file_log, "\nERROR MESSAGE: %s", log_data.debug_msg);	
				fprintf(file_log, "\n***********************************\n");
			}
		}
		usleep(garbage_sleep);
		ack_heartbeat(LOG_THREAD_NUM);
	}
	pthread_exit(NULL);
	
}

/**
 * @brief
 * Timer for each sensor thread is created, with a trigger rate of 1 seconds.
 */
void timer_init()
{
	struct sigevent sev;
	struct itimerspec trigger;
	
	memset(&sev, 0, sizeof(struct sigevent));
    memset(&trigger, 0, sizeof(struct itimerspec));
    
    sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = timer_handle;
	
	timer_create(CLOCK_REALTIME, &sev, &timer_id);
	
	trigger.it_interval.tv_sec = 1;
	trigger.it_value.tv_sec = 1;
		
	timer_settime(timer_id, 0, &trigger, NULL);
}

/**
 * @brief 
 * Timer handle function, on being called, sets a timer flag for each thread, and the flag is cleared when the thread receives timer interrupt.
 * @param sv
 */
void timer_handle(union sigval sv)
{
	for(uint32_t i=0; i<(NUM_OF_THREADS-2); i++)
	{
		timer_flag[i] = 1;
	}
}

/**
 * @brief
 * Log thread is created, and its conditional variables and mutex initialized. This function is called by main.
 */
void log_entry(void)
{
	exit_flag[LOG_THREAD_NUM] = 0;
	
	rc_log = pthread_create(&log_th, (void *)0, logger_func, (void *)&(log_file));
	if(rc_log != 0)
	{
		handle_error_exit("Error in creating logger thread");
	}
	else
	{
		rc_log = pthread_cond_init(&mon[LOG_THREAD_NUM].cond, NULL); 
		if(rc_log!=0)
			handle_error_exit("pthread cond init");
			
		rc_log = pthread_mutex_init(&mon[LOG_THREAD_NUM].lock, NULL); 
		if(rc_log!=0)
			handle_error_exit("pthread mutex init");
	}
}

/**
 * @brief 
 * Thread is cancelled, message queue is closed and unlinked, log file is closed,allowing for graceful exit.
 */
void log_exit()
{
	if(!exit_flag[LOG_THREAD_NUM])
	{
		rc_log = pthread_cancel(log_th);
		if(rc_log != 0)
			handle_error("Error cancelling logger thread");
		
		rc_log = mq_close(queue_fd);
		if(rc_log == -1)
			handle_error("Error in closing logger thread queue");
		
		rc_log = mq_unlink(queue_name);
		if(rc_log == -1)
			handle_error("Error in unlinking logger thread queue");
		
		rc_log = fclose(file_log);
		if(rc_log != 0)
			handle_error("Error in closing file");
		
		printf("Log file closed\n");
		printf("Exiting logger thread\n");
		exit_flag[LOG_THREAD_NUM] = 1;
	}
}

/**
 * @brief 
 *The heartbeat function, which will be called by each thread to ensure it is not dead. Conditional variable signal is given, after being protected by a mutex.
 * 
 * @param th_num
 * Thread number
 */
void ack_heartbeat(uint32_t th_num)
{
	pthread_mutex_lock(&mon[th_num].lock);
	pthread_cond_signal(&mon[th_num].cond);  
	pthread_mutex_unlock(&mon[th_num].lock);
}


void set_notify_signal()
{
	sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = notify_handler;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = NULL;
    
    if(mq_notify(queue_fd, &sev) == -1)
		handle_error("mq_notify");
}

/**
 * @brief 
 * Flag indicating data is available is set, to indicate to logger thread to enter functionality once mq_notify handler occurs.
 */
void notify_handler(union sigval sv)
{
	//printf("notify\n");   //DEBUG
	data_avail = 1;
}

/**
 * @brief 
 * A function for writing values to the structure to be sent via mqueue.
 *
 * @return 
 */
struct log_msg write_to_log_queue(uint32_t thread_id,
				    float thread_data,
				    uint32_t msg_verbosity,
				    char* msg)
{
	struct log_msg sensor_data;
	sensor_data.id = thread_id;
	sensor_data.data = thread_data;
	clock_gettime(CLOCK_REALTIME, &(sensor_data.time_stamp));
	sensor_data.verbosity = msg_verbosity;
	sensor_data.debug_msg = msg;
	return sensor_data;
}
