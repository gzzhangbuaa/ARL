#include "mpi.h"
#include "arltoolapi.h"

int MY_Init(int *argc, char ***argv)
{
	MPI_Init(argc, argv);
	ARL_Init(*argc, *argv);
}

int MY_Finalize(void)
{
	MPI_Finalize();
	ARL_Finalize();
}
