#include "interface.h"

#define ERR_EXIT(m)\
	do{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0);



struct ARL_job	*arljobs;
static jobnums = 0;
int finishflag = FALSE;
int enablecheck = FALSE;

int getlaunchID(char *file, char *id);

int jobSubmit()
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
        		getlaunchID(SUBMIT_LOG, launchID);
			arljobs[i].launchID = atol(launchID);
			printf("[DEBUG_INFO]:%d job's launchID is %ld\n", jobID, arljobs[i].launchID);
			strcpy(arljobs[i].state, SUBMITTED);
			printf("[DEBUG_INFO]:%d job current state is %s\n", jobID, arljobs[i].state);
		}
	}
	else
	{
		printf("[DEBUG_INFO]:can't submit jobs!\n");
		return FALSE;
	}
}

void main()
{
	char hostName[MAX_NAME_BUFS];
	gethostname(hostName, MAX_NAME_BUFS);
	printf("My host-name is %s\n", hostName);

	struct hostent *hp;
	if((hp = gethostbyname(hostName)) == NULL)
	{
		ERR_EXIT("gethostbyname");
	}
	int i = 0;
	while(hp->h_addr_list[i] != NULL)
	{
		printf("hostname:%s\n", hp->h_name);
		printf("ip:%s\n", inet_ntoa(*(struct in_addr*)hp->h_addr_list[i]));
		i++;
	}

	
	/**####	create	xml file about exec information	####**/
	createExecXml(CONF_NAME, EXEC_INFO, &jobnums);
	
	printf("[DEBUG_INFO]:jobs num need to be launched is %d\n", jobnums);

	/**####	create shell command used for job submit ####**/
	genShCommand(CONF_NAME, jobnums, &arljobs);
	
	
	/**#### job submit ####**/
	jobSubmit();
	
	/**#### synchnoize state information between ARL_job stuct and execInfo.xml ####**/
	updateState(EXEC_INFO, arljobs, jobnums);	
	
	addTimer(TIMEOUT);

	while(!finishflag)
	{
		while(enablecheck)
		{
			stateCheck(arljobs, jobnums);
			enablecheck = FALSE;
		}
	}
	printf("[DEBUG_INFO]: all the jobs are completed! WOW!\n");	

	//int rst = system("yhbatch -N 1 -n 4 -p paratera ./hellolaunch.sh |&tee jobsubmit.log");
	//printf("job submit command result is %d\n", rst);
	//system("yhi |&tee yhi.log");v
	
	//char launchID[64];
	//getlaunchID("jobsubmit.log", launchID);
	//printf("[DEBUG_INFO]:job submitted, launch-ID is %s\n", launchID);
}

int getlaunchID(char *file, char *id)
{
	FILE *fp;
	fp = fopen(file, "r");
	if(NULL == fp)
	{
		printf("[DEBUG_INFO]:can't load file %s\n", file);
		return -1;
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
	return 0;
}
