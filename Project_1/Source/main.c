#include "../Include/main.h"



int main(int argc, char *argv[])
{
	if(argv[1]=="Debug")
	{
		log_verbosity = LOG_DEBUG;
	}
	else
	{
		log_verbosity = LOG_NONE;
	}
	
	return 0;
}
