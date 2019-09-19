//#include <stdio.h>
//#include <libxml/parser.h>
//#include <libxml/tree.h>
#include <stdlib.h>
#include "interface.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

extern int errno;
extern int enabledouble;

extern int messagecount;
extern int messagesize;

#define SHELLFOLDER     "./shellfiles"
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



int getOwnShell(char *originshell, char *id, char *finalshell)
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

        char newfilepath[MAX_STRING_BUFS];
        char *filename = strrchr(originshell, '/');
        filename = filename+1;
        printf("[DEBUG_INFO]:original shell file name is %s\n", filename);
        char *suffixname = strstr(filename, ".sh");
        int strsize = suffixname - filename;
        char newfilename[MID_STRING_BUFS] = {0};
        strncpy(newfilename, filename, strsize);
        if(id == NULL)
	{
		return -1;
	}
        strcat(newfilename, "_");
        strcat(newfilename, id);
        strcat(newfilename, ".sh");
        printf("[DEBUG_INFO]: This job's own shell file name is :\%s\n", newfilename);

        strcpy(newfilepath, SHELLFOLDER);
        strcat(newfilepath, "/");
        strcat(newfilepath, newfilename);
        printf("[DEBUG_INFO]:This job's own shell path is %s\n", newfilepath);
        char command[MAX_STRING_BUFS];
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
                strcat(parameter, id);
                fwrite(parameter, strlen(parameter), 1, fp);
        }
        else
        {
                strcpy(parameter, " -ID ");
                strcat(parameter, id);
                strcat(parameter, "\n");
                fseek(fp, -size, SEEK_END);
                fwrite(parameter, strlen(parameter), 1, fp);
        }
        fclose(fp);

        strcpy(finalshell, newfilepath);
        return 0;
}





int updateState(char *exstfile, ARL_job *arljobs, int jobnums)
{
	if(jobnums <= 0 || arljobs == NULL)
	{
		return FALSE;
	}

	int i;
	for(i = 0; i <jobnums; i++)
	{
		int jobID = arljobs[i].id;
		char state[MIN_STRING_BUFS];
		strcpy(state, arljobs[i].state);
		char launchID[MIN_STRING_BUFS];
		sprintf(launchID, "%ld", arljobs[i].launchID);
		int rst1 = updateOneProp(exstfile, jobID, "state", state);
		int rst2 = updateOneProp(exstfile, jobID, "latest-launchID", launchID);
		if(!(rst1 && rst2))
		{
			printf("[DEBUG_INFO]: error when call updateOneProp function!\n");
		}
	}
	return TRUE;
}

