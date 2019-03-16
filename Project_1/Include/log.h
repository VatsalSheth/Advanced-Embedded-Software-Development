#include <stdint.h>
#include <time.h>

#define LOG_NONE (0)
#define LOG_DEBUG (1)

#define handle_error(msg) \
			{\ 
				perror(msg); \
				exit(1);\ 
			}

struct log_msg
{
	uint32_t id;
	float data;
	struct timespec time_stamp;
	uint32_t verbosity;
};
