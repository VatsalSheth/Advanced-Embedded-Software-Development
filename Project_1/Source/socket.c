#include "../Include/socket.h" 

void* socket_func(void* threadp)
{
	useconds_t garbage_sleep = 1;
	struct command user;
	socket_hb = 1;
	char string[60];

	port = SERV_TCP_PORT;

	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		handle_error("Failed to create socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
	if(bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		handle_error("can't bind local address");
	struct command req, res;
	
	soc_queue_init();
	
	listen(server_fd, BACKLOG);

	clilen = sizeof(cli_addr);
	while(socket_hb)
	{
		newserver_fd = accept(server_fd, (struct sockaddr *) &cli_addr, &clilen);
		if(errno == EAGAIN)
		{
			ack_heartbeat(SOCKET_THREAD_NUM);
			sleep(1);
		}	
		else if(newserver_fd < 0) 
		{
			handle_error("Unable to accept connection");
		}
		else
		{
			socket_hb = 0;
			printf("\nConnection with remote client established");
		}
	}

	while(1)
	{
		len = read(newserver_fd, (char*)&user, sizeof(struct command));	
		if (len < 0)
		{
			handle_error("Receiving Read failed");
		}
		else if (len == 0)
		{
			usleep(garbage_sleep);
			ack_heartbeat(SOCKET_THREAD_NUM);	
		}
		else
		{
//			string[len] = 0;		
			printf("\nReceived %d bytes: %d\n", len, user.action);
			ack_heartbeat(SOCKET_THREAD_NUM);
		}
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
	rc_socket = close(newserver_fd);
	if(rc_socket != 0)
		handle_error("Error closing socket file descriptor");

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