//==============================================================================
//	update the keyvalue for a specified segment with a perticular jobID
//	one time one record
//==============================================================================
int updateOneProp(char *targetfile, int jobID, char* keyword, char* keyvalue)
{
	xmlDocPtr doc;
	xmlNodePtr rootNode;
	doc = xmlReadFile(targetfile, "UTF-8", XML_PARSE_RECOVER);	
	if(NULL == doc)
	{
		fprintf(stderr, "[DEBUG_INFO]:xmlParseFile Error in %s %d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	rootNode = xmlDocGetRootElement(doc);
	if(NULL == rootNode)
	{
		fprintf(stderr, "[DEBUG_INFO]:xmlDocGetRootElement Error in %s %d\n", __FUNCTION__, __LINE__);
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
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)(keyword)) && flag == TRUE)
				{
					xmlNodeSetContent(pSecond, BAD_CAST keyvalue);
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


int getRecordInXml(int jobID, char *keyword, char *keyvalue)
{
	xmlDocPtr doc;
	xmlNodePtr rootNode;
	doc = xmlReadFile(EXEC_INFO, "UTF-8", XML_PARSE_RECOVER);
	if(NULL == doc)
	{
		printf("[DEBUG_INFO]:xmlReadFile error in %s %d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	rootNode = xmlDocGetRootElement(doc);
	if(NULL == rootNode)
	{
		printf("[DEBUG_INFO]:xmlGetRootElement error in %s %d\n", __FUNCTION__, __LINE__);
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
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)(keyword)) && flag == TRUE)
				{
					xmlChar *value = xmlNodeGetContent(pSecond);
					strcpy(keyvalue, value);
					xmlFree(value);
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
	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();
	return TRUE;
}

/**
 * ==========================================================================================	
 * 	keep the state information consistent between memory and report file 
 * 	eg. execInfo.xml when rerun the failure job
 * ========================================================================================== 
**/

int stateSynToReport(int jobID, long oldlaunchID, long newlaunchID)
{
        xmlDocPtr doc;
        xmlNodePtr rootNode;
	xmlKeepBlanksDefault(0);

        doc = xmlReadFile(EXEC_INFO, "UTF-8", XML_PARSE_RECOVER);
        if(NULL == doc)
        {
                printf("[DEBUG_INFO]:xmlReadFile error in %s %d\n", __FUNCTION__, __LINE__);
                return FALSE;
        }
        rootNode = xmlDocGetRootElement(doc);
        if(NULL == rootNode)
        {
                printf("[DEBUG_INFO]:xmlGetRootElement error in %s %d\n", __FUNCTION__, __LINE__);
                xmlFreeDoc(doc);
                return FALSE;
        }
        xmlNodePtr pFirst = rootNode->xmlChildrenNode;
        int existflag = FALSE;

        while(NULL != pFirst)
        {
		char state[MIN_STRING_BUFS] = {0};
		char claunchID[MIN_STRING_BUFS] = {0};	//current-launch-ID
                if(!xmlStrcmp(pFirst->name, (const xmlChar *)("job")))
                {
                        xmlNodePtr pSecond = pFirst->xmlChildrenNode;
                        while(NULL != pSecond)
                        {
                                if(!xmlStrcmp(pSecond->name, (const xmlChar *)("ID")))
                                {
                                        xmlChar *id = xmlNodeGetContent(pSecond);
                                        existflag = (jobID == atoi(id)) ? TRUE:FALSE;
                                        xmlFree(id);
                                }
                                if(!xmlStrcmp(pSecond->name, (const xmlChar *)("state")) && existflag == TRUE)
                                {
                                        xmlChar *value = xmlNodeGetContent(pSecond);
                                        strcpy(state, value);
                                        xmlFree(value);
					if(!strcmp(state, COMPLETED))
					{
						printf("[DEBUG_INFO]:Extreme case, the job resubmited has been quickly completed!\n");
					}
					else
					{
						 xmlNodeSetContent(pSecond, BAD_CAST RERUN);
					}
                                }
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("latest-launchID")) && existflag == TRUE)
				{
					xmlChar *value = xmlNodeGetContent(pSecond);
					strcpy(claunchID, value);
					xmlFree(value);

					if(oldlaunchID != atol(claunchID))
					{
						printf("[DEBUG_INFO]: Something strange occurred, the launchID in memory and in xml is different!\n");
					}
					printf("[DEBUG_INFO]: Save the new launchID as the latest one in xml report!\n");
					char newlidStr[MIN_STRING_BUFS] = {0};
					char oldlidStr[MIN_STRING_BUFS] = {0};
					sprintf(newlidStr, "%ld", newlaunchID);
					sprintf(oldlidStr, "%ld", oldlaunchID);
					xmlNodeSetContent(pSecond, BAD_CAST newlidStr);
					xmlSaveFormatFileEnc(EXEC_INFO, doc, "UTF-8", 1);
					
					printf("[DEBUG_INFO]: Save the old launchID as the history!\n");
                                	xmlNodePtr history = NULL;
                                	history = xmlNewChild(pFirst, NULL, BAD_CAST "history", BAD_CAST oldlidStr);
                                	xmlSaveFormatFileEnc(EXEC_INFO, doc, "UTF-8", 1);
                                	break;
				}
                                pSecond = pSecond->next;
                        }
			if(existflag == TRUE)
			{
				break;
			}
                }
                pFirst = pFirst->next;
        }
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        return TRUE;
}




int createExecXml(char *confxml, char *execxml, int *jobnums)
{
	if((access(confxml, 0)) == -1)
	{
		printf("[DEBUG_INFO]:doc %s dosen't exists, you should create it on demand!\n", confxml);
		return -1;
	}
	
	xmlDocPtr execdoc = NULL, confdoc = NULL;
	xmlNodePtr root_node = NULL;
	execdoc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "exec-info");	
	xmlDocSetRootElement(execdoc, root_node);
	
	confdoc = getdoc(confxml);
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	xmlChar *xpath = (xmlChar *)"//job-ID";

	result = getnodeset(confdoc, xpath);	
	if(result)
	{
		nodeset = result->nodesetval;
		*jobnums = nodeset->nodeNr;		

		int i;
		for(i = 0; i < nodeset->nodeNr; i++)
		{
			xmlChar *keyvalue;
			keyvalue = xmlNodeListGetString(confdoc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
			printf("[DEBUG_INFO]:current job-ID is %s,Now add it to execxml\n", keyvalue);
			
			xmlNodePtr job_node = NULL;
			job_node = xmlNewChild(root_node, NULL, BAD_CAST "job", NULL);
			xmlNodePtr id_node = NULL, state_node = NULL, latest_launchID_node = NULL;
			id_node = xmlNewChild(job_node, NULL, BAD_CAST "ID", BAD_CAST keyvalue);
			state_node = xmlNewChild(job_node, NULL, BAD_CAST "state", NULL);
			latest_launchID_node = xmlNewChild(job_node, NULL, BAD_CAST "latest-launchID", NULL);
			if(keyvalue)
			{
				xmlFree(keyvalue);
			}
		}
		xmlXPathFreeObject(result);
		xmlSaveFormatFileEnc(execxml, execdoc, "UTF-8", 1);
	}
	xmlFreeDoc(execdoc);
	xmlFreeDoc(confdoc);

	xmlCleanupParser();	
	xmlMemoryDump();
	return (0);
}

/**
 * =================================================================================================
 *	manipulate on monitor.xml file including create, getdata and update etc.
 * =================================================================================================
**/
int createMonitorXml()
{

        xmlKeepBlanksDefault(0);
        xmlIndentTreeOutput = 1;
        xmlDocPtr doc = NULL;
        xmlNodePtr root_node = NULL, jobs_node = NULL, switch_node = NULL;
        doc = xmlNewDoc(BAD_CAST "1.0");
        root_node = xmlNewNode(NULL, BAD_CAST "Monitor");
        xmlDocSetRootElement(doc, root_node);

        xmlNodePtr shellpath_node = NULL, master_node = NULL, backup_node = NULL;

        shellpath_node = xmlNewChild(root_node, NULL, BAD_CAST "Shell-path", BAD_CAST "./autoReLaunch.sh");
        master_node = xmlNewChild(root_node, NULL, BAD_CAST "Master", NULL);
        backup_node = xmlNewChild(root_node, NULL, BAD_CAST "Backup", NULL);

        xmlNodePtr mIP_node = NULL, mlaunchID_node = NULL;
        xmlNodePtr bIP_node = NULL, blaunchID_node = NULL;
        mIP_node = xmlNewChild(master_node, NULL, BAD_CAST "IP", NULL);
        mlaunchID_node = xmlNewChild(master_node, NULL, BAD_CAST "launchID", NULL);
        bIP_node = xmlNewChild(backup_node, NULL, BAD_CAST "IP", NULL);
        blaunchID_node = xmlNewChild(backup_node, NULL, BAD_CAST "launchID", NULL);

        xmlSaveFormatFileEnc(MONITOR_INFO, doc, "UTF-8", 1);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        return (0);
}

int getMonitorData(char *keyword, char *keyvalue)
{
	xmlDocPtr doc;
	xmlNodePtr rootNode;
	doc = xmlReadFile(MONITOR_INFO, "UTF-8", XML_PARSE_RECOVER);
	if(NULL == doc)
	{
		printf("[DEBUG_INFO]: xmlReadFile error in %s %d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	rootNode = xmlDocGetRootElement(doc);
	if(rootNode == NULL)
	{
		printf("[DEBUG_INFO]: xmlGetRootElement error in %s %d\n", __FUNCTION__, __LINE__);
		xmlFreeDoc(doc);
		return FALSE;
	}

	printf("[DEBUG_INFO]:##### I am looking for %s in %s\n", keyword, MONITOR_INFO);
	xmlNodePtr pFirst = rootNode->xmlChildrenNode;
	int flag = FALSE;
	while(NULL != pFirst)
	{
		printf("[DEBUG_INFO]:pFirst->name is %s\n", pFirst->name);

		if(!strcmp(pFirst->name, (const xmlChar *)keyword))
		{
			xmlChar *value = xmlNodeGetContent(pFirst);
			printf("[DEBUG_INFO]:current looking key is %s value is %s\n", pFirst->name, value);
			if(value != NULL)
			{
				strcpy(keyvalue, value);
				xmlFree(value);
			}
			flag = TRUE;
			break;
		}	
		else
		{
			xmlNodePtr pSecond = pFirst->xmlChildrenNode;
			while(NULL != pSecond)
			{
				printf("[DEBUG_INFO]:pSecond->name is %s\n", pSecond->name);

				if(!strcmp(pSecond->name, (const xmlChar *)keyword))
				{
					xmlChar *value = xmlNodeGetContent(pSecond);
					printf("[DEBUG_INFO]:current looking key is %s value is %s\n", pSecond->name, value);				
					if(value != NULL)
					{
						strcpy(keyvalue, value);
						xmlFree(value);
					}
					flag = TRUE;
					break;
				}
				pSecond = pSecond->next;
			}
		}	
		if(flag)
		{
			break;
		}
		pFirst = pFirst->next;
	}

	printf("[DEBUG_INFO]:##### keyvalue is %s\n", keyvalue);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();
	return TRUE;
}


int monitor_Init(int* morbflag, MONITOR_EXECINFO *master_execinfo, MONITOR_EXECINFO *backup_execinfo)
{
	int morbflag_local = -1;
        int rst = TRUE;
        if(access(MONITOR_INFO, F_OK|R_OK|W_OK == -1))
        {
                printf("[DEBUG_INFO]: check the existence of %s!\n", MONITOR_INFO);
                return FALSE;
        }

        char originpath[MAX_STRING_BUFS] = {0};
        rst = getMonitorData("shellpath", originpath);
	printf("[DEBUG_INFO]:$$$$$$ ARLTool's shellpath is %s, rst=%d\n", originpath, rst);

        if(!rst)
        {
                printf("[DEBUG_INFO]: can not get shellfile's original path!\n");
                return FALSE;
        }
        if(access(originpath, F_OK|R_OK|W_OK) == -1)
        {
                printf("[DEBUG_INFO]: the original shell file of ARLTool is not right, check it! path is :%s\n", originpath);
                return FALSE;
        }

	char control[MIN_STRING_BUFS] = {0};
	rst = getMonitorData("Placeholder", control);
	if(!rst)
	{
		printf("[DEBUG_INFO]: can not get Placeholder frm monitor.xml to deceide local monitor is master of backup!\n");
		return FALSE;
	}
	if(atoi(control) == 0)
	{
		morbflag_local = TRUE;	
		(*morbflag) = morbflag_local;
	}
	else
	{
		morbflag_local = FALSE;
		(*morbflag) = morbflag_local;
	}
	if(morbflag_local)
	{
		printf("[DEBUG_INFO]: I am the master!\n");
		master_Init(master_execinfo, backup_execinfo);
	}
	else if(enabledouble)
	{
		printf("[DEBUG_INFO]: I am the backup!\n");
		backup_Init(master_execinfo, backup_execinfo);
	}
	else
	{
		printf("[DEBUG_INFO]: Caution! Do not start the Backup monitor.\n");
	}
	return TRUE;
}

int master_Init(MONITOR_EXECINFO *master_execinfo, MONITOR_EXECINFO *backup_execinfo)
{
	int rst = TRUE;
	char *lIDarray = malloc(sizeof(char)*MAX_STRING_BUFS);
	if(lIDarray == NULL)
	{
		printf("[DEBUG_INFO]: malloc error at %s %s\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	int resultnums = 0;
	//master_execinfo = malloc(sizeof(MONITOR_EXECINFO));
	if(!master_execinfo)
	{
		printf("[DEBUG_INFO]: malloc error at %s %d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}	

        gethostname(master_execinfo->hostname, MAX_NAME_BUFS);
        printf("My host-name is %s\n", master_execinfo->hostname);

        struct hostent *hp;
        if((hp = gethostbyname(master_execinfo->hostname)) == NULL)
        {
                printf("[DEBUG_INFO]: gethostbyname error! at %s %d\n", __FUNCTION__, __LINE__);
		return FALSE;
        }
        int i = 0;
        while(hp->h_addr_list[i] != NULL)
        {
                printf("hostname:%s\n", hp->h_name);
                sprintf(master_execinfo->IP, "%s", inet_ntoa(*(struct in_addr*)hp->h_addr_list[i]));
                printf("ip:%s\n", master_execinfo->IP);
                i++;
        }



	rst = getMonitorID(lIDarray, resultnums);
	if(!rst)
	{	
		printf("[DEBUG_INFO]: can not get the launchID of the monitor!\n");
		return FALSE;
	}
	char *token = NULL;
	for(token = strsep(&lIDarray, ","); token != NULL; token = strsep(&lIDarray, ","))
	{
		if(token != "")
		{
			strcpy(master_execinfo->launchID, token);
			break;
		}
	}
	rst = keepIdentity(TRUE, master_execinfo);
	if(!rst)
	{
		return FALSE;
	}
	

	if(enabledouble)
	{
		char command_backuprun[MAX_STRING_BUFS] = {0};	
		char parameter_n[MIN_STRING_BUFS] = {0};
		char parameter_N[MIN_STRING_BUFS] = {0};
		char partition[MIN_STRING_BUFS] = {0};
		char shellpath[MID_STRING_BUFS] = {0};
		getMonitorData("parameter-n", parameter_n);
		getMonitorData("parameter-N", parameter_N);
		getMonitorData("partition", partition);
		getMonitorData("shellpath", shellpath);
		sprintf(command_backuprun, "yhbatch -N %s -n %s --exclusive -p %s %s|&tee %s", 
			parameter_N, parameter_n, partition, shellpath, SUBMIT_LOG);	
		printf("[DEBUG_INFO]: submit backup monitor command: %s\n", command_backuprun);
		rst = system(command_backuprun);
		printf("[DEBUG_INFO]: the result of backup monitor submiting is %d\n", rst);

		serverTCP(WAITBACKUPSTART);	

		messagecount++;
		messagesize = messagesize + sizeof(char)*8;

		//backup_execinfo = malloc(sizeof(MONITOR_EXECINFO));
		if(backup_execinfo == NULL)
		{
			printf("[DEBUG_INFO]: malloc error in function:%s  line:%d\n", __FUNCTION__, __LINE__);
			return FALSE;
		}
		getMonitorExecInfo(FALSE, backup_execinfo);
	}
	return TRUE;
}


int backup_Init(MONITOR_EXECINFO *master_execinfo, MONITOR_EXECINFO *backup_execinfo)
{
	int rst = TRUE;
	//backup_execinfo = malloc(sizeof(MONITOR_EXECINFO));
	if(backup_execinfo == NULL)
	{
		printf("[DEBUG_INFO]: malloc error at %s %d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
        gethostname(backup_execinfo->hostname, MAX_NAME_BUFS);
        printf("My host-name is %s\n", backup_execinfo->hostname);

        struct hostent *hp;
        if((hp = gethostbyname(backup_execinfo->hostname)) == NULL)
        {
                printf("[DEBUG_INFO]: gethostbyname error at %s %d\n", __FUNCTION__, __LINE__);
		return FALSE;
        }
        int i = 0;
        while(hp->h_addr_list[i] != NULL)
        {
                printf("hostname:%s\n", hp->h_name);
                sprintf(backup_execinfo->IP, "%s", inet_ntoa(*(struct in_addr*)hp->h_addr_list[i]));
                printf("ip:%s\n", backup_execinfo->IP);
                i++;
        }

        rst = getLaunchID(SUBMIT_LOG, backup_execinfo->launchID);
	if(rst == FALSE)
	{
		printf("[DEBUG_INFO]: can not get launchID from submit log!\n");
		return FALSE;
	}
        rst = keepIdentity(FALSE,backup_execinfo);
        if(rst == FALSE)
        {
                printf("[DEBUG_INFO]: error in backfillMXml function!\n");
                return FALSE;
        }
	//master_execinfo = malloc(sizeof(MONITOR_EXECINFO));
	getMonitorExecInfo(TRUE, master_execinfo);
	char *message = malloc(sizeof(char)*MIN_STRING_BUFS);
	memset(message, 0, MIN_STRING_BUFS);
	sprintf(message, "%d", STARTWORK);
	clientTCP(master_execinfo->hostname, message);
	if(message != NULL)
	{
		free(message);
	}
	

	printf("[DEBUG_INFO]: The Backup Monitor turn to listening notification from The Master!\n");
	serverTCP(WAITHEARTBEAT);
	printf("[DEBUG_INFO]: The Backup Monitor's serverTCP over!\n ");
	return TRUE;
}



int getMonitorExecInfo(int morb, MONITOR_EXECINFO * meinfo)
{
	xmlDocPtr doc;
	xmlNodePtr rootNode;
	doc = xmlReadFile(MONITOR_INFO, "UTF-8", XML_PARSE_RECOVER);	
	if(NULL == doc)
	{
		printf("[DEBUG_INFO]: error in %s %d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	rootNode = xmlDocGetRootElement(doc);
	if(rootNode == NULL)
	{
		printf("[DEBUG_INFO]: error in %s %d\n", __FUNCTION__, __LINE__);
		xmlFreeDoc(doc);
		return FALSE;
	}
	xmlNodePtr pFirst = rootNode->xmlChildrenNode;
	while(NULL != pFirst)
	{
		if(!xmlStrcmp(pFirst->name, (const xmlChar *)"Master") && (morb == TRUE))
		{
			xmlNodePtr pSecond = pFirst->xmlChildrenNode;
			while(NULL != pSecond)
			{
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)"hostname"))
				{
                                        xmlChar *value = xmlNodeGetContent(pSecond);
					strcpy(meinfo->hostname, value);
					if(!value)
					{
						xmlFree(value);
					}
				}
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)"IP"))
				{
					xmlChar *value = xmlNodeGetContent(pSecond);
					strcpy(meinfo->IP, value);
					if(!value)
					{
						xmlFree(value);
					}
				}
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)"launchID"))
				{
					xmlChar *value = xmlNodeGetContent(pSecond);
					strcpy(meinfo->launchID, value);
					if(!value)
					{
						xmlFree(value);
					}
				}
				pSecond = pSecond->next;
			}
		}
                if(!xmlStrcmp(pFirst->name, (const xmlChar *)"Backup") && (morb == FALSE))
                {
                        xmlNodePtr pSecond = pFirst->xmlChildrenNode;
                        while(NULL != pSecond)
                        {
                                if(!xmlStrcmp(pSecond->name, (const xmlChar *)"hostname"))
                                {
                                        xmlChar *value = xmlNodeGetContent(pSecond);
                                        strcpy(meinfo->hostname, value);
                                        if(!value)
                                        {
                                                xmlFree(value);
                                        }
                                }
                                if(!xmlStrcmp(pSecond->name, (const xmlChar *)"IP"))
                                {
                                        xmlChar *value = xmlNodeGetContent(pSecond);
                                        strcpy(meinfo->IP, value);
                                        if(!value)
                                        {
                                                xmlFree(value);
                                        }
                                }
                                if(!xmlStrcmp(pSecond->name, (const xmlChar *)"launchID"))
                                {
                                        xmlChar *value = xmlNodeGetContent(pSecond);
                                        strcpy(meinfo->launchID, value);
                                        if(!value)
                                        {
                                                xmlFree(value);
                                        }
                                }
                                pSecond = pSecond->next;
                        }
                }
		pFirst = pFirst->next;
	}

	printf("[DEBUG_INFO]: *****Got monitor self info: host:%s, IP:%s, launchID:%s\n", meinfo->hostname, 
		meinfo->IP, meinfo->launchID);	

	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();
	return TRUE;
}


int keepIdentity(int morbflag, MONITOR_EXECINFO *monitor_execinfo)
{	        
        xmlDocPtr doc;
        xmlNodePtr rootNode;
        doc = xmlReadFile(MONITOR_INFO, "UTF-8", XML_PARSE_RECOVER);
        if(NULL == doc)
        {
                fprintf(stderr, "[DEBUG_INFO]:xmlParseFile Error in %s %d\n", __FUNCTION__, __LINE__);
                return FALSE;
        }
        rootNode = xmlDocGetRootElement(doc);
        if(NULL == rootNode)
        {
                fprintf(stderr, "[DEBUG_INFO]:xmlDocGetRootElement Error in %s %d\n", __FUNCTION__, __LINE__);
                xmlFreeDoc(doc);
                return FALSE;
        }
        xmlNodePtr pFirst = rootNode->xmlChildrenNode;
        int flag = FALSE;
        while(NULL != pFirst)
        {
		if(!xmlStrcmp(pFirst->name, (const xmlChar *)("Placeholder")) && (morbflag == TRUE))
		{
			printf("[DEBUG_INFO]: Iam master , I take the placeholder!\n");
			xmlNodeSetContent(pFirst, BAD_CAST "1");
		}	
                if(!xmlStrcmp(pFirst->name, (const xmlChar *)("Master")) && (morbflag == TRUE)) 
                {
                        xmlNodePtr pSecond = pFirst->xmlChildrenNode;
                        while(NULL != pSecond)
                        {
				if(!strcmp(pSecond->name, (const xmlChar *)("hostname")))
				{
					xmlNodeSetContent(pSecond, BAD_CAST monitor_execinfo->hostname);
				}
                                if(!xmlStrcmp(pSecond->name, (const xmlChar *)("launchID")))
                                {
					xmlNodeSetContent(pSecond, BAD_CAST monitor_execinfo->launchID);
                                }
                                if(!xmlStrcmp(pSecond->name, (const xmlChar *)("IP")))
                                {
                                        xmlNodeSetContent(pSecond, BAD_CAST monitor_execinfo->IP);
                                }
                                pSecond = pSecond->next;
                        }
                }
		if(!xmlStrcmp(pFirst->name, (const xmlChar *)("Backup")) && (morbflag == FALSE))	
		{
                        xmlNodePtr pSecond = pFirst->xmlChildrenNode;
                        while(NULL != pSecond)
                        {
                                if(!strcmp(pSecond->name, (const xmlChar *)("hostname")))
                                {
                                        xmlNodeSetContent(pSecond, BAD_CAST monitor_execinfo->hostname);
                                }
                                if(!xmlStrcmp(pSecond->name, (const xmlChar *)("launchID")))
                                {
                                        xmlNodeSetContent(pSecond, BAD_CAST monitor_execinfo->launchID);
                                }
                                if(!xmlStrcmp(pSecond->name, (const xmlChar *)("IP")))
                                {
                                        xmlNodeSetContent(pSecond, BAD_CAST monitor_execinfo->IP);
                                }
                                pSecond = pSecond->next;
                        }

		}	
                pFirst = pFirst->next;
        }
        xmlSaveFormatFileEnc(MONITOR_INFO, doc, "UTF-8", 1);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
	return TRUE;
}


/**
 *=============================================================================================
	update detail data of the monitor.xml according to the master of backup and keyword
	parameter morbflag = TRUE denotes it's filling data of master monitor
		morbflag = FALSE denotes it's filling data of backup monitor
 *=============================================================================================
**/
int backfillMXml(int morbflag, char *keyword, char *keyvalue)
{
        xmlDocPtr doc;
        xmlNodePtr rootNode;
        doc = xmlReadFile(MONITOR_INFO, "UTF-8", XML_PARSE_RECOVER);
        if(NULL == doc)
        {
                fprintf(stderr, "[DEBUG_INFO]:xmlParseFile Error in %s %d\n", __FUNCTION__, __LINE__);
                return FALSE;
        }
        rootNode = xmlDocGetRootElement(doc);
        if(NULL == rootNode)
        {
                fprintf(stderr, "[DEBUG_INFO]:xmlDocGetRootElement Error in %s %d\n", __FUNCTION__, __LINE__);
                xmlFreeDoc(doc);
                return FALSE;
        }
        xmlNodePtr pFirst = rootNode->xmlChildrenNode;
        int flag = FALSE;

        while(NULL != pFirst)
        {
                if(!xmlStrcmp(pFirst->name, (const xmlChar *)("Master")) && (morbflag == TRUE))
                {
                        xmlNodePtr pSecond = pFirst->xmlChildrenNode;
                        while(NULL != pSecond)
                        {
                                if(!strcmp(pSecond->name, (const xmlChar *)(keyword)))
                                {
                                        xmlNodeSetContent(pSecond, BAD_CAST keyvalue);
                                }
                                pSecond = pSecond->next;
                        }
                }
                if(!xmlStrcmp(pFirst->name, (const xmlChar *)("Backup")) && (morbflag == FALSE))
                {
                        xmlNodePtr pSecond = pFirst->xmlChildrenNode;
                        while(NULL != pSecond)
                        {
                                if(!strcmp(pSecond->name, (const xmlChar *)(keyword)))
                                {
                                        xmlNodeSetContent(pSecond, BAD_CAST keyvalue);
                                }
                                pSecond = pSecond->next;
                        }

                }
                pFirst = pFirst->next;
        }
        xmlSaveFormatFileEnc(MONITOR_INFO, doc, "UTF-8", 1);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        return TRUE;
}











