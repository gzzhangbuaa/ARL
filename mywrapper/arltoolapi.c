#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <unistd.h>


#include "../include/share.h"
//#include "arltoolapi.h"


//#include "arltoolapi.h"

int leaderflag = FALSE;
int jobID = -1;


int updateExecState(char *targetfile, int jobID, char *state)
{
	xmlDocPtr doc;
	xmlNodePtr rootNode;
	doc = xmlReadFile(targetfile, "UTF-8", XML_PARSE_RECOVER);
	if(NULL == doc)
	{
		printf("[DEBUG_INFO]:xmlReadFile error in %s %d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	rootNode = xmlDocGetRootElement(doc);
	if(NULL == rootNode)
	{
		printf("[DEBUG_INFO]:xmlDocGetRootElement Error in %s %d\n", __FUNCTION__, __LINE__);
		xmlFreeDoc(doc);
		return FALSE;
	}
	xmlNodePtr pFirst = rootNode->xmlChildrenNode;
	int flag = FALSE;
	while(NULL != pFirst)
	{
		if(!xmlStrcmp(pFirst->name, (const xmlChar *)("job")))
		{
			xmlNodePtr pSecond = pFirst->xmlChildrenNode;
			while(NULL != pSecond)
			{
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("ID")))
				{
					xmlChar *id = xmlNodeGetContent(pSecond);
					flag = (jobID == atoi(id)) ? TRUE:FALSE;
					xmlFree(id);
				}
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("state")) && flag == TRUE)
				{
					xmlNodeSetContent(pSecond, BAD_CAST COMPLETED);
					break;
				}
				pSecond = pSecond->next;
			}
		}
		if(flag == TRUE)
		{
			break;
		}
		pFirst = pFirst->next;
	}
	xmlSaveFormatFileEnc(targetfile, doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();
	return TRUE;
}

int notifyJobComplete(int jobID)
{
	int rst;
	rst = updateExecState(EXEC_INFO, jobID, COMPLETED);
	return rst;
}


int ARL_Init(int argc, char * argv[])
{
        int i = 0;
        int id = -1;
        if(argc > 1)
        {
                for(i = 1; i < argc; i++)
                {
                        if(strcmp(argv[i], "-ID") == 0)
                        {
                                if(i+1 < argc)
                                {
                                        id = atoi(argv[i+1]);
                                        printf("[DEBUG_INFO]: command line parameter ID is %d\n", id);
					jobID = id;
                                }
                                else
                                {
                                        printf("[DEBUG_INFO]: missing command line parameter -ID!\n");
                                        return FALSE;
                                }
                        }
                }
        }
	
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//printf("[DEBUG_INFO]: my rank is %d\n", rank);
	if(rank == 0)
	{
		leaderflag = TRUE;
	}
	else
	{
		leaderflag = FALSE;
	}

        return TRUE;
}


int ARL_Finalize()
{
	//printf("[DEBUG_INFO]:here coming!\n");
	int rst = FALSE;
	if(leaderflag)
	{
		rst = notifyJobComplete(jobID);
	}
	return rst;
}





