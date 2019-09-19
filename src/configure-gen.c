#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "interface.h"


#define CONF_NAME  "configure.xml"
#define EXEC_INFO  "execInfo.xml"
#define MONITOR_INFO  "monitor.xml"



int main(int argc, char **argv)
{
	createConfXml();
	/*
	int jobnums = 0;
	createExecXml(CONF_NAME, EXEC_INFO, &jobnums);
	printf("[DEBUG_INFO]:jobnums is %d\n", jobnums);
	*/
	createMonitorXml();	
}

/**	create a initial xml file for initiation	**/
int createConfXml()
{
	
	xmlKeepBlanksDefault(0);
	xmlIndentTreeOutput = 1;
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL, jobs_node = NULL, switch_node = NULL;
	doc = xmlNewDoc(BAD_CAST "1.0");


	root_node = xmlNewNode(NULL, BAD_CAST "configure");
	xmlDocSetRootElement(doc, root_node);
	
	switch_node = xmlNewChild(root_node, NULL, BAD_CAST "switch", BAD_CAST "0");
	//jobs_node = xmlNewChild(root_node, NULL, BAD_CAST "jobs", BAD_CAST "content of jobs");
	
	jobs_node = xmlNewChild(root_node, NULL, BAD_CAST "job", NULL);
	xmlNodePtr para_n_node = NULL, para_N_node = NULL, jobshell_node = NULL, para_p_node = NULL;
	xmlNodePtr job_ID_node = NULL, job_launchID_node = NULL, state_node = NULL, limit_time_node;
	
	/**	preset before program launch	**/
	job_ID_node = xmlNewChild(jobs_node, NULL, BAD_CAST "job-ID", BAD_CAST "0");
	para_N_node = xmlNewChild(jobs_node, NULL, BAD_CAST "parameter-N", BAD_CAST "1");
	para_n_node = xmlNewChild(jobs_node, NULL, BAD_CAST "parameter-n", BAD_CAST "4");
	para_p_node = xmlNewChild(jobs_node, NULL, BAD_CAST "parameter-p", BAD_CAST "paratera");
	jobshell_node = xmlNewChild(jobs_node, NULL, BAD_CAST "jobshell-Path", BAD_CAST "./hellolaunch.sh");

	/**	set an expect time for each job	**/
	limit_time_node = xmlNewChild(jobs_node, NULL, BAD_CAST "limit-time", BAD_CAST "00:00:00:00");
	
	/**	created when the job is submitted successfully	**/
	job_launchID_node = xmlNewChild(jobs_node, NULL, BAD_CAST "job-launchID", BAD_CAST "0");
	state_node = xmlNewChild(jobs_node, NULL, BAD_CAST "job-state", BAD_CAST "0");
	/*	
	para_n_node = xmlNewNode(NULL, BAD_CAST "parameter-n");
	para_N_node = xmlNewNode(NULL, BAD_CAST "parameter-N");
	execPath_node = xmlNewNode(NULL, BAD_CAST "execPath");
	
	xmlAddChild(jobs_node, para_n_node);
	xmlAddChild(jobs_node, para_N_node);
	xmlAddChild(jobs_node, execPath_node);
	*/
	
	xmlSaveFormatFileEnc(CONF_NAME, doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();
	return (0);
}

int createMonitorXml()
{

        xmlKeepBlanksDefault(0);
        xmlIndentTreeOutput = 1;
        xmlDocPtr doc = NULL;
        xmlNodePtr root_node = NULL, jobs_node = NULL, switch_node = NULL;
        doc = xmlNewDoc(BAD_CAST "1.0");
        root_node = xmlNewNode(NULL, BAD_CAST "Monitor");
        xmlDocSetRootElement(doc, root_node);

        xmlNodePtr placeholder_node = NULL;
        placeholder_node = xmlNewChild(root_node, NULL, BAD_CAST "Placeholder", BAD_CAST "0");


        xmlNodePtr setup_node = NULL, parameter_n_node = NULL, parameter_N_node = NULL, partition_node = NULL;
        xmlNodePtr shellpath_node = NULL, master_node = NULL, backup_node = NULL;

        setup_node = xmlNewChild(root_node, NULL, BAD_CAST "Setup", NULL);
        parameter_n_node = xmlNewChild(setup_node, NULL, BAD_CAST "parameter-n", BAD_CAST "1");
        parameter_N_node = xmlNewChild(setup_node, NULL, BAD_CAST "parameter-N", BAD_CAST "1");
        partition_node = xmlNewChild(setup_node, NULL, BAD_CAST "partition", BAD_CAST "paratera");
        shellpath_node = xmlNewChild(setup_node, NULL, BAD_CAST "shellpath", BAD_CAST "./autoReLaunch.sh");

        master_node = xmlNewChild(root_node, NULL, BAD_CAST "Master", NULL);
        backup_node = xmlNewChild(root_node, NULL, BAD_CAST "Backup", NULL);

        xmlNodePtr mhost_node = NULL, mIP_node = NULL, mlaunchID_node = NULL;
        xmlNodePtr bhost_node = NULL, bIP_node = NULL, blaunchID_node = NULL;
        mhost_node = xmlNewChild(master_node, NULL, BAD_CAST "hostname", NULL);
        mIP_node = xmlNewChild(master_node, NULL, BAD_CAST "IP", NULL);
        mlaunchID_node = xmlNewChild(master_node, NULL, BAD_CAST "launchID", NULL);

        bhost_node = xmlNewChild(backup_node, NULL, BAD_CAST "hostname", NULL);
        bIP_node = xmlNewChild(backup_node, NULL, BAD_CAST "IP", NULL);
        blaunchID_node = xmlNewChild(backup_node, NULL, BAD_CAST "launchID", NULL);

        xmlSaveFormatFileEnc(MONITOR_INFO, doc, "UTF-8", 1);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        return (0);
}


/*
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

*/

