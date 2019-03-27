#include "../Include/socket.h" 

void* socket_func(void* threadp)
{
	useconds_t garbage_sleep = 1;
	struct command req, res;
	
	soc_queue_init();
	
	while(1)
	{
		rc_socket = read(newserver_fd, (char *)&req, sizeof(struct command));
		if(rc_socket == -1)
			handle_error("socket read");
		
		if(req.id
		socket_req_id = ;
		socket_req_flag = 1;	
		rc_socket = mq_send(soc_queue_fd, (char*)&req, sizeof(struct command), 0);
		if(rc_socket == -1)
			handle_error("socket mq_send");
		
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

void soc_queue_init()
{
	soc_queue_attr.mq_flags = 0;
	soc_queue_attr.mq_maxmsg = 1;
	soc_queue_attr.mq_msgsize = sizeof(struct command);
	soc_queue_attr.mq_curmsgs = 0;

	soc_queue_fd = mq_open(socket_queue, O_RDWR | O_CREAT | O_EXCL, 0664, &queue_attr);
	if(soc_queue_fd == -1)
		handle_error("Error opening socket queue");
}
