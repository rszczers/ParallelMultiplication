#define ROOT 0
#define DISTRIBUTION 1337
#define SKEW 23
#define COLLECTING 42

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <argp.h>
#include <string.h>
#include <time.h>

#include <mkl.h>
#include <math.h>

#include <mpi.h>

#include "load_matrix.h"
#include "save_matrix.h"
#include "save_info.h"

const char *argp_program_version = "pmm v0.1";
const char *argp_program_bug_address = "<rafal.szczerski@gmail.com>";
static char doc[] = "Parallel matrix multiplication with Intel Math Kernel Library";
static char args_doc[] = "-A matrixA_path -B matrixB_path -m NUM -k NUM -n NUM";

static struct argp_option options[] = {
    { "method", 'a', "METHOD", 0, "Algorithm used."},
    { "inputA", 'A', "FILE", 0, "Path to input FILE containing matrix A data."},
    { "inputB", 'B', "FILE", 0, "Path to input FILE containing matrix B data."},
    { "output", 'o', "FILE", OPTION_ARG_OPTIONAL, "Path to output FILE containing matrix C=A*B data."},
    { 0 , 'C', "FILE", OPTION_ALIAS, 0},
    { 0, 'm', "NUM", 0, "Number of rows of A."},
    { 0, 'k', "NUM", 0, "Number of rows of B."},
    { 0, 'n', "NUM", 0, "Number of columns of B."},
    { "list", 'l', 0, 0, "Show list of available algorithms."},
    { "time", 't', 0, 0, "Show elapsed time."},
    { "quiet", 'q', 0, 0, "Do not show any computations."},
    { "verbose", 'v', 0, 0, "Show all computations."},
    { 0 } 
};

struct arguments {
    enum {SEQUENTIAL, MKL, CANNON} method;
    enum {QUIET, VERBOSE} mode;
    int m, k, n;
    char *pathA;
    char *pathB;
    char *pathC;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {        
        case 'a':
        {           
			char name[11];
            for (int i = 0; i<11 ; arg++, i++)
                name[i] = tolower(*arg);

            if(strcmp("mkl", name) == 0)
                arguments->method = MKL;
            else if(strcmp("sequential", name) == 0)
                arguments->method = SEQUENTIAL;
            // cannon is set as default

            break;
        }
        case 'A': {
            arguments->pathA = arg;
            break;
        }
        case 'B': {
            arguments->pathB = arg;
            break;
        }
		case 'C':
        case 'o': {
            arguments->pathC = arg;
            break;
		}
        case 'l':
        {
            printf("Available methods:\n");
            char *availableMethods[] = {"sequential", "MKL", "cannon"};
            for (int i = 0; i <= CANNON; ++i)
                printf("%d - %s\n", i, availableMethods[i]);
            break;            
        }
        case 'm':
            arguments->m = atoi(arg);
            break;
        case 'n':
            arguments->n = atoi(arg);
            break;
        case 'k':
            arguments->k = atoi(arg);
            break;
        case 'q':
            arguments->mode = QUIET;
            break;
        case 'v':
            arguments->mode = VERBOSE;
            break;
        case ARGP_KEY_ARG:
            break;
        case ARGP_KEY_INIT:
            arguments->method = CANNON;
            arguments->mode = QUIET;
            arguments->pathC = NULL;
            break;
        case ARGP_KEY_END:
//            if(state->arg_num > 0)
//				argp_usage(state);
            break;
        default: return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char *argv[]) {
	struct arguments arguments;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	
	int m = arguments.m;
	int k = arguments.k;
	int n = arguments.n;

	double t0, t1;

	int pid;
	int numprocs;
	MPI_Status status;
	MPI_Comm cartcom;

	int dims[2];
	int period[2];
	int coord[2];
		
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

	dims[0] = 0;
	dims[1] = 0;
	period[0] = 1;
	period[1] = 1;
		
	MPI_Dims_create(numprocs, 2, dims);	
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, period, 1, &cartcom);
	MPI_Cart_coords(cartcom, pid, 2, coord);
	MPI_Comm_rank(cartcom, &pid);

	bool m_odd = !arguments.m || (arguments.m & (arguments.m - 1));
	bool k_odd = !arguments.k || (arguments.k & (arguments.k - 1));
	bool n_odd = !arguments.n || (arguments.n & (arguments.n - 1));
		
	if (m_odd)
		m = pow(2, 32 - __builtin_clz(arguments.m)); // 32 - n-pierwszych zer

