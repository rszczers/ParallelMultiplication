#include <mpi.h>
#include <stdio.h>
#include <stddef.h>
#include <math.h>

int naive(double *A, double *B, double *C, int m, int k, int n) {
	int pid;
	int numprocs;
	MPI_Status stat;
	MPI_Comm cartcom;

	int dim[2];
	int period[2];
	int coord[2];

	MPI_Init(NULL, NULL);

	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

	dim[0] = sqrt(numprocs);
	dim[1] = sqrt(numprocs);
	period[0] = 1;
	period[1] = 1;

	MPI_Cart_create(MPI_COMM_WORLD, 2, dim, period, 0, &cartcom);
	MPI_Cart_coords(cartcom, pid, 2, coord);

	printf("Rank = %d (%d, %d)\n", pid, coord[0], coord[1]);
	MPI_Finalize();
	return 0;;
}
