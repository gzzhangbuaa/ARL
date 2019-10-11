#include <mpi.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "arltoolapi.h"


int main(int argc, char * argv[])
{
	int nproc;
	int iproc;
	char proc_name[MPI_MAX_PROCESSOR_NAME];
	int nameLength;
	int result = 0;
	int i;

	MPI_Init(&argc, &argv);


	ARL_Init(argc, argv);
	printf("[DEBUG_INFO]:jobID is %d, leaderflag bool value %d\n", jobID, leaderflag);		

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
	ARL_Finalize();
	printf("[DEBUG_INFO]: ARL_Finalize function complete!\n");
	return 0;
}
