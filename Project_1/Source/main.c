#include "../Include/main.h"



int main(int argc, char *argv[])
{
	if(!arg_init(argv[1], argv[2]))
	{
		return 0;
	}
	
	return 0;
}

int arg_init(char *arg1, char *arg2)
{
	if(arg1 == NULL)
	{
		printf("No file input given...!!!\n");
		return 0;
	}
	else if(arg1 == "Debug")
	{
		printf("File name cannot be debug...!!!\n");
		return 0;
	}
	
	log_file = arg1;
	
	if(arg2=="Debug")
	{
		log_verbosity = LOG_DEBUG;
	}
	else
	{
		log_verbosity = LOG_NONE;
	}
	
	return 1;
}
