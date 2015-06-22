//
// Created by rszczers on 21.06.15.
//


#include "cblas.h"
#include "cblas_f77.h"
#include <mpich/mpi.h>
#include <stdio.h>
#include <malloc.h>

int main(int argc, char *argv[]) {
    int myid, myid2d, numprocs, i, j, ndims, newid1, newid2; double tim;
    MPI_Status stat;
    MPI_Comm cartcom, splitcom1, splitcom2;
    MPI_Request reqs[4];

    int dims[2];
    int pers[2];
    int coord[2];
    int rem[2];

    int p, q, n;
    double *a;
    double *b;
    double *c;
    double *a_buff[2], *b_buff[2];

    int up, down, left, right, shiftsource, shiftdest;

    double ALPHA = 1.0, BETA = 1.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if(myid==0) {
        int k = scanf("%d", &n);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    ndims = 2;
    dims[0] = 0;
    dims[1] = 0;
    pers[0] = 1;
    pers[1] = 1;

    MPI_Dims_create(numprocs, ndims, dims);
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, pers, 1, &cartcom);
    MPI_Comm_rank(cartcom, &myid2d);
    MPI_Cart_coords(cartcom, myid2d, 2, coord);

    p = dims[0];
    int myrow = coord[0];
    int mycol = coord[1];

    MPI_Cart_shift(cartcom, 1, -1, &right, &left);
    MPI_Cart_shift(cartcom, 0, -1, &down, &up);

    //tablice

    q = n/p;
    a = (double *)malloc(q*q*sizeof(a));
    b = (double *)malloc(q*q*sizeof(b));
    c = (double *)malloc(q*q*sizeof(c));

    a_buff[0] = a;
    a_buff[1] = (double *)malloc(q*q*sizeof(a));

    b_buff[0] = b;
    b_buff[1] = (double *)malloc(q*q*sizeof(b));

    for (i = 0; i < q; ++i) {
        for (j = 0; j < q; ++j) {
            a[i * q + j] = myid;
            b[i * q + j] = myid;
            c[i + q + j] = 0;
        }
    }

    if(myid2d == 0) tim = MPI_Wtime();

    MPI_Cart_shift(cartcom, 1, 1-coord[0], &shiftsource, &shiftdest);

    MPI_Sendrecv_replace(a_buff[0], q*q, MPI_DOUBLE, shiftdest, 101, shiftsource, 101, cartcom, &stat);

    MPI_Cart_shift(cartcom, 1, 1-coord[0], &shiftsource, &shiftdest);

    MPI_Sendrecv_replace(b_buff[0], q*q, MPI_DOUBLE, shiftdest, 101, shiftsource, 101, cartcom, &stat);


    //iloczyny lokalne

    for (i = 0; i < dims[0]; ++i) {
        MPI_Isend(a_buff[i%2], q*q, MPI_DOUBLE, left, 1, cartcom, &reqs[0]);
        MPI_Isend(b_buff[i%2], q*q, MPI_DOUBLE, left, 1, cartcom, &reqs[1]);
        MPI_Isend(a_buff[i%2], q*q, MPI_DOUBLE, left, 1, cartcom, &reqs[2]);
        MPI_Isend(b_buff[i%2], q*q, MPI_DOUBLE, left, 1, cartcom, &reqs[3]);

        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, q, q, q, ALPHA, a, q, b, q, BETA, c, q);
        for(j = 0; j < 4; j++) {
            MPI_Wait(&reqs[j], &stat);
        }
    }

    // rozmieszczenie startowe aij bij

    MPI_Cart_shift(cartcom, 1, 1 + coord[0], &shiftsource, &shiftdest);
    MPI_Sendrecv_replace(a_buff[i % 2], q*q, MPI_DOUBLE, shiftdest, 1, shiftsource, 1, cartcom, &stat);
    MPI_Cart_shift(cartcom, 0, coord[1], &shiftsource, &shiftdest);

    MPI_Sendrecv_replace(b_buff[i%2], q*q, MPI_DOUBLE, shiftdest, 1, shiftsource, 1, cartcom, &stat);

    if(myid2d == 0) {
        tim = MPI_Wtime() - tim;
        printf("Czas = %lf\n", tim);
     //   printf("Mflops = %lf\n", ( 2.0*n )*n*(n/1.0e + 6.0)/tim);
    }

    MPI_Comm_free(&cartcom);
    MPI_Finalize();
    return 0;
}
