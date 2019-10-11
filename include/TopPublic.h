#ifndef	_TOPPUBLIC_H
#define _TOPPUBLIC_H

#include <mpi.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/resource.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "share.h"


/** @typedef	ARL_job	
 *  @brief	job information used for launch
 *  This type stores the initiation data in the configure file
**/

typedef struct ARL_job
{
	int	id;
	int	parameter_n;
	int	parameter_N;
	char	partition[MIN_STRING_BUFS];
	char	shellpath[MID_STRING_BUFS];
	char	state[MIN_STRING_BUFS];
	char	command[MAX_STRING_BUFS];
	long	launchID;
	long  	limittime;
} ARL_job;


typedef struct QUERY_dimension
{
	long	launchID;
	char	state[MIN_STRING_BUFS];
	int	nodesnum;
	char	runtime[MIN_STRING_BUFS];
} QUERY_dimension;


typedef struct MONITOR_EXECINFO
{
	char	hostname[MIN_STRING_BUFS];
	char	IP[MIN_STRING_BUFS];
	char	launchID[MIN_STRING_BUFS];
} MONITOR_EXECINFO;

#endif




