/***********************************************************************************
* @socket.c
* @This file contains server code for remote client. It accepts connection and send request
* to respective sensor threads using message queues. It then send back the data to remote
* client
* 
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#include "../Include/socket.h" 

/**
 * @brief 
 * This function is the thread for socket. It accepts connection and passes on request to respective sensor
 * thread and send back the reply to client.
 * @param threadp
 *
 * @return 
 */
void* socket_func(void* threadp)
{	
//	rc_socket = mq_unlink(socket_queue);
//		if(rc_socket == -1)
//			handle_error("Error in unlinking socket thread mqueue");

	uint32_t tmp_flag;
	useconds_t garbage_sleep = 1;
	struct command req, res;
	socket_hb = 1;
	struct timespec rx_timeout;
	char string[60];	//for testing only

	port = SERV_TCP_PORT;

	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		handle_error("Failed to create socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
	if(bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		handle_error("can't bind local address");
	
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
			int flags = fcntl(newserver_fd, F_GETFL, 0);
			fcntl(newserver_fd, F_SETFL, flags | O_NONBLOCK);
			printf("\nConnection with remote client established\n");
		}
	}
	
	while(1)
	{
		len = read(newserver_fd, (char*)&req, sizeof(struct command));	
		if (len < 0)
		{
			if(errno != EAGAIN)
				handle_error("Receiving Read failed");
		}
		else if(len > 0)
		{
			printf("\nReceived %d bytes: %d\n", len, req.action);	//for testing only
					
			if((req.action == REQUEST_TEMPERATURE_C) || (req.action == REQUEST_TEMPERATURE_F) || (req.action == REQUEST_TEMPERATURE_K) || (req.action == KILL_TEMPERATURE))
			{
				socket_req_id = TEMP_THREAD_NUM;
				rc_socket = mq_send(soc_queue_fd, (char*)&req, sizeof(struct command), 0);
				if(rc_socket == -1)
				{
					handle_error("socket mq_send");
				}	
				else
				{
					socket_req_flag = 1;
					if(req.action != KILL_TEMPERATURE)
					{
						tmp_flag = 1;
					}
				}
			}
			else if((req.action == REQUEST_LIGHT) || (req.action == KILL_LIGHT))
			{
				socket_req_id = LIGHT_THREAD_NUM;
				rc_socket = mq_send(soc_queue_fd, (char*)&req, sizeof(struct command), 0);
				if(rc_socket == -1)
				{
					handle_error("socket mq_send");
				}
				else
				{
					socket_req_flag = 1;
					if(req.action != KILL_LIGHT)
					{
						tmp_flag = 1;
					}
				}
			}
			else if(req.action == KILL_SOCKET)
			{
				exit_flag[SOCKET_THREAD_NUM] = 1;
				socket_exit();
			}
			else if(req.action == KILL_LOGGER)
			{
				log_exit();
				exit_flag[LOG_THREAD_NUM] = 1;
			}
			
			while(tmp_flag == 1)
			{
				if((socket_req_flag == 0) 
					|| ((exit_flag[LIGHT_THREAD_NUM] == 2) && (socket_req_id == LIGHT_THREAD_NUM)) 
					|| ((socket_req_id == TEMP_THREAD_NUM) && (exit_flag[TEMP_THREAD_NUM] == 2)))
				{
					clock_gettime(CLOCK_REALTIME, &rx_timeout);
					rx_timeout.tv_sec += 1;
					rc_socket = mq_timedreceive(soc_queue_fd, (char*)&res, sizeof(struct command), NULL, &rx_timeout);
					if(rc_socket < 0)
					{
						if(errno == ETIMEDOUT)
						{
							socket_req_flag = 0;
							res.action = REQUEST_FAIL;
						}
						else
							handle_error("socket mq_receive");
					}
					if(((exit_flag[LIGHT_THREAD_NUM] == 2) && (socket_req_id == LIGHT_THREAD_NUM)) 
						|| ((socket_req_id == TEMP_THREAD_NUM) && (exit_flag[TEMP_THREAD_NUM] == 2)))
					{
						socket_req_flag = 0;
						res.action = REQUEST_FAIL;
					}
					tmp_flag = 0;
					//printf("Received from light:%f\n", res.sensor_data);
					rc_socket = write(newserver_fd, (void*)&res, sizeof(struct command));
					if(rc_socket == -1)
						handle_error("write");
				}
			}
		}
		usleep(garbage_sleep);
		ack_heartbeat(SOCKET_THREAD_NUM);
	}
	pthread_exit(NULL);
}

