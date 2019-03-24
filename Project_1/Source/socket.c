#include "../Include/socket.h" 

void* socket_func(void* threadp)
{
	useconds_t garbage_sleep = 1;
	
	while(1)
	{
		usleep(garbage_sleep);
		ack_heartbeat(SOCKET_THREAD_NUM);
	}
	pthread_exit(NULL);
}

void socket_exit(void)
{
	rc_socket = pthread_cancel(socket_th);
	if(rc_socket != 0)
		handle_error("Error cancelling socket thread");
	printf("\nExiting socket thread");
}
