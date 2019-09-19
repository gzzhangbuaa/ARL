#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char test_str[] = "00:12:00:00";
	char *ptr, *retptr;
	int i = 0;
	ptr = test_str;
	int count = 0;
	
	int time[5];
	time[0] = count;
	while((retptr = strtok(ptr, ":")) != NULL)
	{
		printf("substr[%d]:%s\n", i++, retptr);
		ptr = NULL;

		//check isdigit
		int isdigit = strspn(retptr, "0123456789")==strlen(retptr);
		if(isdigit == 1)
		{
			printf("This is a digit!\n");
		}
		else
		{
			printf("This is not a digit!\n");
		}
		int num = atoi(retptr);
		printf("the %d num is %d\n", i, num);
		count = i;
		time[0] = count;
		if(count > 4)
		{
			printf("limit-time format set wrong!\n");
		}
		time[i] = num;
	}

	if(count >=1 && (time[count] < 0 || time[count] > 60)) 
	{
		printf("[DEBUG_INFO]:limit-time set in xml file error, seconds value should between 0 and 60! \n");
	}
	if(count >= 2 && (time[count - 1] < 0 || time[count -1] > 60))	
	{
		printf("[DEBUG_INFO]:limit-time set in xml file error, minutes value should between 0 and 60! \n");
	}
	if(count >= 3 && (time[count - 2] < 0 || time[count -2] > 24))
	{
		printf("[DEBUG_INFO]:limit-time set in xml file error, hours value should between 0 and 24! \n");
	}
	if(count >= 4 && (time[count - 3] < 0))
	{
		printf("[DEBUG_INFO]:limit-time set in xml file error, days value should be an integer! \n");
	}

	return 0;
}
