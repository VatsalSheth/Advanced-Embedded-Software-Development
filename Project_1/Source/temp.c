#include "../Include/temp.h"

void* temp_func(void* threadp)
{
	sleep(1);
	
	srand(time(NULL));
}

void temp_exit()
{

}
