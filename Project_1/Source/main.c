#include "../Include/main.h"



int main(int argc, char *argv[])
{
	set_signal_handler();
	
	if(!arg_init(argv[1], argv[2]))
	{
		return 0;
	}
	
	thread_create();
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
	rc = pthread_join(temp_th, NULL);
	if(rc != 0)
	{
		handle_error("Error in joining temperature sensor thread");
	}
}

void thread_create()
{
	rc = pthread_create(&log_th, (void *)0, logger_func, (void *)&(log_file));
	if(rc != 0)
	{
		handle_error("Error in creating logger thread");
	}

	rc = pthread_create(&temp_th, (void *)0, temp_func, (void *)0);
	if(rc != 0)
	{
		handle_error("Error in creating temperature sensor thread");
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
	log_exit();
	temp_exit();
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
