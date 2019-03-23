#include "../Include/light.h"

#define Thread_num 1

void* light_func(void* threadp)
{
	//sleep(1);
	while(1)
	{
		ack_heartbeat(1);
	}
}

