#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#define MAXBUFF	1024
#define NEWLINE	1
#define NOTNEWLINE 0

int readFile(char *file);
int readFileByLine(char *file);
 
int splitStateResult(char * rstStr)
{
	int i, len, colums, rows;
	len = strlen(rstStr);
	colums = 0;
	rows = 0;
	for(i = 0; i < len; i++)
	{
		if(rstStr[i] == '\n')
		{
			rows++;
			colums = 0;
		}
		if(rstStr[i] == ' ' || rstStr[i] == '\t')
		{
			colums++;
		}
	}
	printf("row is %d, colum is %d\n", rows, colums);
}

void main()
{
	system("sinfo |& tee yhi.log");
	//readFile("yhi.log");
	readFileByLine("yhi.log");
}


int readFile(char * file)
{
	struct stat fileInfo;
	FILE *filePointer;
	char *fileData;

	//int max-size = 1024;
	if(file == NULL)
	{
		return 0;
	}
	if(!(filePointer = fopen(file, "rb")))
	{
		return 0;
	}
	stat(file, &fileInfo);
	fileData = malloc(sizeof(char)*(fileInfo.st_size + 1));
	fread(fileData, sizeof(char), fileInfo.st_size, filePointer);
	fclose(filePointer);
	fileData[fileInfo.st_size] = 0;
	printf("\n%s\n", fileData);
	free(fileData);
	fileData = NULL;
	return 1;
}

int readFileByLine(char *file)
{
	FILE *fp;
	char load_file[256];
	char *line = NULL;	//denote the current line going to read
	char *preRead = NULL;	//denote the previous lines have been read if one real line read can't be completed once
	int curLineFlag = NEWLINE;	//this is a flag to denote the current line is a real line or just reach the buffer size
	int preLineFlag = NEWLINE;	//this is a flag to denote the previous line is a newline or not.
	int lineNum = 0;	//denote the number of the real lines with the enter key
	int readCounter = 0;	//read times for one real line

	strcpy(load_file, "yhi.log");
	fp = fopen(load_file, "r");
	if(fp == NULL)
	{
		printf("can not load file!\n");
		return 1;
	}

	line = malloc(sizeof(char)*MAXBUFF);
	while(!feof(fp))
	{
		readCounter++;
		preLineFlag = curLineFlag;
		memset(line, 0, MAXBUFF);
		fgets(line, MAXBUFF, fp);
		int len = strlen(line);
		if(len == 0)
		{
			continue;
		}

		printf("[@@@@ len = %d maxbuff-2 = %d @@@@@@]\n", len, MAXBUFF-2);
		printf("line-maxbuff-2 = %c\n", line[MAXBUFF-2]);
		if(len == MAXBUFF-1 && line[MAXBUFF-2] != '\n')
		{
			//this line the last elemnt is 0 and the second-to-last is not enter
			//this is not a real line, the actural size of this line is bigger than MAXBUFF
			//scale the buffer and go on read data from file until find enter

			//tell the next line going to read, the current line is not a ture line with enter key
			curLineFlag = NOTNEWLINE;
		}
		if(len <= MAXBUFF-1)
		{
			if(line[len-1] == '\n')
			{
				printf("The last element of this line is the enter key\n");
				printf("parser this line!\n");
				curLineFlag = NEWLINE;
			}
			else
			{
				printf("The last element of this line is not enter\n");
				curLineFlag = NOTNEWLINE;	
			}
		}
		if(len > MAXBUFF - 1)
		{
			printf("This can't happen!\n");
		}

		if(preLineFlag == NEWLINE)
		{	
			if(!preRead)
			{
				free(preRead);
				preRead = NULL;
			}
			if(curLineFlag == NOTNEWLINE)
			{
				preRead = malloc(sizeof(char)*MAXBUFF);
				strcpy(preRead, line);
				printf("[##### put the data in the bag! #####]\n");
			}
			
		}	
	
		if(preLineFlag == NOTNEWLINE)
		{
			
			printf("Combine the buffer\n");
			char *newRead = malloc(sizeof(char)*(readCounter + 1)*MAXBUFF);
			strcpy(newRead, preRead);
			if(!preRead)
			{
				free(preRead);
				preRead = NULL;
			}
			preRead = newRead;
			strcat(preRead, line);
		}

		if(curLineFlag == NEWLINE)
		{
			readCounter = 0;	//reset
			lineNum++;
			if(lineNum == 1)
			{
				printf("This line is the attribute: %s\n", line);
			}
			else
			{
				if(preLineFlag == NEWLINE)
				{
					printf("This line is the content: %s\n", line);
				}
				else
				{
					printf("This line is the content: %s\n", preRead);
				}
			}			
		}
	}
	
	printf("line number is %d \n", lineNum);
	if(!line)
	{
		free(line);
		line = NULL;
	}
	if(!preRead)
	{
		free(preRead);
		preRead = NULL;
	}
	fclose(fp);
	return 0;
}





















