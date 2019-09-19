#include "interface.h"
#include <sys/types.h>
#include <libxml/xmlmemory.h>
#define CONF_FILE	"./configure.xml"

/*
int genShCommand(char *confpath, int nums, ARL_job *arl_jobs);
int main()
{
	ARL_job *arl_jobs = NULL;
	genShCommand(CONF_FILE, 2, arl_jobs);
}
*/
long getruntime(char *runtime)
{
	if(runtime == NULL)
	{
		printf("[DEBUG_INFO]:runtime string input is NULL\n");
		return 0;
	}
	printf("[DEBUG_INFO]: runtime is %s\n", runtime);
	int time[10];	
	int count = 0;
	char *ptr, *retptr;
	ptr = runtime;
	time[0] = count;
	int i = 0;
	while((retptr = strtok(ptr, ":")) != NULL)
	{
		i++;
		ptr = NULL;
		int num = atoi(retptr);
		count = i;
		time[0] = count;
		time[i] = num;
		printf("[DEBUG_INFO]:getruntime:key=%d, value=%d, count=%d\n", i, num, count);
	}
	long cost = 0;
	if(count >= 1)
	{
		cost = time[count];
		printf("[DEBUG_INFO]:*********secondes count is %d\n", time[count]);
	}
	if(count >= 2)
	{
		cost = cost + 60*time[count-1];
		printf("[DEBUG_INFO]:**********minutes count is %d\n", time[count-1]);
	}
	if(count >= 3)
	{
		cost = cost + 60*60*time[count-2];
		printf("[DEBUG_INFO]:**********hours count is %d\n", time[count-2]);
	}
	if(count == 4)
	{
		cost = cost + 60*60*24*time[count-3];
		printf("[DEBUG_INFO]:**********days count is %d\n", time[count-3]);
	}
	if(count > 4)
	{
		printf("[DEBUG_INFO]: This runtime maybe abnormal!\n");
		return -1;
	}
	printf("[DEBUG_INFO]:runtime input is %s, totalseconds is %ld\n", runtime, cost);
	return cost;
}

int checklimittime(char *limittime, long *totalseconds)
{
	if(limittime == NULL)
	{
		return FALSE;
	}

	char *ptr, *retptr;
        int i = 0;
        ptr = limittime;
        int count = 0;

        int time[5];
        time[0] = count;
        while((retptr = strtok(ptr, ":")) != NULL)
        {
		i++;
                ptr = NULL;
                int isdigit = strspn(retptr, "0123456789")==strlen(retptr);
                if(isdigit == 1)
                {
                        printf("[DEBUG_INFO]:%s This is a digit!\n", retptr);
                }
                else
                {
                        printf("[DEBUG_INFO]: limittime set in xml %s is not a digit! limit-time format 00:00:00:00 (days:hours:minutes:seconds)\n", retptr);
			return FALSE;
                }
                int num = atoi(retptr);
                printf("[DEBUG_INFO]:the %d num is %d\n", i, num);
                count = i;
                time[0] = count;
                if(count > 4)
                {
                        printf("[DEBUG_INFO]:limit-time format set wrong! limit-time format 00:00:00:00 (days:hours:minutes:seconds)\n");
			return FALSE;
                }
                time[i] = num;
        }

        if(count >=1 && (time[count] < 0 || time[count] > 60))
        {
                printf("[DEBUG_INFO]:limit-time set in xml file error, seconds value should between 0 and 60! \n");
		return FALSE;
        }
        if(count >= 2 && (time[count-1] < 0 || time[count-1] > 60))
        {
                printf("[DEBUG_INFO]:limit-time set in xml file error, minutes value should between 0 and 60! \n");
		return FALSE;
        }
        if(count >= 3 && (time[count-2] < 0 || time[count-2] > 24))
        {
                printf("[DEBUG_INFO]:limit-time set in xml file error, hours value should between 0 and 24! \n");
		return FALSE;
        }
        if(count >= 4 && (time[count-3] < 0))
        {
                printf("[DEBUG_INFO]:limit-time set in xml file error, days value should be an integer! \n");
		return FALSE;
        }	

	int sum = 0;
	if(count >= 1)
	{
		sum = time[count];
	}
	if(count >= 2)
	{
		sum = sum + 60*time[count-1]; 
	}	
	if(count >= 3)
	{
		sum = sum + 60*60*time[count-2];
	}
	if(count == 4)
	{
		sum = sum + 60*60*24*time[count-3];
	}
	if(count > 4)
	{
		printf("[DEBUG_INFO]:limit-time format set wrong! limit-time format 00:00:00:00 (days:hours:minutes:seconds)\n");
		return FALSE;
	}
	(*totalseconds) = sum;	

        return TRUE;
}



