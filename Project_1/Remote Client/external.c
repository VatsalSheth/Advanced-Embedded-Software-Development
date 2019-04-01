/***********************************************************************************
* @external.c
* @This file creates user interface for remote client application. It connects to server 
* and request light and temperature sensor data and can also issue kill signals for threads 
* running on server.
* 
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#include "external.h"

/**
 * @brief 
 * This file creates user interface for remote client application. It connects to server 
 * and request light and temperature sensor data and can also issue kill signals for threads 
 * running on server.	
 * @param argc
 * @param argv[]
 *
 * @return 
 */
int main(int argc, char* argv[])
{
	sleepy = 1;
		
	option = (char*)malloc(30);
	if(option == NULL)
	{
		printf("Malloc failed ...!!!\n");
		exit(0);
	}
	
	serv_host = (char*)malloc(20);
	if(serv_host == NULL)
	{
		printf("Malloc failed ...!!!\n");
		exit(0);
	}
	
	while(1)
	{
		printf("\nWhat would you like to do? <request> <kill> <close> <exit>\n");
		scanf("%s", option);
		option = strlwr(option);
		
		if(!strcmp(option, "kill"))
		{
			conn_attempt_flag = 1;
			printf("Which thread would you like to kill? <Temperature> <Light> <Logger> <Socket>\n");
			scanf("%s", option);
			option = strlwr(option);
			
			if(!strcmp(option, "temperature"))
				user.action = KILL_TEMPERATURE;
			else if(!strcmp(option, "light"))
				user.action = KILL_LIGHT;
			else if(!strcmp(option, "logger"))
				user.action = KILL_LOGGER;
			else if(!strcmp(option, "socket"))
				user.action = KILL_SOCKET;
			else 
			{
				printf("\nInvalid option. Enter Again...!!!\n");
				continue;
			}
		}
		else if(!strcmp(option, "request"))
		{
			conn_attempt_flag = 1;
			res_flag = 1;
			printf("Which sensor's data would you like? <Temperature> <Light>\n");
			scanf("%s", option);
			option = strlwr(option);
			
			if(!strcmp(option, "temperature"))
			{
				printf("What unit would you like? Celsius <C> Fahrenheit <F> Kelvini <K>\n");
				scanf("%s", option);
				option = strlwr(option);
				
				if(!strcmp(option, "c"))
					user.action = REQUEST_TEMPERATURE_C;
				else if(!strcmp(option, "f"))
					user.action = REQUEST_TEMPERATURE_F;
				else if(!strcmp(option, "k"))
					user.action = REQUEST_TEMPERATURE_K;
				else
				{
					printf("\nInvalid option. Enter Again...!!!\n");
					continue;
				}
			}
			else if(!strcmp(option, "light"))
				user.action = REQUEST_LIGHT;
			else 
			{
				printf("\nInvalid option. Enter Again...!!!\n");
				res_flag = 0;
				continue;
			}
		}
		else if(!strcmp(option, "close"))
		{
			if(conn_flag == 1)
			{
				close(client_fd);
				printf("Closing Socket...!!!\n");
				conn_flag = 0;
			}
			else
			{
				printf("Application NOT connected...!!!\n");
			}
			continue;
		}
		else if(!strcmp(option, "exit"))
		{
			printf("Exiting application...!!!\n");
			if(conn_flag == 1)
			{
				close(client_fd);
				printf("Closing Socket...!!!\n");
				conn_flag = 0;
			}
			free(option);
			free(serv_host);
			exit(0);
		}
		else
		{
			printf("\nInvalid option. Enter Again...!!!\n");
			continue;
		}

		if((!conn_flag) && (conn_attempt_flag == 1))
		{
			conn_attempt_flag = 0;
			printf("Enter IP Address:\n");
			scanf("%s", option);
			option = strlwr(option);
			
			strcpy(serv_host, option);
			port = SERV_TCP_PORT;
			
			host_ptr = gethostbyname(serv_host);
			if(host_ptr == NULL) 
			{
				printf("gethostbyname error\n");
				continue;
			}
			  
			if(host_ptr->h_addrtype !=  AF_INET) 
			{
				printf("unknown address type\n");
				continue;
			}

			bzero((char *) &serv_addr, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_addr.s_addr = ((struct in_addr *)host_ptr->h_addr_list[0])->s_addr;
			serv_addr.sin_port = htons(port);

			client_fd = socket(AF_INET, SOCK_STREAM, 0);
			if(client_fd < 0)
			{
				printf("Error opening socket\n");
				continue;
			}

			rc = connect(client_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
			if(rc < 0) 	
			{
				printf("Error connecting to server\n");
				continue;
			}
			
			conn_flag = 1;
			printf("\nRemote connection with Environment Monitoring application established\n");
		}
		rc = write(client_fd, (void*)&user, sizeof(struct command));		
		if(rc == -1)
		{
			printf("Failed to send request...!!!\n");
			continue;
		}
		
		if(res_flag)
		{
			res_flag = 0;
			rc = read(client_fd, (char*)&user, sizeof(struct command));
			if(rc < 0)
			{
				printf("Receiving Read failed\n");
				continue;
			}
			else if(rc > 0)
			{
				if(user.action == REQUEST_FAIL)
				{
					printf("Response timed out...!!!\n");
					continue;
				}
				else
				{
					switch(user.action)
					{
						case STATUS_LIGHT:	printf("Ambience is light with %f lux\n",user.sensor_data);
											break;
						case STATUS_DARK:	printf("Ambience is dark with %f lux\n",user.sensor_data);
											break;
						case REQUEST_TEMPERATURE_C:	printf("Temperature is %f C\n",user.sensor_data);
											break;
						case REQUEST_TEMPERATURE_K:	printf("Temperature is %f K\n",user.sensor_data);
											break;
						case REQUEST_TEMPERATURE_F:	printf("Temperature is %f F\n",user.sensor_data);
											break;
						default:			break;
					}
				}
			}
		}
	}
}

/**
 * @brief 
 * This funtion converts string to lower case
 * @param ip
 *
 * @return 
 */
char *strlwr(char *ip)
{
	for(__uint32_t i=0; ip[i]!='\0'; i++)
	{
		if(ip[i]>='A' && ip[i]<='Z')
			ip[i] = ip[i] + 32;
	}
	return ip;
}
