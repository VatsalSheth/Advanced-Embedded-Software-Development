#include <pthread.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>           
#include <sys/stat.h>        
#include <string.h>
#include <time.h>

#define queue_name "/ipc_queue"

#define handle_error(msg) \
			{ perror(msg); \
			exit(EXIT_FAILURE); }
                      
struct data
{
	char str[32];
	int led;
};

char *log_file = "log.txt";
struct timespec start = {0,0};

int main()
{
	mqd_t q_id;
	struct mq_attr q_attr;
	struct data send[10], receive[10];
	char tmp[32];
	int rc, i, rnd;
	
	srand(time(0));
	
	q_attr.mq_flags = 0;
    q_attr.mq_maxmsg = 10;
    q_attr.mq_msgsize = 50;
    q_attr.mq_curmsgs = 0;
	
	q_id = mq_open(queue_name, O_RDWR | O_CREAT, 0664, &q_attr);
	if(q_id == -1)
		handle_error("mq_open");
	
	for(i=0; i<10; i++)
	{	
		rc = mq_receive (q_id, (char*)&receive[i], 100, NULL);
		if(rc == -1)
			handle_error("mq_receive");
		
		printf("%s %d\n",receive[i].str, receive[i].led);
		
		rnd = rand();
		sprintf(tmp, "String with random number %d",rnd%100);
		strcpy(send[i].str, tmp);
		send[i].led = rnd%2;
		rc = mq_send(q_id, (const char *)&send[i], sizeof(struct data), 0); 
		if(rc == -1)
			handle_error("mq_send");
	}
	
	mq_close(q_id);
	if(rc == -1)
		handle_error("mq_close");
		
	return 0;
}

void time_diff(struct timespec *time2, struct timespec *time1, struct timespec *time_res)
{
  int dt_sec=time2->tv_sec - time1->tv_sec;
  int dt_nsec=time2->tv_nsec - time1->tv_nsec;

  if(dt_sec >= 0)
  {
    if(dt_nsec >= 0)
    {
      time_res->tv_sec=dt_sec;
      time_res->tv_nsec=dt_nsec;
    }
    else
    {
      time_res->tv_sec=dt_sec-1;
      time_res->tv_nsec=1000000000+dt_nsec;
    }
  }
}
