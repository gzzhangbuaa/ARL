#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#include "share.h"
#define MAXBUFF 1024
#define NEWLINE 1
#define NOTNEWLINE 0

extern int finishflag;

int getRowNumsFromString(char * rstStr)
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
        printf("[DEBUG_INFO]:row is %d, colum is %d\n", rows, colums);
	return rows;
}



int getRowNumsFromFile(char * file)
{
        struct stat fileInfo;
        FILE *filePointer;
        char *fileData;

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
        printf("[DEBUG_INFO]:\n%s\n", fileData);
	int rownums = getRowNumsFromString(fileData);	

        free(fileData);
        fileData = NULL;
        return rownums;
}

int readFileByLine(char *file)
{
        FILE *fp;
        char load_file[256];
        char *line = NULL;      //denote the current line going to read
        char *preRead = NULL;   //denote the previous lines have been read if one real line read can't be completed once
        int curLineFlag = NEWLINE;      //this is a flag to denote the current line is a real line or just reach the buffer size
        int preLineFlag = NEWLINE;      //this is a flag to denote the previous line is a newline or not.
        int lineNum = 0;        //denote the number of the real lines with the enter key
        int readCounter = 0;    //read times for one real line

	/**	the following variables record the position of the corresponding attribute	**/
	int pos_launchID = -1;
	int pos_state = -1;
	int pos_nodesnum = -1;
	
	char delim[10] = " \t\n";

        strcpy(load_file, file);
        fp = fopen(load_file, "r");
        if(fp == NULL)
        {
                printf("[DEBUG_INFO]:can not load file!\n");
                return -1;
        }
        line = malloc(sizeof(char)*MAXBUFF);
        while(!feof(fp))
        {
                readCounter++;
                preLineFlag = curLineFlag;
                memset(line, 0, MAXBUFF);
                fgets(line, MAXBUFF, fp);
		printf("[DEBUG_INFO]:fgets occurred! %s\n", line);
        }
        fclose(fp);
        return 0;
}




void main()
{

	int rownums = getRowNumsFromFile(QUERY_LOG);
	/**	here aubtract 1 because the head line in the file is attribute	**/
	int entrynums = rownums -1;	
	printf("[DEBUG_INFO]:entrynums is %d\n", entrynums);


	readFileByLine(QUERY_LOG);
}


























