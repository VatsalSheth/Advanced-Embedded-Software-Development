#include <stdint.h>
#include <time.h>

#define LOG_NONE (0)
#define LOG_DEBUG (1)
 
char *log_file;
uint32_t log_verbosity;

struct log_mssg
{
	uint32_t id;
	float light;
	float temp;
	struct timespec time_stamp;
	uint32_t verbosity;
};
