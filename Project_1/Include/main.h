#include <unistd.h>
#include <fcntl.h>           
#include <sys/stat.h>        
#include <string.h>
#include <sys/syscall.h>
#include <signal.h>
#include <time.h>

#include "temp.h"
#include "light.h"
#include "socket.h"

struct log_msg main_data;

mqd_t main_queue_fd;
struct mq_attr main_queue_attr;

int rc;
useconds_t usec;
struct log_param log_file;

void main_exit(void);
int arg_init(char *arg1, char *arg2);
void thread_create(void);
void thread_join(void);
void set_signal_handler();
void signal_handler(int signo, siginfo_t *info, void *extra);
void heartbeat_check(void);
