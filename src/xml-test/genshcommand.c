#include "interface.h"
#include <sys/types.h>
#include <libxml/xmlmemory.h>
#define CONF_FILE	"./configure.xml"


int genShCommand(char *confpath, int nums, struct ARL_job **arl_jobs);
int main()
{
	struct ARL_job ** arljobs = NULL;
	genShCommand(CONF_FILE, 2, arl_jobs);
}

int genShCommand(char *confpath, int nums, struct ARL_job **arl_jobs)
{
	arl_jobs = malloc(sizeof(struct ARL)*nums);
	if(!arl_jobs)
	{
		printf("[DEBUG_INFO]:memory allocation for struct ARL_job fail\n");
		return -1;
	}
	
	xmlDocPtr pDoc = xmlReadFile(confpath, "UTF-8", XML_PARSE_RECOVER);
	if(NULL == pDoc)
	{
		fprintf(stderr, "xmlParseFile Error in %s %d\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	xmlNodePtr pRoot = xmlDocGetRootElement(pDoc);
	if(NULL == pRoot)
	{
		fprintf(stderr, "xmlDocGetRootElement Error in %s %d\n", __FUNCTION__, __LINE__);
		xmlFreeDoc(pDoc);
		return -1;
	}
	printf("Node name is %s!\n", pRoot->name);


	int currentnodeID = 0;
	xmlNodePtr pFirst = pRoot->xmlChildrenNode;
	while(NULL != pFirst)
	{
		if(!xmlStrcmp(pFirst->name, (const xmlChar *)("jobs")))
		{
			currentnodeID++;
			char jobshell[256];
			strcpy(jobshell, "yhbatch");
			xmlNodePtr pSecond = pFirst->xmlChildrenNode;

			int flag = 0;
			int condflag_ID = FALSE, condflag_n = FALSE, condflag_N = FALSE,
			condflag_p = FALSE, condflag_shpath = FALSE;

			while(NULL != pSecond)
			{
				xmlChar* jobID = NULL;
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("job-ID")))
				{
					condflag_ID = TRUE;
					jobID = xmlNodeGetContent(pSecond);
					if(jobID == NULL)
					{
						printf("Please check the configure.xml, job-ID should not be empty!\n");
					}
					else
					{
						printf("job-ID is %s\n", jobID);
					}
				}				

				xmlChar* value = NULL;
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("parameter-n")))
				{
					condflag_n = TRUE;
					value = xmlNodeGetContent(pSecond);
					printf("\n%s-->%s\n", pSecond->name, value);
					if(value == NULL||strlen(value) == 0)
					{
						printf("parameter-n can't be empty!\n");
						flag = -1;
					}
					else
					{
						strcat(jobshell, " -n ");
						strcat(jobshell, value);
					}	
					xmlFree(value);
					value = NULL;
				}
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("parameter-N")))
				{
					condflag_N = TRUE;
					value = xmlNodeGetContent(pSecond);
					printf("\n%s-->%s\n", pSecond->name, value);
					if(value == NULL||strlen(value) == 0)
					{
						printf("parameter-N can't be empty!\n");
						flag = -1;
					}
					else
					{
						strcat(jobshell, " -N ");
						strcat(jobshell, value);
					}
					xmlFree(value);
					value = NULL;
				}
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("parameter-p")))
				{
					condflag_p = TRUE;
					value = xmlNodeGetContent(pSecond);
					printf("\n%s-->%s\n", pSecond->name, value);
					if(value == NULL||strlen(value) == 0)
					{
						printf("parameter-p can't be empty!\n");
						flag = -1;
					}
					else
					{
						strcat(jobshell, " -p ");
						strcat(jobshell, value);
					}
					xmlFree(value);
					value = NULL;
				}
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("jobshell-Path")))
				{
					condflag_shpath = TRUE;
					value = xmlNodeGetContent(pSecond);
					printf("\n%s-->%s\n", pSecond->name, value);
					if(value == NULL||strlen(value) == 0)
					{
						printf("jobshell-Path can't be empty!\n");
						flag = -1;
					}
					else
					{
						strcat(jobshell, " ");
						strcat(jobshell, value);
					}
					xmlFree(value);
					value = NULL;
				}
				pSecond = pSecond->next;
			}	
			if(flag == -1)
			{
				return flag;
			}
			else
			{
				printf(jobshell);
				printf("\n");
			}
			if(!(condflag_ID && condflag_n && condflag_N && condflag_p &&condflag_shpath))
			{
				printf("[DEBUG_INFO]:data missing, check configure.xml\n");
			}
		}
		
		pFirst = pFirst->next;
	}
	xmlFreeDoc(pDoc);
	return 0;
}

