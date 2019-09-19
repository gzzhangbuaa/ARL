#include "interface.h"

int jobSubmit(ARL_job *arljobs, int jobnums)
{
	printf("[DEBUG_INFO]: %d jobnums need to be launched.\n", jobnums);
	if(jobnums > 0 && arljobs != NULL)
	{
		int i;
		for(i = 0; i < jobnums; i++)
		{
			int jobID = arljobs[i].id;
			char command[MAX_STRING_BUFS];
			strcpy(command, arljobs[i].command);
			printf("[DEBUG_INFO]:shell command is :%s\n", command);
			int rst = system(command);
			printf("[DEBUG_INFO]:job %d submit result is %d\n", jobID, rst);
			if(rst != 0)
			{
				return FALSE;
			}
        		char launchID[MIN_STRING_BUFS];
        		getLaunchID(SUBMIT_LOG, launchID);
			arljobs[i].launchID = atol(launchID);
			printf("[DEBUG_INFO]:%d job's launchID is %ld\n", jobID, arljobs[i].launchID);
			strcpy(arljobs[i].state, SUBMITTED);
			printf("[DEBUG_INFO]:%d job current state is %s\n", jobID, arljobs[i].state);
			updateOneProp(CONF_NAME, jobID, "job-launchID", launchID);
			updateOneProp(CONF_NAME, jobID, "job-state", SUBMITTED);

		}
	}
	else
	{
		printf("[DEBUG_INFO]:can't submit jobs!\n");
		return FALSE;
	}
}

int reSubmit(ARL_job *arljobs, int jobnums, int jobID)
{
	if(arljobs == NULL || jobnums <= 0)
	{
		return FALSE;
	}
	int i, position, existflag = FALSE;
	for(i = 0; i < jobnums; i++)
	{
		if(jobID == arljobs[i].id)
		{
			existflag = TRUE;
			position = i;
			break;
		}
	}
	if(!existflag)
	{
		return FALSE;
	}
	char command[MAX_STRING_BUFS];
	strcpy(command, arljobs[i].command);
	int rst = system(command);
	printf("[DEBUG_INFO]: job %d resubmit result is %d\n", jobID, rst);
	if(rst != 0)
	{
		return FALSE;
	}
	char launchID[MIN_STRING_BUFS];
	getLaunchID(SUBMIT_LOG, launchID);
	printf("[DEBUG_INFO]: update the last launchID as history and put the new one as latest\n");
	long newlaunchID = atol(launchID);
	long oldlaunchID = arljobs[i].launchID;
	arljobs[i].launchID = newlaunchID;

	updateOneProp(CONF_NAME, jobID, "job-launchID", launchID);

	printf("[DEBUG_INFO]: job %d old-launch-ID is %ld, new-launch-ID is %ld\n", jobID, oldlaunchID, newlaunchID);
	strcpy(arljobs[i].state, RERUN);
	printf("[DEBUG_INFO]: job %d current state is %s\n", jobID, arljobs[i].state);
	updateOneProp(CONF_NAME, jobID, "job-state", RERUN);

	stateSynToReport(jobID, oldlaunchID, newlaunchID);

	//record the time of resubmit
	time_t timeresub;
	time(&timeresub);
	printf("[DEBUG_INFO] resubmit time is %s**********!\n", ctime(&timeresub));

	return TRUE;
}




int getLaunchID(char *file, char *id)
{
	FILE *fp;
	fp = fopen(file, "r");
	if(NULL == fp)
	{
		printf("[DEBUG_INFO]:can't load file %s\n", file);
		return FALSE;
	}
	char *line = malloc(sizeof(char)*512);
	while(!feof(fp))
	{
		memset(line, 0, sizeof(line));
		fgets(line, 512, fp);
		if(!strlen(line))
		{
			continue;
		}
		//printf("[DEBUG_INFO]: get line:%s\n", line);
		char *token;
		/**	be careful here the delimiter is blank and newlines**/
		for(token = strsep(&line, " ,\n");token != NULL; token = strsep(&line, " ,\n"))
		{
			//printf("[DEBUG_INFO]:splited token is:%s\n", token);
			//printf("strspn result is %d, strlen result is %d\n", strspn(token, "0123456789"), strlen(token));
			if(strspn(token, "0123456789") == strlen(token))
			{
				//printf("[DEBUG_INFO]:This token is a digital\n");
				strcpy(id, token);
				break;
			}
			else
			{
				//printf("[DEBUG_INFO]:This token is a string\n");
			}
		}
		
	}
	if(!line)
	{
		free(line);
		line = NULL;
	}
	fclose(fp);
	return TRUE;
}