	if (k_odd)
		k = pow(2, 32 - __builtin_clz(arguments.k));

	if (n_odd)
		n = pow(2, 32 - __builtin_clz(arguments.n));

	// maximum of m, k, n
    int max = m;
    (max < k) && (max = k); 
    (max < n) && (max = n);

	int xSz = max/dims[1];  // długość bloku
	int ySz = max/dims[0]; // wysokość bloku

	double *pA = (double *)mkl_malloc(xSz * ySz * sizeof(double), 64);
	double *pB = (double *)mkl_malloc(xSz * ySz * sizeof(double), 64);
	double *pC = (double *)mkl_malloc(xSz * ySz * sizeof(double), 64);
	double *tmp_pC = (double *)mkl_malloc(xSz * ySz * sizeof(double), 64);

	MPI_Datatype MPI_SUBMATRIX;
	MPI_Type_contiguous(xSz * ySz, MPI_DOUBLE, &MPI_SUBMATRIX);
	MPI_Type_commit(&MPI_SUBMATRIX);
	
	double *A;
	double *B;
	double *C;
	
	// loading matrices
	if (pid == ROOT) { 
		A = (double *)mkl_malloc(max * max * sizeof(double), 64);
		B = (double *)mkl_malloc(max * max * sizeof(double), 64);
		C = (double *)mkl_malloc(max * max * sizeof(double), 64);

		load_matrix(arguments.pathA, A, arguments.m, arguments.k, max);
		load_matrix(arguments.pathB, B, arguments.k, arguments.n, max);
	}

