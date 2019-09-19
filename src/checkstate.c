#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#include "interface.h"

#define MAXBUFF 1024
#define NEWLINE 1
#define NOTNEWLINE 0

extern int finishflag;

extern long messagecount;
extern long messagesize;


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

int readFileByLine(char *file, QUERY_dimension *queryresult, int entrynums)
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
	int pos_runtime = -1;
	
	char delim[10] = " \t\n";

        strcpy(load_file, file);
        fp = fopen(load_file, "r");
        if(fp == NULL)
        {
                printf("[DEBUG_INFO]:can not load file!\n");
                return -1;
        }
        line = malloc(sizeof(char)*MAXBUFF);

	messagecount++; //for counting
        while(!feof(fp))
        {
                readCounter++;
                preLineFlag = curLineFlag;
                memset(line, 0, MAXBUFF);
                fgets(line, MAXBUFF, fp);
		printf("[DEBUG_INFO]:fgets occurred!\n");
                int len = strlen(line);

		messagesize = messagesize + len*8;

                if(len == 0)
                {
                        continue;
                }

                printf("[DEBUG_INFO]:[@@@@ len = %d maxbuff-2 = %d @@@@@@]\n", len, MAXBUFF-2);
                printf("[DEBUG_INFO]:line-maxbuff-2 = %c\n", line[MAXBUFF-2]);
		if(len == MAXBUFF-1 && line[MAXBUFF-2] != '\n')
                {
                        curLineFlag = NOTNEWLINE;
                }
                if(len <= MAXBUFF-1)
                {
                        if(line[len-1] == '\n')
                        {
                                printf("[DEBUG_INFO]:The last element of this line is the enter key\n");
                                printf("[DEBUG_INFO]:parser this line!\n");
                                curLineFlag = NEWLINE;
                        }
                        else
                        {
                                printf("[DEBUG_INFO]:The last element of this line is not enter\n");
                                curLineFlag = NOTNEWLINE;
                        }
                }
                if(len > MAXBUFF - 1)
                {
                        printf("[DEBUG_INFO]:This can't happen!\n");
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
                                printf("[DEBUG_INFO]:[##### put the data in the bag! #####]\n");
                        }

                }
                if(preLineFlag == NOTNEWLINE)
                {

                        printf("[DEBUG_INFO]:Combine the buffer\n");
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
                        readCounter = 0;        //reset
                        lineNum++;
                        if(lineNum == 1)
                        {
                                printf("[DEBUG_INFO]:This line is the attribute: %s\n", line);
				char *dimension = NULL;
				int colum = 0;
				for(dimension = strtok(line, delim); dimension != NULL; dimension = strtok(NULL, delim))
				{
					printf("dimension is %s\n", dimension);	
					if(!strcmp(dimension, Dime_launchID))
					{
						pos_launchID = colum;
						printf("[DEBUG_INFO]: %s position %d\n", dimension, colum);
					}
					if(!strcmp(dimension, Dime_state))
					{
						pos_state = colum;
						printf("[DEBUG_INFO]: %s position %d\n", dimension, colum);
					}
					if(!strcmp(dimension, Dime_nodes))
					{
						pos_nodesnum = colum;
						printf("[DEBUG_INFO]: %s position %d\n", dimension, colum);
					}
					if(!strcmp(dimension, Dime_time))
					{
						pos_runtime = colum;
						printf("[DEBUG_INFO]: %s position %d\n", dimension, colum);
					}
					colum++;
				}
                        }
                        else
                        {
				char *currentline = NULL;
                                if(preLineFlag == NEWLINE)
                                {
					currentline = line;
                                        printf("[DEBUG_INFO]:This line is the content: %s\n", currentline);
                                }
                                else
                                {
                                        printf("[DEBUG_INFO]:This line is the content: %s\n", preRead);
					currentline = preRead;
                                }
				
				int colum = 0;
				char *element = NULL;
				for(element = strtok(currentline, delim); element != NULL; element = strtok(NULL, delim))				
				{
					int order = lineNum - 2;  // -1 for headline in the table, and -1 for order not nums
					if(order >= entrynums)
					{
						printf("[DEBUG_INFO]:the num of records shouldn't be larger than entrynums!\n");
						return -1;
					}					

					if(colum == pos_launchID)
					{
						queryresult[order].launchID = atol(element);
						printf("[DEBUG_INFO]: record %d in xml launchID is %ld\n", order, queryresult[order].launchID);
					}
					if(colum == pos_state)
					{
						strcpy(queryresult[order].state, element);
						printf("[DEBUG_INFO]: record %d in xml state is %s\n", order, element);
					}
					if(colum == pos_nodesnum)
					{
						queryresult[order].nodesnum = atoi(element);
						printf("[DEBUG_INFO]: record %d in xml nodesnum is %d\n", order, queryresult[order].nodesnum);
					}
					if(colum == pos_runtime)
					{
						strcpy(queryresult[order].runtime, element);
						printf("[DEBUG_INFO]: record %d in xml runtime is %s\n", order, queryresult[order].runtime);
					}
					colum++;
				}

                        }
                }
        }
        printf("[DEBUG_INFO]:line number is %d \n", lineNum);
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



