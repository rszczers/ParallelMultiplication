#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <argp.h>
#include <string.h>

#include <mkl.h>
#include <math.h>

#include <mpi.h>

#include "load_matrix.h"
#include "save_matrix.h"


//#include "sequential.h"
//#include "naive.h"

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
    enum {SEQUENTIAL, NAIVE, STRASSEN, CANNON} method;
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
            char *name = (char *)malloc(10 * sizeof(char));

            for (int i = 0; i<11 ; arg++, i++)
                name[i] = tolower(*arg);

            if(strcmp("strassen", name) == 0)
                arguments->method = STRASSEN;
            else if(strcmp("cannon", name) == 0)
                arguments->method = CANNON;
            else if(strcmp("sequential", name) == 0)
                arguments->method = SEQUENTIAL;
            // NAIVE is set as default

            free(name);         
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
        case 'o':
            arguments->pathC = arg;
            break;
        case 'l':
        {
            printf("Available methods:\n");
            char *availableMethods[] = {"sequential", "naive", "strassen", "cannon"};
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
            arguments->method = NAIVE;
            arguments->mode = QUIET;
            arguments->pathC = NULL;
            break;
        case ARGP_KEY_END:
            // if(state->arg_num > 0)
                 // argp_usage(state);
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
	double *pB = (double *)mkl_malloc((max * max)/(dims[0] * dims[1]) * sizeof(double), 64);
	double *pC = (double *)mkl_malloc((max * max)/(dims[0] * dims[1]) * sizeof(double), 64);
	
	
	//broadcasting
	if (pid == 0) {
		double *A = (double *)mkl_malloc(max * max * sizeof(double), 64);
		double *B = (double *)mkl_malloc(max * max * sizeof(double), 64);
		double *C = (double *)mkl_malloc(max * max * sizeof(double), 64);

		load_matrix(arguments.pathA, A, arguments.m, arguments.k, max);
		load_matrix(arguments.pathB, B, arguments.k, arguments.n, max);

		double *tmpA = (double *)mkl_malloc((max * max)/(dims[0] * dims[1]) * sizeof(double), 64);
		if(dims[0] >= max) {
		//wysyłać po jednym elemencie
		} else {
			int count = ySz;

			int blocklength = xSz;
			int displacements[ySz];

			for(int proc = 0; proc < numprocs; proc++) {
				int start = (proc % dims[1]) * dims[1] + (proc / dims[1]) * (dims[1] * xSz * ySz);
				displacements[0] = start;
				for(int k = 1; k < ySz; k++) {				
					displacements[k] =  displacements[k-1] + xSz * dims[1];
				}
					int k = 0;
					for(int i = 0; i < ySz; i++) {
						for(int j = 0; j < blocklength; j++) {
							pA[k] = A[displacements[i] + j];
							printf("k = %d, i = %d, j = %d, pA[k] = %lf\n", k, i, j, pA[k]);
							k++;
						}
					}
			}
		}

	} else {

	}




    switch(arguments.method) {
        case SEQUENTIAL:
        {               
//            sequential_product(A, B, C, arguments.m, arguments.k, arguments.n, 1.0, 0.0);
            break;
        }
        case NAIVE:
        {
//			naive(A, B, C, arguments.m, arguments.k, arguments.n);
            break;
        }
        case STRASSEN:
    {
            break;
        }
        case CANNON:
        {
            break;
        }
    }

	if(pid == 0) {
	    switch(arguments.mode) {
			case VERBOSE:
	        {
			    for (int i = 0; i <= arguments.m; ++i)            
			        for (int j = 0; j <= arguments.n; ++j)
//			            printf("%lf ", C[i*(j+1)]);
					printf("\n");
	        }
	        case QUIET:
	        {            
				if (arguments.pathC != NULL) {
                //save_matrix(arguments.pathC, A, arguments.m * arguments.k);            
				}
				break;
			}
	    }
		
  
	}

	mkl_free(pA);
	mkl_free(pB);
	mkl_free(pC);

	MPI_Comm_free(&cartcom);
	MPI_Finalize();
    return EXIT_SUCCESS;
}
