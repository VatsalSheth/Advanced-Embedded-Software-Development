#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>

struct thread_param
{
	char *f_name;
};

char *ip_file = "gdb.txt";
pthread_t th_1, th_2;
timer_t timer_id;
int timer_flag, signal_flag, signal_num;
struct timespec start = {0,0};

void *thread_1(void *);
void *thread_2(void *);
void timer_handle(union sigval sv);
void time_diff(struct timespec *time2, struct timespec *time1, struct timespec *time_res);
void set_signal_handler(void);
void siganl_mask(void);

int main(int argc, char *argv[])
{
	struct thread_param parm;
	FILE *fp;
	struct timespec final = {0,0};
	struct timespec diff = {0,0};
	
	signal_flag = 0;
	set_signal_handler();
	
	clock_gettime(CLOCK_REALTIME, &start);
	clock_gettime(CLOCK_REALTIME, &final);
	time_diff(&final, &start, &diff);
	
	if(argv[1]==NULL)
	{
		printf("No file input given...!!!\n");
		return 0;
	}
	
	remove(argv[1]);
	
	parm.f_name = argv[1];
	
	pthread_create(&th_1, (void *)0, thread_1, (void *)&(parm));
	pthread_create(&th_2, (void *)0, thread_2, (void *)&(parm));
	
	fp = fopen(argv[1], "a");
	if(fp == NULL)
	{
		printf("Error in opening file %s from main thread\n",argv[1]);
	}
	fprintf(fp, "\nMain Thread at %ld sec, %ld msec (%ld microsec) (%ld nanosec):\nPOSIX thread ID: %ld\nLinux thread ID %ld\n\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec, pthread_self(), syscall(SYS_gettid));
	
	pthread_join(th_1, NULL);
	clock_gettime(CLOCK_REALTIME, &final);
	time_diff(&final, &start, &diff);
	fprintf(fp, "Completed Work at %ld sec, %ld msec (%ld microsec) (%ld nanosec)\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec);
	fclose(fp);
	
	pthread_join(th_2, NULL);
	
	timer_delete(timer_id);
	
	return 0;
}

void *thread_1(void *arg)
{
	FILE *f_ptr1, *f_ptr2;
	struct thread_param *p;
	int c;
	__uint32_t cnt[26] = {0};
	struct timespec final = {0,0};
	struct timespec diff = {0,0};
	
	p = (struct thread_param *)arg;
	
	clock_gettime(CLOCK_REALTIME, &final);
	time_diff(&final, &start, &diff);
	
	siganl_mask();
	
	f_ptr1 = fopen(ip_file, "r");
	if(f_ptr1 == NULL)
	{
		printf("Error in opening file %s from thread 1\n",p->f_name);
		pthread_exit(NULL);
	}
	
	do
	{
		c = fgetc(f_ptr1);
		if(c>='a' && c<='z')
		{
			cnt[c-'a']++;
		}
		else if(c>='A' && c<='Z')
		{
			cnt[c-'A']++;
		}
		
	}while(!feof(f_ptr1));
	fclose(f_ptr1);
	
	f_ptr2 = fopen(p->f_name, "a");
	if(f_ptr2 == NULL)
	{
		printf("Error in opening file %s from thread 1\n",p->f_name);
		pthread_exit(NULL);
	}
	
	fprintf(f_ptr2, "Thread 1 created at %ld sec, %ld msec (%ld microsec) (%ld nanosec):\nThread ID %ld\n\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec,pthread_self());
	for(c=0; c<26; c++)
	{
		if(cnt[c] < 100)
		{
			fprintf(f_ptr2, "%c count is %d\n",c+'a', cnt[c]);
		}
	}
	
	fclose(f_ptr2);
	
	pthread_exit(NULL);
}

void *thread_2(void *arg)
{
	struct sigevent sev;
	struct itimerspec trigger;
	struct thread_param *p;
	FILE *f_ptr1, *f_ptr2;
	char ch;
	struct timespec final = {0,0};
	struct timespec diff = {0,0};
	
	p = (struct thread_param *)arg;
	
	clock_gettime(CLOCK_REALTIME, &final);
	time_diff(&final, &start, &diff);
	
	timer_flag = 0;
	
	f_ptr1 = fopen(p->f_name, "a");
	
	if(f_ptr1 == NULL)
	{
		printf("Error in opening file %s from thread 2\n",p->f_name);
		pthread_exit(NULL);
	}
	
	fprintf(f_ptr1, "\nThread 2 created at %ld sec, %ld msec (%ld microsec) (%ld nanosec):\nThread ID %ld\n\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec,pthread_self());
	fclose(f_ptr1);
	
	memset(&sev, 0, sizeof(struct sigevent));
    memset(&trigger, 0, sizeof(struct itimerspec));
    
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = timer_handle;
    

	timer_create(CLOCK_REALTIME, &sev, &timer_id);
	
	trigger.it_interval.tv_nsec = 100000000;
	trigger.it_value.tv_nsec = 100000000;
		
	timer_settime(timer_id, 0, &trigger, NULL);
	while(1)
	{
		if(signal_flag == 1)
		{
			signal_flag = 0;
			clock_gettime(CLOCK_REALTIME, &final);
			time_diff(&final, &start, &diff);
			
			f_ptr1 = fopen(p->f_name, "a");
			if(f_ptr1 == NULL)
			{
				printf("Error in opening file %s from thread 2\n",p->f_name);
				pthread_exit(NULL);
			}
			
			if(signal_num == 10)
				signal_num = 1;
			else if(signal_num == 12)
				signal_num = 2;
				
			fprintf(f_ptr1, "\nSignal USR%d received and exiting at %ld sec, %ld msec (%ld microsec) (%ld nanosec)\n",signal_num, diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec);
			pthread_cancel(pthread_self());
		}
		
		if(timer_flag == 1)
		{
			timer_flag = 0;
			
			clock_gettime(CLOCK_REALTIME, &final);
			time_diff(&final, &start, &diff);
			
			f_ptr2 = popen("cat /proc/stat | grep cpu", "r");
			if(f_ptr2 == NULL)
			{
				printf("Error in getting CPU utilization\n");
				pthread_exit(NULL);
			}
			
			f_ptr1 = fopen(p->f_name, "a");
			if(f_ptr1 == NULL)
			{
				printf("Error in opening file %s from thread 2\n",p->f_name);
				pthread_exit(NULL);
			}
			fprintf(f_ptr1, "\nThread 2 at %ld sec, %ld msec (%ld microsec) (%ld nanosec):\nThread ID %ld\n\n",diff.tv_sec, (diff.tv_nsec / 1000000), (diff.tv_nsec / 1000), diff.tv_nsec, pthread_self());
			
			while(!feof(f_ptr2))
			{
				ch = fgetc(f_ptr2);
				fputc(ch, f_ptr1);
			}	
			pclose(f_ptr2);
			fclose(f_ptr1);
		}
	}
	pthread_exit(NULL);
}

void timer_handle(union sigval sv)
{
	timer_flag = 1;
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

void signal_handler(int signo, siginfo_t *info, void *extra) 
{
	signal_flag = 1;
	signal_num = signo;
}

void set_signal_handler(void)
{
        struct sigaction action;
 
 
        action.sa_flags = SA_SIGINFO; 
        action.sa_sigaction = signal_handler;
 
        if (sigaction(SIGUSR1, &action, NULL) == -1) { 
            perror("sigusr1: sigaction");
            _exit(1);
        }
	
		if (sigaction(SIGUSR2, &action, NULL) == -1) { 
            perror("sigusr2: sigaction");
            _exit(1);
        }
}

void siganl_mask(void)
{
	sigset_t mask;
	sigemptyset(&mask); 
    sigaddset(&mask, SIGRTMIN+3); 
                
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
        
}