    switch(arguments.method) {
        case SEQUENTIAL:
		{
			if(pid == ROOT) {
				t0 = MPI_Wtime();
				for(int i = 0; i < arguments.m; i++) {
					for(int j = 0; j < arguments.n; j++) {
						for(int l = 0; l < arguments.k; l++) {
							C[i * n + j] = A[i * n + l] * B[l * n + j];
						}
					}
				}
				t1 = MPI_Wtime();
			}
			break;
        }
        case MKL:
	    {
			if(pid == ROOT) {
				t0 = MPI_Wtime();
				cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, arguments.m, arguments.k, arguments.n, 1.0, A, arguments.k, B, arguments.n, 0.0, C, arguments.n);
				t1 = MPI_Wtime();
			}
            break;
        }
        case CANNON:
        {
			//distribution
			if(dims[0] >= max) {
				MPI_Scatter(A, 1, MPI_DOUBLE, pA, 1, MPI_DOUBLE, ROOT, cartcom);
				MPI_Scatter(B, 1, MPI_DOUBLE, pB, 1, MPI_DOUBLE, ROOT, cartcom);
			}

			if(pid == ROOT) {
				if(dims[0] < max) {
					int blocklength = xSz;
					int displacements[ySz];

					//initial shift
					int proclA[numprocs];
					for(int i = 0; i < dims[0]; i++) {
						for(int j = 0; j < dims[1]; j++) {
							if(j < dims[1] - i) {
								proclA[i * dims[1] + j] = i * dims[1] + j + i;			
							} else {
								proclA[i * dims[1] + j] = i * dims[1] + j - (dims[1] - i);
							 }
						}
					}

					int proclB[numprocs];
					for(int j = 0; j < dims[1]; j++) {
						for(int i = 0; i < dims[0]; i++) {
							if(i < dims[0] - j) { 
								proclB[i * dims[1] + j] = (i + j) * dims[0] + j;			
							} else {
								proclB[i * dims[1] + j] = (i - dims[0] + j) * dims[1] + j;
							}
						}
					}

				
					for(int proc = numprocs - 1; proc >= 0; proc--) {

						//A matrix
						int start = (proc % dims[1]) * xSz + (proc / dims[1]) * (dims[1] * xSz * ySz);
						displacements[0] = start;
						for(int k = 1; k < ySz; k++) {				
							displacements[k] =  displacements[k-1] + xSz * dims[1];
						}

						int k = 0;
						for(int i = 0; i < ySz; i++) {
							for(int j = 0; j < blocklength; j++) {
								pA[k] = A[displacements[i] + j];
								pB[k] = B[displacements[i] + j];
								k++;
							}
						}

						if(proc != ROOT) {
							MPI_Send(pA, 1, MPI_SUBMATRIX, proclA[proc], DISTRIBUTION, cartcom);
							MPI_Send(pB, 1, MPI_SUBMATRIX, proclB[proc], DISTRIBUTION, cartcom);
						}
						//po ostatnim refrenie w pA jest zawartośc dla procesu 0
					}
				}
		
			} else {
				if(dims[0] < max) {					
					MPI_Recv(pA, 1, MPI_SUBMATRIX, ROOT, DISTRIBUTION, cartcom, &status);
					MPI_Recv(pB, 1, MPI_SUBMATRIX, ROOT, DISTRIBUTION, cartcom, &status);
				}
			}

			t0 = MPI_Wtime();
			cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, xSz, xSz, xSz, 1.0, pA, xSz, pB, xSz, 0.0, pC, xSz);

//			MPI_Barrier(cartcom);
			//scewing
			int top, bottom, left, right;
			MPI_Cart_shift(cartcom, 1, 1, &left, &right);
			MPI_Cart_shift(cartcom, 0, 1, &top, &bottom);
			for(int i = 0; i < dims[0]; i++) {
				MPI_Sendrecv_replace(pA, 1, MPI_SUBMATRIX, left, SKEW, right, SKEW, cartcom, &status);
				MPI_Sendrecv_replace(pB, 1, MPI_SUBMATRIX, bottom, SKEW, top, SKEW, cartcom, &status);
				cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, xSz, xSz, xSz, 1.0, pA, xSz, pB, xSz, 0.0, tmp_pC, xSz);
			//	mkl_domatadd(CblasRowMajor, CblasNoTrans, CblasNoTrans, xSz, xSz, 1.0, pC, xSz, 1.0, tmp_pC, xSz, pC, xSz);
				for(int j = 0; j < xSz * ySz; j++) {
					pC[j] += tmp_pC[j];
				}
			}
			t1 = MPI_Wtime();
            break;
        	}
    }

	MPI_Barrier(cartcom);

	if(arguments.method != SEQUENTIAL) {
		if(pid == ROOT) {
			int displacements[ySz];
			displacements[0] = 0; 
			for(int k = 1; k < ySz; k++) {				
				displacements[k] =  displacements[k-1] + xSz * dims[1];
			}
			for(int i = 0; i < ySz; i++) {
				for(int j = 0; j < xSz; j++) {
					C[displacements[i] + j] = pC[i * xSz + j];
				}
			}
	
		}

		for(int proc = 1; proc < numprocs; proc++) {
			if(pid == proc) {
				MPI_Send(pC, 1, MPI_SUBMATRIX, ROOT, COLLECTING, cartcom);
			}

			if (pid == ROOT) {
				MPI_Recv(pC, 1, MPI_SUBMATRIX, proc, COLLECTING, cartcom, &status);

				int displacements[ySz];
				int start = (proc % dims[1]) * xSz + (proc / dims[1]) * (dims[1] * xSz * ySz);
				displacements[0] = start;

				for(int k = 1; k < ySz; k++) {				
					displacements[k] =  displacements[k-1] + xSz * dims[1];
				}

				for(int i = 0; i < ySz; i++) {
					for(int j = 0; j < xSz; j++) {
						C[displacements[i] + j] = pC[i * xSz + j];
					}
				}
			}
		}
	}

	if(pid == ROOT) {
		switch(arguments.mode) {
			case VERBOSE: {
				for(int i = 0; i < arguments.m * arguments.n; i++) {
//					printf("%lf \t", C[i]);
//					if((i + 1) % m  == 0) {
//						printf("\n");
					printf("%lf ", C[i]);
//					}					
				}
				printf("\nTime elapsed: %lf\n", t1-t0);
			}

	        case QUIET:
	        {
				if (arguments.pathC != NULL) {
					char filename[15];
					char method[10];

					sprintf(filename,"debug_%d", (unsigned)time(NULL));

					switch(arguments.method) {
						case 0: {
							sprintf(method, "SEQUENTIAL");
							break;
						}
						case 1: {
							sprintf(method, "MKL");
							break;
						}
						case 2: {
							sprintf(method, "CANNON");
							break;
						}
					}
					save_matrix(arguments.pathC, C, arguments.m * arguments.k);
					save_info(filename, t1-t0, method, arguments.m, arguments.k, arguments.n, numprocs);
				}

				break;
			}
	    }
		mkl_free(A);
		mkl_free(B);
		mkl_free(C);		
	}

	mkl_free(pA);
	mkl_free(pB);
	mkl_free(pC);
	mkl_free(tmp_pC);

	MPI_Comm_free(&cartcom);
	MPI_Finalize();
    return(EXIT_SUCCESS);
}
