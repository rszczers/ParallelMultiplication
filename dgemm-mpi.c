#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "cblas.h"
#include "mpi.h"

#define USE_MPI
#define RECV_STREAM_TAG 1

/* 
Matrix Matrix Multiplication

C <-- alpha * op(A)op(B) + beta * C

*/

void random_matrix(double *aa, int nn, char *filename);

int main(int argc, char *argv[]) {
   int n;
   double alpha=2.0, beta = 0.5;
   double *a, *b, *c1, *c2;
   clock_t start, finish;
   double elapsed, sum, diff, max_diff;
   int i, j, k;
   int size;
   int numrows;
   FILE *fp;

   int myid, nprocs;
   MPI_Status status;

   /*************************** MPI calls ***********************************/
    
   MPI_Init(&argc, &argv);       /* Initialize MPI                          */
   MPI_Comm_rank(MPI_COMM_WORLD, &myid); /* find process id                 */
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs); /* find number of processes      */

   /* root node */    
   if (myid == 0) {
      printf("Type in the row of the matrix:  ");
      scanf("%d", &n);
      size=n*n;

      a=(double *)calloc(size,sizeof(double));
      if (a == NULL) {
         printf ("Not enough memory!\n");   
         exit(0);
      }
      b=(double *)calloc(size,sizeof(double));
      if (b == NULL) {
         printf ("Not enough memory!\n");   
         exit(0);
      }

      printf ("Master node\n");
      printf("Generate random matrix 1 (%d x %d)\n",n,n);
      random_matrix(&a[0], size, "mat1.dat");
      printf("Generate random matrix 2 (%d x %d)\n",n,n);
      random_matrix(&b[0], size, "mat2.dat");
      free(a);
      free(b);
   }
   MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);

   start = clock();
   if (myid == 0) {
      numrows = n;
      printf ("node = %d\n",myid);
      printf("size of matrix 1 is: %d\n",size);	   
   }
   else {
      printf ("node = %d\n",myid);

      numrows = ceil((double) n/(nprocs-1));
      if (myid == (nprocs -1)) {
         numrows = n - numrows*(nprocs-2);
      }
      size = n*numrows;
      /*
      printf ("number of rows = %d\n",numrows);
      */
      printf("size of matrix 1 is: %d\n",size);	   
   }

   a=(double *)calloc(size,sizeof(double));
   if (a == NULL) {
      printf ("Not enough memory!\n");   
      exit(0);
   }
   b=(double *)calloc(n*n,sizeof(double));
   if (b == NULL) {
      printf ("Not enough memory!\n");   
      exit(0);
   }
   c1=(double *)calloc(n*n,sizeof(double));
   if (c1 == NULL) {
      printf ("Not enough memory!\n");   
      exit(0);
   }
   c2=(double *)calloc(n*n,sizeof(double));
   if (c2 == NULL) {
      printf ("Not enough memory!\n");   
      exit(0);
   }
   fp = fopen("mat2.dat","r");
   if (fp == NULL) {
      fprintf(stderr,"ERROR opening file\n");
      exit(1);
   }
   fread(b, size, sizeof(double), fp);
   fclose(fp);
   fp = fopen("mat1.dat","r");
   if (fp == NULL) {
      fprintf(stderr,"ERROR opening file\n");
      exit(1);
   }
   if (myid == 0) {
      fread(a, size, sizeof(double), fp);
   }
   else {
      fseek(fp, (myid-1)*sizeof(double)*ceil((double) n/(nprocs-1)), SEEK_SET);
      fread(a, size, sizeof(double), fp);
   }
   fclose(fp);
/*
   for (i=0;i<numrows;i++) {
      for (j=0;j<n;j++) {
         printf("%f ",a[i+j]);
      }
      printf("\n");
   }
*/ 
   for (i=0; i<n*n; i++) {
      c1[i] = 0.0;
      c2[i] = 0.0;
   }

   printf("call cblas_dgemm\n");
   cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, numrows, n, n, alpha, 
		  &a[0], n, &b[0], n, beta, &c1[0], n);
   printf("leave cblas_dgemm\n");
   finish = clock();
   elapsed = (double)( finish - start ) / CLOCKS_PER_SEC;
   printf("CPU time used by dgemm = %f seconds\n",elapsed);

   free(a);
   free(b);
   free(c1);
   free(c2);

   MPI_Barrier(MPI_COMM_WORLD);

   if (myid == 0) {
   system ("rm -f mat1.dat");
   system ("rm -f mat2.dat");
   }

   MPI_Finalize();
}

void random_matrix(double* aa, int nn, char *filename)
{
   int i;
   FILE *fp;

   fp = fopen(filename,"w");
   if (fp == NULL) {
      fprintf(stderr,"Could not open file %s for writing\nCheck path or permissions\n", filename);
      exit(1);
   }
   for (i=0; i<nn; i++) {
      aa[i] = (double)( rand() ) / RAND_MAX;
   }
   fwrite(aa,nn,sizeof(double),fp);
   fclose(fp);
}
