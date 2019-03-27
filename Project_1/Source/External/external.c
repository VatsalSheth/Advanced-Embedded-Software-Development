#include "external.h"

int main(int argc, char* argv[])
{
	sleepy = 1;
	char* songs[10] = {"We Don't talk anymore", "Wolves",
			 "It Ain't Me", "Gone gone gone", 
			 "Castle on the hill", "Shape of you",
			 "Hall of Fame", "Counting Stars",
			 "Girls Like You", "Something Just Like this"};
	struct command user;
	printf("\nWhat would you like to do? <request> <kill> ");
	option = malloc(30);	
	scanf("%s", option);
	if(!strcmp(option, "kill"))
	{
		printf("Which thread would you like to kill? <Temperature> <Light> <Logger> <Socket> ");
		scanf("%s", option);
		if(!strcmp(option, "Temperature"))
			user.action = KILL_TEMPERATURE;
		else if(!strcmp(option, "Light"))
			user.action = KILL_LIGHT;
		else if(!strcmp(option, "Logger"))
			user.action = KILL_LOGGER;
		else if(!strcmp(option, "Socket"))
			user.action = KILL_SOCKET;
		else 
		{
			printf("\nInvalid option\n");
			printf("Exiting now\n");
			exit(1);
		}
	}
	else if(!strcmp(option, "request"))
	{
		printf("Which sensor's data would you like? <Temperature> <Light> ");
		scanf("%s", option);
		if(!strcmp(option, "Temperature"))
			user.action = REQUEST_TEMPERATURE;
		else if(!strcmp(option, "Light"))
			user.action = REQUEST_LIGHT;
		else 
		{
			printf("\nInvalid option\n");
			printf("Exiting now\n");
			exit(1);
		}
	}
	else 
	{
		printf("\nInvalid option\n");
		printf("Exiting now\n");
		exit(1);
	}

	printf("Connect with application now? <Y> or <N> ");
	scanf("%s", option);
	if(!strcmp(option, "Y"))
	{
		char *serv_host = "localhost";
		port = SERV_TCP_PORT;

		if((host_ptr = gethostbyname(serv_host)) == NULL) 
		{
			perror("gethostbyname error");
			exit(1);
		}
	  
	  	if(host_ptr->h_addrtype !=  AF_INET) 
		{
			perror("unknown address type");
			exit(1);
		}

		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = ((struct in_addr *)host_ptr->h_addr_list[0])->s_addr;
		serv_addr.sin_port = htons(port);

		if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			perror("Error opening socket");
			exit(1);
		}

		if(connect(client_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 	
		{
			perror("Error connecting to server");
			exit(1);
		}

		printf("\nRemote connection with Environment Monitoring application established\n");

/*		for(int i=0; i<10; i++)
		{
			printf("Sending: <%s>", songs[i]);
			write(client_fd, songs[i], strlen(songs[i]));
			usleep(sleepy);
		}	
*/
		write(client_fd, (void*)&user, sizeof(struct command));		
//		close(client_fd);
	}
	else if(!strcmp(option, "N"))
	{
		printf("\nExiting now\n");
	}
	else printf("\nInvalid option\n");
}

