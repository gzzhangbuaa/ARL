#include <stdio.h>
#include <string.h>

int getOwnShell(char *originshell)
{
	if(access(originshell, F_OK) == -1)
	{
		printf("[DEBUG_INFO]:%s doesn't exist!\n", originshell);
		return -1;
	}
	if(access(originshell, R_OK|W_OK) == -1)
	{
		printf("[DEBUG_INFO]:%s can't read or write!\n", originshell);
		return -1;
	}
	
}

int main()
{
}