int checkStateInMemory(ARL_job *arljobs, int jobnums)
{
	printf("[DEBUG_INFO]:check Memory !\n");
	if(jobnums > 0 && arljobs != NULL)
	{
		int i;
		int completenums = 0;
		for(i = 0; i < jobnums; i++)
		{
			int jobID = arljobs[i].id;
			char state[MIN_STRING_BUFS] = {0};
			strcpy(state, arljobs[i].state);
			if(!strcmp(state, COMPLETED))
			{
				printf("[DEBUG_INFO]:known from memory various, job id = %d has been completed\n", jobID);
				completenums++;
				continue;
			}
			else
			{
				char record[MIN_STRING_BUFS] = {0};	
				getRecordInXml(jobID, "state", record);
				printf("[DEBUG_INFO]:job %d state code in xml is %s\n", jobID, record);
				if(!strcmp(record, COMPLETED))
				{
					printf("[DEBUG_INFO]:known from xml, job id = %d has been completed\n", jobID);
					strcpy(arljobs[i].state, COMPLETED);
					completenums++;
					continue;
				}
				printf("[DEBUG_INFO]:job %d has not been completed!\n", jobID);
			}
		}
		if(completenums == jobnums)
		{
			printf("[DEBUG_INFO]: WOW! all jobs have been completed!\n");
			finishflag = TRUE;
			return TRUE;
		}	
		else
		{
			printf("[DEBUG_INFO]: Still have some jobs unfinished! Check their realtime state\n");
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

int queryState(ARL_job *arljobs, int jobnums)
{
	char command[MAX_STRING_BUFS] = {0};
	strcpy(command, "squeue |&tee ");
	strcat(command, QUERY_LOG);
	int rst = system(command);	
	printf("[DEBUG_INFO]:yhq:%s result is %d\n", command, rst);

	/**	because the existance of the monitor job, the query result include them	**/
	/**	not only the jobs of the target programs				**/
	int rownums = getRowNumsFromFile(QUERY_LOG);

	/**	here aubtract 1 because the head line in the file is attribute	**/
	int entrynums = rownums -1;	
	QUERY_dimension *queryresult = malloc(sizeof(QUERY_dimension)*entrynums);
	if(queryresult == NULL && entrynums != 0)
	{
		printf("[DEBUG_INFO]:malloc error %s %d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	readFileByLine(QUERY_LOG, queryresult, entrynums);
	int i,j;
	for(i = 0; i < jobnums; i++)
	{
		int existflag = FALSE;
		int jobID = arljobs[i].id;
		long lidinmemory = arljobs[i].launchID;
		int parameter_N = arljobs[i].parameter_N;
		char *stateinmemory = arljobs[i].state;
		long limittime = arljobs[i].limittime;

		int enabletimecheck = FALSE;
		if(limittime > 0)
		{
			enabletimecheck = TRUE;
		}

		for(j = 0; j < entrynums; j++)
		{
			long lidfromquery = queryresult[j].launchID;
			char state[MIN_STRING_BUFS] = {0};
			strcpy(state, queryresult[j].state);
			int nodesnum = queryresult[j].nodesnum;
			char runtime[MIN_STRING_BUFS] = {0};
			strcpy(runtime, queryresult[j].runtime);

			if(lidinmemory == lidfromquery)
			{
				existflag = TRUE;
			}
			else
			{
				continue;
			}
			
			int flag_cancel = FALSE;
			if(nodesnum != parameter_N)
			{
				printf("[DEBUG_INFO]:jobID is %d, luanchID is %ld parameter_N is %d, nodesnum from yhq is %d\n", 
					jobID, lidfromquery, parameter_N, nodesnum);
				printf("[DEBUG_INFO]: parts of the job is terminated abnormally! call yhcanel this failure job\n");
				flag_cancel = TRUE;
			}
			else
			{
				printf("[DEBUG_INFO]: current job's state is %s\n", state);
			}
			
			if(TRUE == enabletimecheck)
			{
				printf("[DEBUG_INFO]: check limittime!\n");
				long realcost = getruntime(runtime);
				printf("[DEBUG_INFO]:job:%ld realtime is %ld\n", lidfromquery, realcost);
				if(realcost > limittime)
				{
					printf("[DEBUG_INFO]:real runtime beyond the limitation! cancel this job:%ld\n", lidfromquery);
					flag_cancel = TRUE;
				}
			}
			if(flag_cancel == TRUE)
			{
				char command[MID_STRING_BUFS] = {0};
                                sprintf(command, "yhcancel %ld", lidfromquery);
                                system(command);
                                printf("[DEBUG_INFO]:update job state in memory and in xml\n");
                                printf("[DEBUG_INFO]:rerun the failed job\n");
                                reSubmit(arljobs, jobnums, jobID);
			}
			

			if(existflag)
			{
				printf("[DEBUG_INFO]: find a matched entry in yhq result for current job %d\n", jobID);
				printf("[DEBUG_INFO]: search corresponding entry for the next job!\n");
				break;
			}
		}
		if(!existflag)
		{
			//for the  extreme case that tha target job actually completed so that didn't exhibit in the yhq result
			printf("[DEBUG_INFO]: for the current job %d no record in yhq result!\n", jobID);
			if(!strcmp(stateinmemory, COMPLETED))
			{
				printf("[DEBUG_INFO]: because job %d has been finished!\n", jobID);
			}
			else
			{
				printf("[DEBUG_INFO]: there is no corresponding job now for jobID %d, launchID %ld\n",jobID, lidinmemory);
				printf("[DEBUG_INFO]: Now rerun this job and update state info in memory and xml\n");
				reSubmit(arljobs, jobnums, jobID);
			}
		}
	}
	return TRUE;	
}


int stateCheck(ARL_job *arljobs, int jobnums)
{
	if(arljobs == NULL || jobnums <=0)
	{
		return FALSE;
	}
	int finishflag = checkStateInMemory(arljobs, jobnums);
	if(finishflag == FALSE)
	{
		queryState(arljobs, jobnums);
	}
	return TRUE;
}


int getMonitorID(char *lIDarray, int resultnums)
{
	char command[MAX_STRING_BUFS] = {0};
	strcpy(command, "squeue |tee ");
	strcat(command, QUERY_LOG);
	int rst = system(command);
	printf("[DEBUG_INFO]: yhq:%s result is %d\n", command, rst);
	if(rst != 0)
	{
		printf("[DEBUG_INFO]: query from batch processing system error!\n");
		return FALSE;
	}
	
	FILE *fp = NULL;
	char *line = NULL;
	char *preRead = NULL;
	int curLineFlag = NEWLINE;
	int preLineFlag = NEWLINE;
	int lineNum = 0;
	int readCounter = 0;
	
	int pos_launchID = -1;
	char delim[10] = " \t\n";
	char queryresult[MAX_STRING_BUFS] = {0};
	resultnums = 0;
	
	fp = fopen(QUERY_LOG, "r");
	if(fp == NULL)
	{
		printf("[DEBUG_INFO]: can not load %s\n", QUERY_LOG);
		return FALSE;
	}
	line = malloc(sizeof(char)*MAXBUFF);
	while(!feof(fp))
	{
		readCounter++;
		preLineFlag = curLineFlag;
		memset(line, 0, MAXBUFF);
		fgets(line, MAXBUFF, fp);
		printf("[DEBUG_INFO]: fgets occurred!\n");
		int len = strlen(line);
		if(len == 0)
		{
			continue;
		}
		printf("[DEBUG_INFO]: line length is %d, maxbuff-2 = %d\n", len, MAXBUFF-2);
		printf("[DEBUG_INFO]: line[MAXBUFF-2] = %c\n", line[MAXBUFF-2]);
		if(len == MAXBUFF-1 && line[MAXBUFF-2] != '\n')
		{
			curLineFlag = NOTNEWLINE;
		}
		if(len <= MAXBUFF-1)
		{
			if(line[len-1] == '\n')
			{
				printf("[DEBUG_INFO]: the last element of this line is the enter key\n");
				curLineFlag = NEWLINE;
			}
			else
			{
				printf("[DEBUG_INFO]: the last element of this line is not enter.\n");
				curLineFlag = NOTNEWLINE;
			}
		}
		if(len > MAXBUFF-1)
		{
			printf("[DEBUG_INFO]: this can not happen!\n");
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
				printf("[DEBUG_INFO]: put the data in the bag!\n");
			}
		}		
		if(preLineFlag == NOTNEWLINE)
		{
			printf("[DEBUG_INFO]: need to combine with last content!\n");
			char *newRead =  malloc(sizeof(char)*(readCounter + 1)*MAXBUFF);
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
			readCounter = 0;
			lineNum++;
			if(lineNum == 1)
			{
				printf("[DEBUG_INFO]: this line is head line denotes attributes: %s\n", line);
				char *dimension = NULL;
				int colum = 0;
				for(dimension = strtok(line, delim); dimension != NULL; dimension = strtok(NULL, delim))
				{
					printf("[DEBUG_INFO]: dimension is %s\n", dimension);
					if(!strcmp(dimension, Dime_launchID))
					{
						pos_launchID = colum;
						printf("[DEBUG_INFO]: %s position is %d\n", dimension, colum);
					}
					colum++;
				}
			}
			else
			{
				char *currentline = NULL;
				if(preLineFlag == NEWLINE)
				{
					currentline = line;
					printf("[DEBUG_INFO]: this line is :%s\n", currentline);
				}
				else
				{
					currentline = preRead;
					printf("[DEBUG_INFO]: this line is :%s\n", currentline);
				}
				int colum = 0;
				char *element = NULL;
				for(element = strtok(currentline, delim); element != NULL; element = strtok(NULL, delim))
				{
					if(colum == pos_launchID)
					{
						printf("[DEBUG_INFO]: get launchID record at colum %d : %s\n", colum, element);
						strcat(queryresult, element);
						strcat(queryresult, ",");
						resultnums++;
					}
					colum++;
				}
			}
		}

	}
	
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
	
	strcpy(lIDarray, queryresult);	

	fclose(fp);

	return TRUE;

}