/**
 * @brief 
 * This function creates socket thread and initializes its respective monitor structure
 */
void socket_entry(void)
{
	exit_flag[SOCKET_THREAD_NUM] = 0;
	
	rc_socket = pthread_create(&socket_th, (void *)0, socket_func, (void *)0);
	if(rc_socket != 0)
	{
		handle_error_exit("Error in creating socket sensor thread");
	}
	else
	{
		rc_socket = pthread_cond_init(&mon[SOCKET_THREAD_NUM].cond, NULL); 
		if(rc_socket!=0)
			handle_error_exit("pthread cond init");
			
		rc_socket = pthread_mutex_init(&mon[SOCKET_THREAD_NUM].lock, NULL); 
		if(rc_socket!=0)
			handle_error_exit("pthread mutex init");
	}
}

/**
 * @brief 
 * This function safely exits from socket thread.
 */
void socket_exit(void)
{
	if(!exit_flag[SOCKET_THREAD_NUM])
	{
		rc_socket = pthread_cancel(socket_th);
		if(rc_socket != 0)
			handle_error("Error cancelling socket thread");

		rc_socket = mq_close(soc_queue_fd);
		if(rc_socket == -1)
		{
			printf("%d", errno);
			handle_error("Error in closing socket thread mqueue");
		}
		
		rc_socket = mq_unlink(socket_queue);
		if(rc_socket == -1)
			handle_error("Error in unlinking socket thread mqueue");

		rc_socket = close(newserver_fd);
		if(rc_socket != 0)
			handle_error("Error closing socket file descriptor");
		
		rc_socket = close(server_fd);
		if(rc_socket != 0)
			handle_error("Error closing socket file descriptor");

		printf("\nExiting socket thread");
		exit_flag[SOCKET_THREAD_NUM] = 2;
	}
	else if(exit_flag[SOCKET_THREAD_NUM] == 1)
	{
		rc_socket = mq_close(soc_queue_fd);
		if(rc_socket == -1)
		{
			printf("%d", errno);
			handle_error("Error in closing socket thread mqueue");
		}
		
		rc_socket = mq_unlink(socket_queue);
		if(rc_socket == -1)
			handle_error("Error in unlinking socket thread mqueue");

		rc_socket = close(newserver_fd);
		if(rc_socket != 0)
			handle_error("Error closing socket file descriptor");
			
		rc_socket = close(server_fd);
		if(rc_socket != 0)
			handle_error("Error closing socket file descriptor");

		printf("\nExiting socket thread");
		exit_flag[SOCKET_THREAD_NUM] = 2;
		pthread_exit(NULL);
	}
}

/**
 * @brief 
 * This function initializes message queue used for communication between socket and sensor threads
 */
void soc_queue_init()
{
	soc_queue_attr.mq_flags = 0;
	soc_queue_attr.mq_maxmsg = 1;
	soc_queue_attr.mq_msgsize = sizeof(struct command);
	soc_queue_attr.mq_curmsgs = 0;

	soc_queue_fd = mq_open(socket_queue, O_RDWR | O_CREAT | O_EXCL, 0664, &soc_queue_attr);
	if(soc_queue_fd == -1)
		handle_error("Error opening socket queue");
}
