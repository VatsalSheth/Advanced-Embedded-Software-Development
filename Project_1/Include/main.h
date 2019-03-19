#include <unistd.h>
#include <fcntl.h>           
#include <sys/stat.h>        
#include <string.h>
#include <sys/syscall.h>
#include <signal.h>

#include "temp.h"

pthread_t light_th, socket_th;
int rc;
struct log_param log_file;

int arg_init(char *arg1, char *arg2);
void thread_create(void);
void thread_join(void);
void set_signal_handler();
void signal_handler(int signo, siginfo_t *info, void *extra);
