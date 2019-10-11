#include <mpi.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int recvID(int argc, char * argv[])
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
					return id;
				}
				else
				{
					printf("[DEBUG_INFO]: missing command line parameter -ID!\n");
					return id;
				}
			}
		}
	}
	return id;
}

int main(int argc, char * argv[])
{
	recvID(argc, argv);
int nproc;
int iproc;
char proc_name[MPI_MAX_PROCESSOR_NAME];
int nameLength;
int result = 0;
int i;

MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &nproc);
MPI_Comm_rank(MPI_COMM_WORLD, &iproc);

MPI_Get_processor_name(proc_name, &nameLength);
for(i=0; i < 100; i++)
{
	result += 1;
	printf("Hello World, I am host %s with rank %d of%d\n", proc_name, iproc, nproc);
	printf("%d\n", result);
	sleep(1);
}

MPI_Finalize();
return 0;
}
