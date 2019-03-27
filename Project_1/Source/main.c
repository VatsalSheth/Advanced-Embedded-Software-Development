#include "../Include/main.h"

int main(int argc, char *argv[])
{
	set_signal_handler();
	exit_cond = 1;
	
	if(!arg_init(argv[1], argv[2]))
	{
		return 0;
	}
	
	thread_create();
	while(exit_cond)
	{
		heartbeat_check();
	}
	thread_join();
	
	return 0;
}

void thread_join()
{
	rc = pthread_join(log_th, NULL);
	if(rc != 0)
	{
		handle_error("Error in joining logger thread");
	}
	else 
	{
		pthread_cond_destroy(&mon[LOG_THREAD_NUM].cond);
		pthread_mutex_destroy(&mon[LOG_THREAD_NUM].lock);
	}

	rc = pthread_join(temp_th, NULL);
	if(rc != 0)
	{
		handle_error("Error in joining temperature sensor thread");
	}
	else 
	{
		pthread_cond_destroy(&mon[TEMP_THREAD_NUM].cond);
		pthread_mutex_destroy(&mon[TEMP_THREAD_NUM].lock);
	}	
		
	rc = pthread_join(light_th, NULL);
	if(rc != 0)
	{
		handle_error("Error in joining light sensor thread");
	}
	else 
	{
		pthread_cond_destroy(&mon[LIGHT_THREAD_NUM].cond);
		pthread_mutex_destroy(&mon[LIGHT_THREAD_NUM].lock);
	}
	
	rc = pthread_join(socket_th, NULL);
	if(rc != 0)
	{
		handle_error("Error in joining socket sensor thread");
	}
	else 
	{
		pthread_cond_destroy(&mon[SOCKET_THREAD_NUM].cond);
		pthread_mutex_destroy(&mon[SOCKET_THREAD_NUM].lock);
	}
}

void thread_create()
{
	rc = pthread_create(&log_th, (void *)0, logger_func, (void *)&(log_file));
	if(rc != 0)
	{
		handle_error("Error in creating logger thread");
	}
	else
	{
		rc = pthread_cond_init(&mon[LOG_THREAD_NUM].cond, NULL); 
		if(rc!=0)
			handle_error("pthread cond init");
			
		rc = pthread_mutex_init(&mon[LOG_THREAD_NUM].lock, NULL); 
		if(rc!=0)
			handle_error("pthread mutex init");
	}
	
	rc = pthread_create(&socket_th, (void *)0, socket_func, (void *)0);
	if(rc != 0)
	{
		handle_error("Error in creating socket sensor thread");
	}
	else
	{
		rc = pthread_cond_init(&mon[SOCKET_THREAD_NUM].cond, NULL); 
		if(rc!=0)
			handle_error("pthread cond init");
			
		rc = pthread_mutex_init(&mon[SOCKET_THREAD_NUM].lock, NULL); 
		if(rc!=0)
			handle_error("pthread mutex init");
	}

	rc = pthread_create(&temp_th, (void *)0, temp_func, (void *)0);
	if(rc != 0)
	{
		handle_error("Error in creating temperature sensor thread");
	}
	else
	{
		rc = pthread_cond_init(&mon[TEMP_THREAD_NUM].cond, NULL); 
		if(rc!=0)
			handle_error("pthread cond init");
			
		rc = pthread_mutex_init(&mon[TEMP_THREAD_NUM].lock, NULL); 
		if(rc!=0)
			handle_error("pthread mutex init");
	}
	
	rc = pthread_create(&light_th, (void *)0, light_func, (void *)0);
	if(rc != 0)
	{
		handle_error("Error in creating light sensor thread");
	}
	else
	{
		rc = pthread_cond_init(&mon[LIGHT_THREAD_NUM].cond, NULL); 
		if(rc!=0)
			handle_error("pthread cond init");
			
		rc = pthread_mutex_init(&mon[LIGHT_THREAD_NUM].lock, NULL); 
		if(rc!=0)
			handle_error("pthread mutex init");
	}
}

int arg_init(char *arg1, char *arg2)
{
	if(arg1 == NULL)
	{
		printf("No file input given...!!!\n");
		return 0;
	}
	else if(!strcmp(arg1, "Debug"))
	{
		printf("File name cannot be debug...!!!\n");
		return 0;
	}
	
	log_file.file_name = arg1;
	
	if(!strcmp(arg2, "Debug"))
	{
		log_file.log_verbosity = LOG_DEBUG;
	}
	else
	{
		log_file.log_verbosity = LOG_NONE;
	}
	
	return 1;
}

void signal_handler(int signo, siginfo_t *info, void *extra) 
{	
	timer_delete(timer_id);
	log_exit();
	temp_exit();
	light_exit();
	socket_exit();
	printf("\n");
	exit_cond = 0;
}

void set_signal_handler(void)
{
	struct sigaction action;
 
    action.sa_flags = SA_SIGINFO; 
    action.sa_sigaction = signal_handler;
 
    if (sigaction(SIGINT, &action, NULL) == -1)
		handle_error("SIGINT: sigaction")
}

void heartbeat_check(void)
{
	uint32_t i;
	for(i=0; i<(NUM_OF_THREADS - socket_hb); i++)
	{	
		if(!exit_cond)
			break;
			
		clock_gettime(CLOCK_REALTIME, &mon[i].timeout);
		mon[i].timeout.tv_sec += 4;
	
		pthread_mutex_lock(&mon[i].lock);
		rc = pthread_cond_timedwait(&mon[i].cond, &mon[i].lock, &mon[i].timeout);  
		if(rc == ETIMEDOUT)
		{
			printf("fail %d\n",i);
		}
		pthread_mutex_unlock(&mon[i].lock);
	}
}
