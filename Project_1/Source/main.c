#include "../Include/main.h"

int main(int argc, char *argv[])
{
	set_signal_handler();
	
	if(!arg_init(argv[1], argv[2]))
	{
		return 0;
	}
	
	thread_create();
	//while(1)
	{
		heartbeat_check();
	}
	thread_join();
	
	return 0;
}

void thread_join()
{
	printf("join %ld\n",log_th);
	rc = pthread_join(log_th, NULL);
	printf("rc %d\n",rc);
	if(rc != 0)
		handle_error("Error in joining logger thread"); 
	
	rc = pthread_join(temp_th, NULL);
	if(rc != 0)
		handle_error("Error in joining temperature sensor thread");
		
	rc = pthread_join(light_th, NULL);
	if(rc != 0)
		handle_error("Error in joining light sensor thread");
}

void thread_create()
{
	rc = pthread_create(&log_th, (void *)0, logger_func, (void *)&(log_file));
	printf("create %ld\n",log_th);
	if(rc != 0)
	{
		handle_error("Error in creating logger thread");
	}
	else
	{
		rc = pthread_cond_init(&mon[0].cond, NULL); 
		if(rc!=0)
			handle_error("pthread cond init");
			
		rc = pthread_mutex_init(&mon[0].lock, NULL); 
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
		rc = pthread_cond_init(&mon[2].cond, NULL); 
		if(rc!=0)
			handle_error("pthread cond init");
			
		rc = pthread_mutex_init(&mon[2].lock, NULL); 
		if(rc!=0)
			handle_error("pthread mutex init");
	}
	
	/*rc = pthread_create(&light_th, (void *)0, light_func, (void *)0);
	if(rc != 0)
	{
		handle_error("Error in creating light sensor thread");
	}
	else
	{
		rc = pthread_cond_init(&mon[1].cond, NULL); 
		if(rc!=0)
			handle_error("pthread cond init");
			
		rc = pthread_mutex_init(&mon[1].lock, NULL); 
		if(rc!=0)
			handle_error("pthread mutex init");
	}*/
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
	uint32_t i;
	
	log_exit();
	temp_exit();
	for(i=0; i<NUM_OF_THREADS; i++)
	{
		pthread_cond_destroy(&mon[i].cond);
		pthread_mutex_destroy(&mon[i].lock);
	}
	printf("\n");
	exit(0);
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
	
	for(i=0; i<NUM_OF_THREADS; i++)
	{	
		clock_gettime(CLOCK_REALTIME, &mon[i].timeout);
		mon[i].timeout.tv_sec += 2;
	
		pthread_mutex_lock(&mon[i].lock);
		rc = pthread_cond_timedwait(&mon[i].cond, &mon[i].lock, &mon[i].timeout);  
		if(rc == ETIMEDOUT)
		{
			printf("fail %d\n",i);
		}
		pthread_mutex_unlock(&mon[i].lock);
	}
}
