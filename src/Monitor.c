#include "interface.h"

#define ERR_EXIT(m)\
	do{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0);



struct ARL_job	*arljobs;
struct MONITOR_EXECINFO *master_execinfo;
struct MONITOR_EXECINFO *backup_execinfo;

static jobnums = 0;
int finishflag = FALSE;
int enablecheck = FALSE;
int alive	= FALSE;
int morbflag	= TRUE;
int enabledouble = FALSE;

long messagecount = 0;
long messagesize = 0;

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

	master_execinfo = malloc(sizeof(MONITOR_EXECINFO));
	if(!master_execinfo)
	{
		printf("[DEBUG_INFO]: can't malloc for master_execinfo\n");
		return;
	}
	backup_execinfo = malloc(sizeof(MONITOR_EXECINFO));
	if(!backup_execinfo)	
	{
		printf("[DEBUG_INFO]: can't malloc for backup_execinfo!\n");
		return;
	}

	//get the switch of double monitors solution
	enabledouble = getDMS(CONF_NAME, DMSWITCH);	


	//master monitor Init
	
	int mi_result = monitor_Init(&morbflag, master_execinfo, backup_execinfo);
	if(mi_result == FALSE)
	{
		printf("[DEBUG_INFO]: Monitor service Initiation failed! Exit!\n");
		return;
	}


	if(morbflag == TRUE)
	{
		printf("[DEBUG_INFO]: I am the master monitor!\n");
		/**####	create	xml file about exec information	####**/
		createExecXml(CONF_NAME, EXEC_INFO, &jobnums);	
		printf("[DEBUG_INFO]:jobs num need to be launched is %d\n", jobnums);

		/**####	create shell command used for job submit ####**/
		genShCommand(CONF_NAME, jobnums, &arljobs);
	
	
		/**#### job submit ####**/
		jobSubmit(arljobs, jobnums);
	
		/**#### synchnoize state information between ARL_job stuct and execInfo.xml ####**/
	
		updateState(EXEC_INFO, arljobs, jobnums);	
		addTimer(TIMEOUT);
	

		char *message = malloc(sizeof(char)*MIN_STRING_BUFS);
		memset(message, 0, MIN_STRING_BUFS);
		sprintf(message, "%d", STARTWORK);

		//get the detail information of the backup monitor
		//getMonitorExecInfo(FALSE, backup_execinfo);
		if(enabledouble)
		{
			printf("[DEBUG_INFO]: master call clientTCP to host %s, message is %s\n", backup_execinfo->hostname, message);
			clientTCP(backup_execinfo->hostname, message);
			
			messagecount++;
			messagesize = messagesize + sizeof(message)*8;			
		}
		memset(message, 0, MIN_STRING_BUFS);	
		sprintf(message, "%d", HEARTBEAT);	

		while(!finishflag)
		{
			while(enablecheck)
			{
				if(enabledouble)
				{
					printf("[DEBUG_INFO]: ####### master send a heartbeat to host %s,message is %s\n", backup_execinfo->hostname, message);
					clientTCP(backup_execinfo->hostname, message);

					messagecount++;
					messagesize = messagesize + sizeof(message)*8;
				}
				printf("[DEBUG_INFO]: variable enablecheck is %d\n", enablecheck);
				stateCheck(arljobs, jobnums);
				enablecheck = FALSE;
			}
		}
		printf("[DEBUG_INFO]: all the jobs are completed! WOW!\n");

		if(enabledouble)
		{
			memset(message, 0, MIN_STRING_BUFS);	
			sprintf(message, "%d", WORKOVER);
			printf("[DEBUG_INFO]: master notify backup work is over! to host is %s, message is %s\n", backup_execinfo->hostname, message);
			clientTCP(backup_execinfo->hostname, message);
			
			messagecount++;
			messagesize = messagesize + sizeof(message)*8;
		}

		if(!message)
		{
			free(message);
		}

		char removecommand[MIN_STRING_BUFS] = {0};
		sprintf(removecommand, "rm %s %s", SUBMIT_LOG, QUERY_LOG);
		memset(removecommand, 0, MIN_STRING_BUFS);
		sprintf(removecommand, "rm -rf %s", SHELLFOLDER);

		printf("[DEBUG_INFO]:message count is %ld\n", messagecount);
		printf("[DEBUG_INFO]:message size is %ld bite\n", messagesize);
	}
	else
	{
		printf("[DEBUG_INFO]: backup monitor is turnning off!\n");
		printf("[DEBUG_INFO]: variable finishflag in backup is %d\n", finishflag);
		
		if(finishflag == FALSE)
		{
			printf("[DEBUG_INFO]: Now take over the monitor!\n");
			genShCommand(CONF_NAME, jobnums, &arljobs);
			addTimer(TIMEOUT);
		}
		else
		{
			printf("[DEBUG_INFO]: The Backup monitor ready to exit normally!\n");
		}

		while(!finishflag)
		{
			while(enablecheck)
			{
				printf("[DEBUG_INFO]: variable enablecheck is %d\n", enablecheck);
				stateCheck(arljobs, jobnums);
				enablecheck = FALSE;
			}
		}
                char removecommand[MIN_STRING_BUFS] = {0};
                sprintf(removecommand, "rm %s %s", SUBMIT_LOG, QUERY_LOG);
                memset(removecommand, 0, MIN_STRING_BUFS);
                sprintf(removecommand, "rm -rf %s", SHELLFOLDER);			
		
	}
	if(arljobs != NULL)
	{
		printf("[DEBUG_INFO]: free arljobs\n");
		free(arljobs);
	}	
	if(master_execinfo != NULL)
	{
		printf("[DEBUG_INFO]: free master_execinfo\n");
		free(master_execinfo);
	}
	if(backup_execinfo != NULL)
	{
		printf("[DEBUG_INFO]: free backup_execinfo\n");
		free(backup_execinfo);
	}
}
