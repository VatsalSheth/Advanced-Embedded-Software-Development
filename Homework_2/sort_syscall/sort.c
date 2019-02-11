#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

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

void main()
{
	long err;	
	__int32_t *ip, *op, len, i;
	struct timespec start = {0,0};
	struct timespec final = {0,0};
	struct timespec diff = {0,0};
	
	srand(time(0));
	len = (rand() % 256)+256;
	
	ip = malloc(len);
	op = malloc(len);
	if(ip == NULL || op == NULL)
	{
		printf("Malloc failed...!!!\n");
		exit(0);
	}
	
	printf("Buffer length = %d\n", len);
	for(i=0; i<len; i++)
	{
			ip[i] = rand();
			printf("Userspace input %ld = %ld\n", i, (ip[i]));
	}
	
	clock_gettime(CLOCK_REALTIME, &start);
	
	err = syscall(398, ip, len, op);
	
	clock_gettime(CLOCK_REALTIME, &final);
	time_diff(&final, &start, &diff);
	
	if(err)
	{
		printf("Custom syscall returned with error number %ld\n",err);
	}
	
	printf("Sort Execution Time = %ld sec, %ld msec (%ld microsec) (%ld nanosec)\n", diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec);

	
	for(i=0; i<len; i++)
	{
		printf("Userspace output %ld = %d\n", i, (op[i]));
	}
	
	free(ip);
	free(op);
}
