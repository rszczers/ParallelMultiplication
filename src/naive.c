#include "mpich/mpi.h"

int naive(double *A, double *B, double *C, int m, int k, int n, int lenght) {
	int pid;
	int numprocs;
	MPI_Status status;
	MPI_Comm cartcom;

	int dim[2];
	int period[2];
	int coord[2];

	MPI_Init(NULL, NULL);

	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	MPI_Cart_create(MPI_COMM_WORLD, 2, dim, period, 1, &cartcom);
	MPI_Comm_rank(&cartcom, &pid);

	MPI_Finalize();
	return EXIT_SUCCESS;
}