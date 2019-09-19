#include <stdio.h>
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

int clientTCP(char* hostName, char* message);

void main();
{
	int nums = 10;
	int i = 0;
	for(i = 0; i < nums; i++)
	{
		clientTCP();
	}
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


