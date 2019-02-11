#include <stdio.h>
#include <stdlib.h>

void main()
{
	FILE *fp;
	char *str, c;
	
	fp = fopen("test","w+");
	printf("Hey...!!!\n");
	fputc('A',fp);
	fclose(fp);
	fp = fopen("test","a");
	str = (char *)malloc(20);
	if(str != NULL)
	{
		fgets(str, 20, stdin);
		fputs(str, fp);
		fflush(fp);
		fclose(fp);
		fp = fopen("test","r");
		c = fgetc(fp);
		fgets(str, 20, fp);
		fclose(fp);
		free(str);
	}
	else 
	{
		printf("Malloc failed...!!!\n");
	}	
}
