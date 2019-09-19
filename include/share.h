#ifndef	_SHARE_H
#define	_SHARE_H

#define TRUE    1
#define FALSE   0

/*-------------------------------------------------
 *                 Defines
 *-------------------------------------------------
*/
/**======= max size of the buffer for a file name or files  ============**/
#define MAX_NAME_BUFS           256

/**========== max size of one string for a short name or words =========**/
#define MAX_STRING_BUFS         256
#define MID_STRING_BUFS         128
#define MIN_STRING_BUFS         32


#define TIMEOUT                 60


/**====================== application state code =======================**/

#define UNRUN           "0000"
#define RUNNING         "0001"
#define COMPLETED       "1111"
#define RERUN           "0010"
#define FAILURE         "0100"
#define TERMINATED      "1000"
#define CANCELED        "1001"
#define SUBMITTED       "1010"

/**==================== default file name agreed ======================**/
#define CONF_NAME       "configure.xml"
#define EXEC_INFO       "execInfo.xml"
#define MONITOR_INFO	"monitor.xml"
#define SUBMIT_LOG      "jobsubmit.log"
#define QUERY_LOG	"jobquery.log"

/**=============== default filefolder for all shell copys =============**/
#define SHELLFOLDER     "./shellfiles"


/**===== different types of tcp communication for different using ======**/
#define	WAITBACKUPSTART		1
#define	WAITHEARTBEAT		0
#define	STARTWORK		2
#define	HEARTBEAT		3
#define WORKOVER		6



/**
=========================================================================
	dimensions in the log of states query need to be recorded	
	the JOBID here actually is the launchID we defined and 
 	 different from the jobID we meant in the program		
=========================================================================
**/
#define	Dime_launchID	"JOBID" 
#define Dime_state	"ST"
#define	Dime_nodes	"NODES"
#define Dime_time	"TIME"




/**
==========================================================================
	properities name in configure.xml
==========================================================================
**/
#define DMSWITCH	"switch"




#endif
