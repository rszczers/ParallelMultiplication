//
// Created by rszczers on 21.06.15.
//

#include "deadlock.h"
#include <mpich/mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    int myid, numprocs, i;
    MPI_Status stat;
    double a, b;

    MPI_Request s_req, r_req;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    a = (double)myid;

    MPI_Isend(&a, 1, MPI_DOUBLE, (myid+1)%numprocs, 100, MPI_COMM_WORLD, &s_req);
    MPI_Irecv(&a, 1, MPI_DOUBLE, (myid-1+numprocs)%numprocs, 100, MPI_COMM_WORLD, &r_req);

    MPI_Wait(&r_req, &stat);

    printf("id = %d a = %lf\n", myid, a);

}


