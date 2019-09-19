#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "interface.h"

extern int errno;

#define SHELLFOLDER	"./shellfiles"
#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)

int mk_dir(char *dir)
{
	DIR *mydir = NULL;
	if((mydir = opendir(dir)) == NULL)
	{
		int ret = mkdir(dir, MODE);
		if(ret != 0)
		{
			return -1;
		}
		printf("%s created success!\n", dir);
	}
	else
	{
		printf("%s exist!\n", dir);
	}
	return 0;
}

int mk_all_dir(char *dir)
{
	int flag = 1;
	char *pDir = dir;
	while(flag)
	{
		char *pIndex = index(pDir, '/');
		if(pIndex != NULL && pIndex != dir)
		{
			char buffer[512] = {0};
			int msg_size = pIndex -dir;
			memcpy(buffer, dir, msg_size);
			int ret = mk_dir(buffer);
			if(ret < 0)
			{
				printf("%s created failed!\n", dir);
			}
		}
		else if(pIndex == NULL && pDir == dir)
		{
			printf("dir is not directory!\n");
			return -1;
		}
		else if(pIndex == NULL && pDir != dir)
		{
			int ret = mk_dir(dir);
			if(ret < 0)
			{
				printf("%s created failed!\n", dir);
			}
			break;
		}
		pDir = pIndex+1;
	}
	return 0;
}

int mkdirforshell(char *shellfolder)
{
	char *pIndex = index(shellfolder, '\n');
	if(pIndex != NULL)
	{
		*pIndex = '\0';
	}
	printf("check path name %s\n", shellfolder);
	int ret = mk_all_dir(shellfolder);
	if(ret < 0)
	{
		printf("% mkdir failed!\n", shellfolder);
		return -1;
	}
	return 0;
}
















