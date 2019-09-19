#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int debug;

void show_version(char* name)
{
	printf("%s by zgz, version: 1.0\n", name);
}

void usage(char *name)
{
	show_version(name);
	printf("	-h,	--help		short help\n");
	printf("	-v	--version	show version\n");
}

int main(int argc, char *argv[])
{
	int i = 0;
	if(argc > 1)
	{
		for(i = 1; i < argc; i++)
		{
			if((strcmp(argv[i], "-D")==0) || strcmp(argv[i], "--debug")==0)
			{
				debug = 1;
			}
		}
	}
	
	for(i = 1; i < argc; i++)
	{
		if(debug)
		{
			printf("arg %d: \"%s\"\n", i, argv[i]);
		}		
		if((strcmp(argv[i], "-h")==0) || (strcmp(argv[i], "--help")==0))
		{
			usage(argv[i]);
			return 0;
		}
		else if((strcmp(argv[i], "-D")==0) || (strcmp(argv[i], "--debug")==0))
		{
			debug = 1;
		}
		else if((strcmp(argv[i], "fpga")==0))
		{
			printf("test of fpga...\n");
		}
		else if((strcmp(argv[i], "-i")==0) || (strcmp(argv[i], "--iface")==0))
		{
			if(i+1<argc)
			{
				char interface[32]={0};
				strncpy(interface, argv[i+1], 32);
				if(debug)
				{
					printf("Used interface: %s\n", interface);
				}
				i++;
				continue;
			}
			else
			{
				printf("Error: Interface for -i missing.\n");
				return 1;
			}
		}
		else if((strcmp(argv[i], "-ru")==0) || (strcmp(argv[i], "--rateunit"))==0)
		{
			if(i+1<argc && isdigit(argv[i+1][0]))
			{
				int rateunit = 0;
				rateunit = atoi(argv[i+1]);
				if(rateunit < 0 || rateunit > 1)
				{
					printf("Error: Invalid parameter \"%d\" for --rateunit.\n", rateunit );
					printf("Valid parameters:\n");
					printf("	0 - bytes\n");
					printf("	1 - bits\n");
					return 1;
				}
				if(debug)
				{
					printf("Rateunit changed: %d\n", rateunit);
				}
				i++;
				continue;
			}
			else
			{}
		}
		else if(strcmp(argv[i], "--enable")==0)
		{
			int enable = 0;
			enable = 1;
		}
		else
		{
			printf("Unknown parameter \"%s\". Use --help for help.\n", argv[i]);
			return 1;
		}
	}



}






