int genShCommand(char *confpath, int nums, ARL_job **arl_jobs)
{
	(*arl_jobs) = (ARL_job *)malloc(sizeof(ARL_job)*nums);
	if(!arl_jobs)
	{
		printf("[DEBUG_INFO]:memory allocation for struct ARL_job fail\n");
		return FALSE;
	}
	
	xmlDocPtr pDoc = xmlReadFile(confpath, "UTF-8", XML_PARSE_RECOVER);
	if(NULL == pDoc)
	{
		fprintf(stderr, "xmlParseFile Error in %s %d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	
	xmlNodePtr pRoot = xmlDocGetRootElement(pDoc);
	if(NULL == pRoot)
	{
		fprintf(stderr, "xmlDocGetRootElement Error in %s %d\n", __FUNCTION__, __LINE__);
		xmlFreeDoc(pDoc);
		return FALSE;
	}
	printf("Node name is %s!\n", pRoot->name);


	int currentnodeID = 0;
	xmlNodePtr pFirst = pRoot->xmlChildrenNode;
	while(NULL != pFirst)
	{
		if(!xmlStrcmp(pFirst->name, (const xmlChar *)("job")))
		{
			char jobshell[256];
			strcpy(jobshell, "yhbatch");
			xmlNodePtr pSecond = pFirst->xmlChildrenNode;

			int flag = TRUE;
			int condflag_ID = FALSE, condflag_n = FALSE, condflag_N = FALSE,
			condflag_p = FALSE, condflag_shpath = FALSE;

			xmlChar *jobID = NULL, *parameter_n = NULL, *parameter_N = NULL,
			*parameter_p = NULL, *parameter_path = NULL;
			xmlChar *launchID = NULL, *state = NULL, *limittime = NULL;

			long totalseconds = 0;

			char newshellpath[MID_STRING_BUFS] = {0};
			
			while(NULL != pSecond)
			{
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("job-ID")))
				{
					condflag_ID = TRUE;
					jobID = xmlNodeGetContent(pSecond);
					if(jobID == NULL || strlen(jobID) == 0)
					{
						printf("[DEBUG_INFO]:Please check the configure.xml, job-ID should not be empty!\n");
						flag = FALSE;
					}
					else
					{
						printf("job-ID is %s\n", jobID);
					}
				}
                                if(!xmlStrcmp(pSecond->name, (const xmlChar *)("parameter-N")))
                                {
                                        condflag_N = TRUE;
                                        parameter_N = xmlNodeGetContent(pSecond);
                                        printf("\n%s-->%s\n", pSecond->name, parameter_N);
                                        if(parameter_N == NULL||strlen(parameter_N) == 0)
                                        {
                                                printf("[DEBUG_INFO]:parameter-N can't be empty!\n");
                                                flag = FALSE;
                                        }
                                        else
                                        {
                                                strcat(jobshell, " -N ");
                                                strcat(jobshell, parameter_N);
                                        }
                                }				

				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("parameter-n")))
				{
					condflag_n = TRUE;
					parameter_n = xmlNodeGetContent(pSecond);
					printf("\n%s-->%s\n", pSecond->name, parameter_n);
					if(parameter_n == NULL||strlen(parameter_n) == 0)
					{
						printf("[DEBUG_INFO]:parameter-n can't be empty!\n");
						flag = FALSE;
					}
					else
					{
						strcat(jobshell, " -n ");
						strcat(jobshell, parameter_n);
					}	
				}
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("parameter-p")))
				{
					condflag_p = TRUE;
					parameter_p = xmlNodeGetContent(pSecond);
					printf("\n%s-->%s\n", pSecond->name, parameter_p);
					if(parameter_p == NULL||strlen(parameter_p) == 0)
					{
						printf("[DEBUG_INFO]:parameter-p can't be empty!\n");
						flag = FALSE;
					}
					else
					{
						strcat(jobshell, " -p ");
						strcat(jobshell, parameter_p);
					}
				}
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("jobshell-Path")))
				{
					condflag_shpath = TRUE;
					parameter_path = xmlNodeGetContent(pSecond);
					printf("\n%s-->%s\n", pSecond->name, parameter_path);
					if(parameter_path == NULL||strlen(parameter_path) == 0)
					{
						printf("[DEBUG_INFO]:jobshell-Path can't be empty!\n");
						flag = FALSE;
					}
					else
					{
						if(getOwnShell(parameter_path, jobID, newshellpath))
						{
							return FALSE;
						}
						printf("[DEBUG_INFO]:get new shell path is %s\n", newshellpath);
						strcat(jobshell, " ");
						strcat(jobshell, newshellpath);
					}
				}
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("job-launchID")))
				{
					launchID = xmlNodeGetContent(pSecond);
					printf("\n%s-->%s\n", pSecond->name, launchID);
				}
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("job-state")))
				{
					state = xmlNodeGetContent(pSecond);
					printf("\n%s-->%s\n", pSecond->name, state);
				}
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("limit-time")))
				{
					limittime = xmlNodeGetContent(pSecond);
					printf("[DEBUG_INFO]:get limit-time is %s\n", limittime);
					printf("[DEBUG_INFO]: check limit-time value!\n");
					if(FALSE == checklimittime(limittime, &totalseconds))
					{
						printf("[DEBUG_INFO]:check and fix limittime in xml file!\n");
						return FALSE;
					}
				}


				pSecond = pSecond->next;
			}	
			if(flag == FALSE)
			{
				printf("[DEBUG_INFO]:The necessary parameters in the configure.xml can't be empty!\n");
				return FALSE;
			}
			else
			{
				strcat(jobshell, " |&tee jobsubmit.log");
				printf(jobshell);
				printf("\n");
			}
			if(!(condflag_ID && condflag_n && condflag_N && condflag_p &&condflag_shpath))
			{
				printf("[DEBUG_INFO]:keyword segment missing, check configure.xml\n");
				return FALSE;
			}
			if(currentnodeID < nums)
			{
				
				(*arl_jobs)[currentnodeID].id = atoi(jobID);
				(*arl_jobs)[currentnodeID].parameter_n = atoi(parameter_n);
				(*arl_jobs)[currentnodeID].parameter_N = atoi(parameter_N);
				(*arl_jobs)[currentnodeID].launchID = atol(launchID);
				strcpy((*arl_jobs)[currentnodeID].partition, parameter_p);
				strcpy((*arl_jobs)[currentnodeID].shellpath, newshellpath);
				strcpy((*arl_jobs)[currentnodeID].state, state);
				strcpy((*arl_jobs)[currentnodeID].command, jobshell);
				(*arl_jobs)[currentnodeID].launchID = 0;
				(*arl_jobs)[currentnodeID].limittime = totalseconds;
					
				printf("[DEBUG_INFO]: %d element's jobID is %d\n", currentnodeID, (*arl_jobs)[currentnodeID].id);
                                printf("[DEBUG_INFO]: %d element's N is %d\n", currentnodeID, (*arl_jobs)[currentnodeID].parameter_N);
                                printf("[DEBUG_INFO]: %d element's n is %d\n", currentnodeID, (*arl_jobs)[currentnodeID].parameter_n);
                                printf("[DEBUG_INFO]: %d element's p is %s\n", currentnodeID, (*arl_jobs)[currentnodeID].partition);
                                printf("[DEBUG_INFO]: %d element's path is %s\n", currentnodeID, (*arl_jobs)[currentnodeID].shellpath);
                                printf("[DEBUG_INFO]: %d element's state is %s\n", currentnodeID, (*arl_jobs)[currentnodeID].state);
        	                printf("[DEBUG_INFO]: %d element's command is %s\n", currentnodeID, (*arl_jobs)[currentnodeID].command); 
		              	printf("[DEBUG_INFO]: %d element's launchID is %ld\n", currentnodeID, (*arl_jobs)[currentnodeID].launchID);
				printf("[DEBUG_INFO]: %d element's limittime is %ld\n", currentnodeID, (*arl_jobs)[currentnodeID].limittime);
			}


			if(jobID)
			{
				xmlFree(jobID);		
				jobID = NULL;		
			}
			if(parameter_n)
			{
				xmlFree(parameter_n);
				parameter_n = NULL;
			}
			if(parameter_N)
			{
				xmlFree(parameter_N);
				parameter_N = NULL;
			}
			if(parameter_p)
			{
				xmlFree(parameter_p);
				parameter_p = NULL;
			}
			if(parameter_path)
			{
				xmlFree(parameter_path);
				parameter_path = NULL;
			}
			currentnodeID++;
		}
		
		pFirst = pFirst->next;
	}
	xmlFreeDoc(pDoc);
	return TRUE;
}




