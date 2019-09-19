#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "interface.h"

extern int enablecheck;
extern int aliveflag;
extern int watchflag;

extern int finishflag;

void addTimer();
int SIGProcess();
void setTimer();

static void SIGHandler(int sig);
static void checkAlive(int sig);

static struct itimerval oldtv;
static struct sigaction sa;
static struct itimerval itv;


/*
void main()
{
	addTimer(10);
	setTimer(30);
	while(1)
	{}
}
*/

void addTimer(int timeout)
{
	printf("[DEBUG_INFO]:Now add a timer to the monitor!\n");
	SIGProcess();
	setTimer(timeout);	
}

int SIGProcess()
{
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIGHandler;
	if(sigaction(SIGALRM, &sa, NULL) == -1)
	{
		printf("[DEBUG_INFO]:sigaction error!\n");
		exit(-1);	
	}
}

void setTimer(int timeout)
{
	itv.it_interval.tv_sec = timeout;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = timeout;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &itv, &oldtv);
}

static void SIGHandler(int sig)
{
	printf("Knock-Knock, timeout!\n");
	enablecheck = TRUE;
	printf("[DEBUG_INFO]:enablecheck is %d in timer handler!\n", enablecheck);
}


void addTimer_CheckAlive(int timeout)
{
	printf("[DEBUG_INFO]: Now add a timer for checking master alive!\n");
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = checkAlive;
	if(sigaction(SIGALRM, &sa, NULL) == -1)
	{
		printf("[DEBUG_INFO]: sigaction error!\n");
		exit(-1);
	}
	setTimer(timeout);
}

static void checkAlive(int sig)
{	
	printf("[DEBUG_INFO]: Knock-Knock, timeout!\n");
	if(aliveflag == TRUE)
	{
		printf("[DEBUG_INFO]: currently, master monitor is alive!\n");
		aliveflag = FALSE;
	}
	else
	{
		printf("[DEBUG_INFO]: currently, master monitor most is possible to be dead!\n");
		watchflag = FALSE;
		finishflag = FALSE;
	}
}




