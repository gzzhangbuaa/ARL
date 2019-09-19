#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>


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
		printf("[DEBUG_INFO]:%s created success!\n", dir);
	}
	else
	{
		printf("[DEBUG_INFO]:%s exist!\n", dir);
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
				printf("[DEBUG_INFO]:%s created failed!\n", dir);
			}
		}
		else if(pIndex == NULL && pDir == dir)
		{
			printf("[DEBUG_INFO]:dir is not directory!\n");
			return -1;
		}
		else if(pIndex == NULL && pDir != dir)
		{
			int ret = mk_dir(dir);
			if(ret < 0)
			{
				printf("[DEBUG_INFO]:%s created failed!\n", dir);
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
	printf("[DEBUG_INFO]:check path name %s\n", shellfolder);
	int ret = mk_all_dir(shellfolder);
	if(ret < 0)
	{
		printf("[DEBUG_INFO]:% mkdir failed!\n", shellfolder);
		return -1;
	}
	return 0;
}



int getOwnShell(char *originshell, int id, char *finalshell)
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
	int rst = mkdirforshell(SHELLFOLDER);
	if(rst != 0 || originshell == NULL)
	{
		return -1;
	}

	char newfilepath[128];
	char *filename = strrchr(originshell, '/');
	filename = filename+1;
	printf("[DEBUG_INFO]:original shell file name is %s\n", filename);
	char *suffixname = strstr(filename, ".sh");
	int strsize = suffixname - filename;
	char newfilename[64] = {0};
	strncpy(newfilename, filename, strsize);	
	char jobID[32]={0};
	sprintf(jobID, "%d", id);
	strcat(newfilename, "_");
	strcat(newfilename, jobID);
	strcat(newfilename, ".sh");
	printf("[DEBUG_INFO]: This job's own shell file name is :\%s\n", newfilename);


	strcpy(newfilepath, SHELLFOLDER);	
	strcat(newfilepath, "/");
	strcat(newfilepath, newfilename);
	printf("[DEBUG_INFO]:This job's own shell path is %s\n", newfilepath);
	char command[256];
	strcpy(command, "cp ");
	strcat(command, originshell);
	strcat(command, " ");
	strcat(command, newfilepath);
	printf("[DEBUG_INFO]:cp command is: %s\n", command);
	rst = system(command);
	printf("[DEBUG_INFO]: copy shell file return value is %d\n", rst);

	if(access(newfilepath, F_OK|R_OK|W_OK) == -1)
	{
		printf("[DEBUG_INFO]:the new shell file for job %d occurred errors!\n", id);
		return -1;
	}
	FILE* fp = fopen(newfilepath, "rb+");
	if(!fp)
	{
		printf("[DEBUG_INFO]:can't open file!\n");
		return -1;
	}
	int size = sizeof(char);
	fseek(fp, -size, SEEK_END);
	char last;
	fread(&last, size, 1, fp);
	printf("[DEBUG_INFO]: last char in shell file is %c\n", last);
	char parameter[32];
	if(last != '\n')
	{
		strcpy(parameter, " -ID ");
		strcat(parameter, jobID);
		fwrite(parameter, strlen(parameter), 1, fp);
	}
	else
	{
		strcpy(parameter, " -ID ");
		strcat(parameter, jobID);
		strcat(parameter, "\n");
		fseek(fp, -size, SEEK_END);
		fwrite(parameter, strlen(parameter), 1, fp);
	}
	fclose(fp);
	
	strcpy(finalshell, newfilepath);
	return 0;	
}


void main()
{
	//mkdirforshell(SHELLFOLDER);
	char finalshell[64] = {0};
	getOwnShell("./hellolaunch.sh", 10, finalshell);
	printf("[DEBUG_INFO]:finalshell is %s\n", finalshell);
}
















