#ifndef _ARL_INTERFACE_H
#define _ARL_INTERFACE_H

#include "TopPublic.h"
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


/**	
 *=====================================================================
 * used for xpath query
 * ====================================================================
 **/
xmlDocPtr getdoc(char *docname);
xmlXPathObjectPtr getnodeset(xmlDocPtr doc, xmlChar *xpath);


int findInXml(char *docname, char *keyword);


/**
 * =======================================================================
 *	get the launchID of Monitors (for master and backup)
 * =======================================================================
**/


int getMonitorID(char *lIDarray, int resultnums);
int createMonitorXml();
int getMonitorData(char *keyword, char *keyvalue);

/**
 *=======================================================================
 	create execuation xml according to the configure file	
 *=======================================================================
 **/
int createExecXml(char *confxml, char *execxml, int *jobnums);

/**
 * =======================================================================	
	generate shell commands need to be launched and store it in memory	
* ========================================================================
**/
int genShCommand(char *confpath, int nums, ARL_job **arl_jobs);

/**
 * ========================================================================
 *	synchnoize the state data in the ARL_job struct to 
 *	the xml file resprents current exection state 
 * ========================================================================
 **/
int updateState(char *exstfile, ARL_job *arljobs, int jobnums);
int updateOneProp(char *targetfile, int jobID, char* keyword, char* keyvalue);

/**
 * ========================================================================
 *	get value from execInfo.xml file with specified ID
 * ========================================================================
 **/
int getRecordInXml(int jobID, char *keyword, char *keyvalue);



/**
 * =======================================================================
 * 	get switch from configure.xml
 * =======================================================================
**/
int getDMS(char *docname, char *keyword);




/**
 *=========================================================================
	check the realtime state of the jobs
**=========================================================================
**/
int stateCheck(ARL_job *arljobs, int jobnums);
long getruntime(char *runtime);

/**
 * =========================================================================
 *	set a timer for periodcally checking
 * =========================================================================
 * */	
void addTimer(int timeout);
void setTimer(int timeout);



/**
 * =========================================================================
 *	create a filefolder for shellfiles and copy one shell file for
 *	each job needs to be launched
 * =========================================================================
 **/
int mkdirforshell(char *shellfolder);

/**
 * =========================================================================
 *	crate a separated shell file for each job and add "-ID XX" 
 *	for translating identification to the job
 * =========================================================================
 **/
int getOwnShell(char *originshell, char *id, char *finalshell);


/**
 * =========================================================================
 * 	used for submit the job or resubmit when it needs rerun
 * 	getlaunchID used for get launchID after submit the job to the PBS
 * =========================================================================
**/
int getLaunchID(char *file, char *id);
int jobSubmit(ARL_job *arljobs, int jobnums);

int stateSynToReport(int jobID, long oldlaunchID, long newlaunchID);
int reSubmit(ARL_job *arljobs, int jobnums, int jobID);


int monitor_Init(int * morbflag, MONITOR_EXECINFO *master_execinfo, MONITOR_EXECINFO *backup_execinfo);



int serverTCP(int type);
int clientTCP(char* hostName, char *message);

#endif



















