
extern int leaderflag;
extern int jobID;


int MY_Init(int *argc, char ***argv);
int MY_Finalize(void);

#define MPI_Init	MY_Init
#define MPI_Finalize	MY_Finalize

