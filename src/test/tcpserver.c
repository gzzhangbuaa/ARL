nclude <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include "interface.h"

#define BACKLOG 50
#define PORT_NUM        "50000"

int aliveflag = FALSE;
int watchflag = TRUE;

int serverTCP(int type)
{
        struct sockaddr_storage claddr;
        int lfd, cfd, optval, reqLen;
        socklen_t addrlen;
        struct addrinfo hints;
        struct addrinfo *result, *rp;

        #define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
        char addrStr[ADDRSTRLEN];
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        {
                printf("[serverTCP-001]: errExit signal\n");
                exit(-1);
        }

        printf("[serverTCP-001]: set singnal ignoral done!\n");
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_canonname = NULL;
        hints.ai_addr = NULL;
        hints.ai_next = NULL;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = AF_UNSPEC;
        hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

        char hostname[32] = {0};
        gethostname(hostname, sizeof(hostname));

        if(getaddrinfo(hostname, PORT_NUM, &hints, &result) != 0)
        {
                printf("[DEBUG_INFO]: errExit, getaddrinfo \n");
                return -1;
        }

        printf("[DEBUG_INFO]: getaddrinfo done! \n");

        optval = 1;
        int count = 0;
        for(rp = result; rp != NULL; rp = rp->ai_next)
        {       count++;
                printf("check result of getaddrinfo count is %d\n", count);
                lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
                if(lfd == -1)
                {
                        printf("[DEBUG_INFO]: socket failed!, next iteration!\n");
                        continue;
                }
                printf("[DEBUG_INFO]: socket done! \n");

                if(setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
                {
                        printf("[DEBUG_INFO]: errExit, setsockopt \n");
                        return -1;
                }
                printf("[DEBUG_INFO]: setsockopt done! \n");

                if(bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0)
                {
                        printf("[DEBUG_INFO]: bind done! and break loop! \n");
                        break;
                }
                printf("[serverTCP-005]: bind failed!\n");

                close(lfd);
        }

        if(rp == NULL)
        {
                printf("[DEBUG_INFO]: errExit! Could not bind socket to any address \n");
                return -1;
        }
        if(listen(lfd, BACKLOG) == -1)
        {
                printf("[DEBUG_INFO]: errExit! listen failed!\n");
                return -1;
        }

        printf("[DEBUG_INFO]: listen done!\n");

        freeaddrinfo(result);


	if(type == WAITHEARTBEAT)	
	{
		while(watchflag == TRUE)
		{
	                addrlen = sizeof(struct sockaddr_storage);
	                cfd = accept(lfd, (struct sockaddr *) &claddr, &addrlen);
	                if(cfd == -1)
	                {
	                        printf("[DEBUG_INFO]: accept error! continue to accept the next connect request! \n");
	                        return -1;
	                }
	                printf("[DEBUG_INFO]: accept one coonection done! \n");
	                if(getnameinfo((struct sockaddr *) &claddr, addrlen, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	                {
	                        snprintf(addrStr, ADDRSTRLEN, "(%s, %s)", host, service);
	                }
	                else
	                {
	                        snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");
	                }
	                printf("Connection from %s \n", addrStr);


	                char* rb = malloc(sizeof(char)*MIN_STRING_BUFS);
	                int len = recv(cfd, rb, MIN_STRING_BUFS, 0);
	                printf("[@@@@ Received: %s, len is %d @@@@]\n", rb, len);
			if(atoi(rb) == 1)
			{
				printf("[DEBUG_INFO]: this message is STARTWORK denotes the master monitor has been ready!\n");
				printf("[DEBUG_INFO]: add a local timer for inspecting the master monitor is alive or not!\n");
				addTimer_CheckAlive(2*TIMEOUT);
			}
			if(atoi(rb) == 2)
			{
				printf("[DEBUG_INFO]: this message is HEARTBEAT denotes the master is aliving!\n");
				aliveflag = TRUE;
			}
			if(atoi(rb) == 3)
			{
				printf("[DEBUG_INFO]: this message is WORKOVER denotes regular work has been completed!\n");
				watchflag = FALSE;
			}
	                free(rb);
	                close(cfd);
			
		}
	}
      
        close(lfd);
}

int clientTCP(char* hostName, char *message)
{
        int cfd;
        ssize_t numRead;
        struct addrinfo hints;
        struct addrinfo *result, *rp;

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_canonname = NULL;
        hints.ai_addr = NULL;
        hints.ai_next = NULL;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_NUMERICSERV;

        if(getaddrinfo(hostName, PORT_NUM, &hints, &result) != 0)
        {
                printf("[DEBUG_INFO]: errExit! getaddrinfo \n");
                return -1;
        }
        printf("[DEBUG_INFO]: getaddrinfo done! \n");
        for(rp = result; rp != NULL; rp = rp->ai_next)
        {
                cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
                if(cfd == -1)
                {
                        printf("[DEBUG_INFO]: socket failed! continue to try next socket!\n");
                        continue;
                }
                printf("[DEBUG_INFO]: socket done! \n");
                if(connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1)
                {
                        printf("[DEBUG_INFO]: connect done! break the loop.\n");
                        break;
                }
                printf("[DEBUG_INFO]: connect failed!\n");
                close(cfd);
                printf("[DEBUG_INFO]: close bad socket descriptor!\n");
        }

        if(rp == NULL)
        {
                printf("[DEBUG_INFO]: fatal, Could not connect socket to any address\n");
                return -1;
        }

        printf("[DEBUG_INFO]: this area send and receive messages\n");

        int len = 0;
        char* sb = malloc(sizeof(char)*MIN_STRING_BUFS);
	memset(sb, 0, MIN_STRING_BUFS);
        sprintf(sb, "%s", message);
        len = send(cfd, sb, MIN_STRING_BUFS, 0);
        printf("[@@@@ send result is %s length is %d\n", sb, len);
        free(sb);

        freeaddrinfo(result);
        close(cfd);
        return 0;
}


