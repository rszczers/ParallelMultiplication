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
#include <omp.h>

#include "load_matrix.h"
#include "save_matrix.h"
#include "save_info.h"

#define ROOT 0
#define DISTRIBUTION_A 1337
#define DISTRIBUTION_B 1338 
#define SKEW_LEFTRIGHT 23
#define SKEW_BOTTOMUP 93
#define COLLECTING 42
#define COMMUNICATION_TIME 44

const char *argp_program_version = "pmm v0.1";
const char *argp_program_bug_address = "<rafal.szczerski@gmail.com>";
static char doc[] = "Parallel Matrix Multiplication";
static char args_doc[] = "-A matrixA_path -B matrixB_path -m NUM -k NUM -n NUM";

static struct argp_option options[] = {
    { "method",     'a', "METHOD", 0, "Algorithm used"},
    { "inputA",     'A',   "FILE", 0, "Path to input FILE containing matrix A data"},
    { "inputB",     'B',   "FILE", 0, "Path to input FILE containing matrix B data"},
    { "output",     'o',   "FILE", OPTION_ARG_OPTIONAL, "Path to output FILE containing matrix C=A*B data"},
    {        0,     'C',   "FILE", OPTION_ALIAS, 0},
    {  "debug",     'd',    "DIR", OPTION_ARG_OPTIONAL, "Path to debug directory"},
    {        0,     'm',    "NUM", 0, "Number of rows of A"},
    {        0,     'k',    "NUM", 0, "Number of rows of B"},
    {        0,     'n',    "NUM", 0, "Number of columns of B"},
    {   "list",     'l',        0, 0, "Show list of available algorithms"},
    {   "time",     't',        0, 0, "Show elapsed time"},
    {  "quiet",     'q',        0, 0, "Do not show any computations"},
    {  "steps",     's',        0, 0, "Dump data from each node for every step"},
    {"verbose",     'v',        0, 0, "Show all computations"},
    {"omp_threads", 777,    "NUM", OPTION_HIDDEN, 0},
    { 0 } 
};

struct arguments {
    enum {SEQUENTIAL, MKL, OMP, CANNON, CANNON_DGEMM, CANNON_OMP} method;
    enum {QUIET, VERBOSE} mode;
    bool time;
    bool steps;
    int omp_threads;
    int m, k, n;
    char *pathA;
    char *pathB;
    char *pathC;
    char *debugDir;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {        
        case 'a':
        {   
            char *name = (char *)malloc(13 * sizeof(char));
            for (int i = 0; i<13 ; arg++, i++)
                name[i] = tolower(*arg);

            if(strncmp("mkl", name, 12) == 0)
                arguments->method = MKL;
            else if(strncmp("sequential", name, 12) == 0)
                arguments->method = SEQUENTIAL;
            else if(strncmp("omp", name, 12) == 0) 
                arguments->method = OMP;
            else if(strncmp("cannon", name, 12) == 0)
                arguments->method = CANNON;
            else if(strncmp("cannon_dgemm", name, 12) == 0)
                arguments->method = CANNON_DGEMM;
            else if(strncmp("cannon_omp", name, 12) == 0)
                arguments->method = CANNON_OMP;
            else {
                /* exit if there's no such method */
                argp_usage(state);
            }
            // cannon_dgemm method is set as default
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
            if(sizeof(arg) != 1)
                arguments->pathC = arg;
            break;
        }
        case 'd': {
            if(arg[strlen(arg)-1] != '/') {
                char parg[strlen(arg)+1];
                strncpy(parg, arg, strlen(arg));
                parg[strlen(arg)] = '/';
                arguments->debugDir = parg;
            } else {
                arguments->debugDir = arg;
            }
            break;
        }
        case 'l':
        {
            printf("Available methods:\n");
            char *availableMethods[] = {"Sequential", "MKL", 
                "OMP", "CANNON", "CANNON_DGEMM", "CANNON_OMP"};
            for (int i = 0; i <= CANNON_OMP; ++i)
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
        case 't':
            arguments->time = true;
            break;
        case 'q':
            arguments->mode = QUIET;
            break;
        case 'v':
            arguments->mode = VERBOSE;
            break;
        case 's':
            arguments->steps = true;
            break;
        case 777:
            arguments->omp_threads = atoi(arg);
            break;
        case ARGP_KEY_ARG:
            break;
        case ARGP_KEY_INIT:
            arguments->steps = false;
            arguments->method = CANNON;
            arguments->mode = QUIET;
            arguments->pathC = NULL;
            break;
        case ARGP_KEY_END:
	        if(state->arg_num > 0)
                argp_usage(state);
            break;
        default: return ARGP_ERR_UNKNOWN;
    }   
    return EXIT_SUCCESS;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char *argv[]) {
    struct arguments arguments;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    
    double t0, t1; /* run-time with data already loaded */
    double total_t0 = 0.0, total_t1 = 0.0, total_elap = 0.0;
    double seq_t0 = 0.0, seq_t1 = 0.0, seq_elap = 0.0;
    double tmp_time0, tmp_time1;

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

    double *A;
    double *B;
    double *C;

    double *pA; 
    double *pB;
    double *pC; 
    double *tmp_pC;
    unsigned int max; // placed here for verbose mode purproses; indices new line;
    MPI_Datatype MPI_SUBMATRIX;

    switch (arguments.method) {        
/*############################################################################*/
/*############################################################################*/            
/*############################################################################*/
/*############################################################################*/
/*############################################################################*/
        case SEQUENTIAL:
        {
            total_t0 = MPI_Wtime();
            if (pid == ROOT) {
                total_t0 = MPI_Wtime();

                A = (double *) mkl_malloc(arguments.m * arguments.k * 
                        sizeof(double), 64);
                B = (double *) mkl_malloc(arguments.k * arguments.n * 
                        sizeof(double), 64);
                C = (double *) mkl_malloc(arguments.m * arguments.n * 
                        sizeof(double), 64);

                if(A == NULL || B == NULL || C == NULL) {
                    printf("\nCouldn't allocate memory. Aborting.\n\n");
                    mkl_free(A);
                    mkl_free(B);
                    mkl_free(C);
                    exit(EXIT_FAILURE);
                }

                load_matrix(arguments.pathA, A, arguments.m, arguments.k, 0, 
                        false);
                load_matrix(arguments.pathB, B, arguments.k, arguments.n, 0, 
                        false);

                t0 = MPI_Wtime();
                for (int i = 0; i < arguments.m; i++) {
                    for (int j = 0; j < arguments.n; j++) {
                        for (int l = 0; l < arguments.k; l++) {
                            C[i * arguments.n + j] += 
                                A[i * arguments.k + l] * B[l * arguments.n + j];
                        }
                    }
                }
                t1 = MPI_Wtime();
                total_t1 = MPI_Wtime();
            }
            break;
        }

/*############################################################################*/
/*############################################################################*/            
/*############################################################################*/
/*############################################################################*/
/*############################################################################*/

        case MKL:            
            if (pid == ROOT) {
                total_t0 = MPI_Wtime();
                A = (double *) mkl_malloc(arguments.m * arguments.k * 
                        sizeof(double), 64);
                B = (double *) mkl_malloc(arguments.k * arguments.n * 
                        sizeof(double), 64);
                C = (double *) mkl_malloc(arguments.m * arguments.n * 
                        sizeof(double), 64);

                if(A == NULL || B == NULL || C == NULL) {
                    printf("\nCouldn't allocate memory. Aborting.\n\n");
                    mkl_free(A);
                    mkl_free(B);
                    mkl_free(C);
                    exit(EXIT_FAILURE);
                }

                load_matrix(arguments.pathA, A, arguments.m, arguments.k, 0, 
                        false);
                load_matrix(arguments.pathB, B, arguments.k, arguments.n, 0, 
                        false);

                t0 = MPI_Wtime();                
                cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                        arguments.m, arguments.k, arguments.n, 1.0, A, 
                        arguments.k, B, arguments.n, 0.0, C, arguments.n);
                t1 = MPI_Wtime();
                total_t1 = MPI_Wtime();
            }
            break;


/*############################################################################*/
/*############################################################################*/            
/*############################################################################*/
/*############################################################################*/
/*############################################################################*/


        case OMP:
        {
            if (pid == ROOT) {
                total_t0 = MPI_Wtime();
                A = (double *) mkl_malloc(arguments.m * arguments.k * 
                        sizeof(double), 64);
                B = (double *) mkl_malloc(arguments.k * arguments.n * 
                        sizeof(double), 64);
                C = (double *) mkl_malloc(arguments.m * arguments.n * 
                        sizeof(double), 64);

                if(A == NULL || B == NULL || C == NULL) {
                    printf("\nCouldn't allocate memory. Aborting.\n\n");
                    mkl_free(A);
                    mkl_free(B);
                    mkl_free(C);
                    exit(EXIT_FAILURE);
                }

                load_matrix(arguments.pathA, A, arguments.m, arguments.k, 0, 
                        false);
                load_matrix(arguments.pathB, B, arguments.k, arguments.n, 0, 
                        false);

                t0 = MPI_Wtime();
                int ii = 0;
                int j = 0;
                int l = 0;
                #pragma omp parallel for shared(A, B, C) private(j,l) schedule(static)
                for (ii = 0; ii < arguments.m; ii++) {
                   for (j = 0; j < arguments.n; j++) {
                        for (l = 0; l < arguments.k; l++) {
                            C[ii * arguments.n + j] +=
                                A[ii * arguments.k + l] *
                                B[l * arguments.n + j];
                        }
                    }
                }            
                t1 = MPI_Wtime();
                total_t1 = MPI_Wtime();
            }
            break;
        }

/*############################################################################*/
/*############################################################################*/            
/*############################################################################*/
/*############################################################################*/
/*############################################################################*/


        case CANNON:
        {
            if (pid == ROOT) total_t0 = MPI_Wtime();
            //lets assume that dims[0] = dims[1]
            if (dims[0] != dims[1]) {
                printf("\nProcess mesh is not appropriate. Aborting.\n\n");
                exit(EXIT_FAILURE);
            }

            max = arguments.m;
            (max < arguments.k) && (max = arguments.k); 
            (max < arguments.n) && (max = arguments.n);

            int sz = max/dims[0];  // row length per block 
            if(max > sz * dims[0]) { 
                sz += 1;
                max = sz * dims[0];
            }
            int blockSz = sz * sz;

            MPI_Type_contiguous(blockSz, MPI_DOUBLE, &MPI_SUBMATRIX);
            MPI_Type_commit(&MPI_SUBMATRIX);

            pA = (double *) mkl_malloc(blockSz * sizeof(double), 64);
            pB = (double *) mkl_malloc(blockSz * sizeof(double), 64);
            pC = (double *) mkl_malloc(blockSz * sizeof(double), 64);

            if(pA == NULL || pB == NULL || pC == NULL) {
                printf("\nCouldn't allocate memory. Aborting.\n\n");
                mkl_free(pA);
                mkl_free(pB);
                mkl_free(pC);                
                exit(EXIT_FAILURE);
            }

            memset(pC, 0, blockSz * sizeof(double));

            if (pid == ROOT) {
                A = (double *) mkl_malloc(max * max * sizeof(double), 64);
                B = (double *) mkl_malloc(max * max * sizeof(double), 64);
                C = (double *) mkl_malloc(max * max * sizeof(double), 64);

                memset(C, 0, max * max * sizeof(double));

                if(A == NULL || B == NULL || C == NULL) {
                    printf("\nCouldn't allocate memory.\n\n");
                    mkl_free(A);
                    mkl_free(B);
                    mkl_free(C);
                    exit(EXIT_FAILURE);
                }

                bool resize = true;
                if(arguments.m == max &&
                    arguments.n == max &&
                    arguments.k == max) {
                    resize = false;
                }

                load_matrix(arguments.pathA, A, arguments.m, arguments.k,
                    max, resize);
                load_matrix(arguments.pathB, B, arguments.k, arguments.n,
                    max, resize);
                
                /* debug dump */
                if(arguments.steps) {
                    char *dump_path = (char *)malloc(
                            (strlen(arguments.debugDir) + 6)
                            * sizeof(char));
                    /* assuming that theres less than 9999 nodes */
                    /* that gives less than 9999 steps to dump   */
                    sprintf(dump_path, "%sA_raw", 
                            arguments.debugDir);
                    save_matrix(dump_path, B,
                        arguments.m, arguments.k, max, true, true);

                    sprintf(dump_path, "%sB_raw", 
                            arguments.debugDir);
                    save_matrix(dump_path, A,
                        arguments.k, arguments.n, max, true, true);

                    free(dump_path);
                }

                t0 = MPI_Wtime();
                seq_t0 = MPI_Wtime();
                /* initial shift with procesor ranks */                
                int proclA[numprocs];
                for (int i = 0; i < dims[0]; i++) {
                    for (int j = 0; j < dims[1]; j++) {
                        if (j < dims[1] - i) {
                            proclA[i * dims[1] + j + i] = i * dims[1] + j;
                        } else {
                            proclA[i * dims[1] + j - (dims[1] - i)] = 
                                i * dims[1] + j;
                        }
                    }
                }

                seq_t1 = MPI_Wtime();
                seq_elap += seq_t1 - seq_t0;

                for (int proc = numprocs - 1; proc >= 0; proc--) {
                    seq_t0 = MPI_Wtime();

                    int displacements[sz];
                    int start = (proc % dims[1]) * sz + 
                        (proc / dims[1]) * (dims[1] * blockSz);
                    displacements[0] = start;
                    for (int k = 1; k < sz; k++) {              
                        displacements[k] =  displacements[k-1] + 
                            sz * dims[1];
                    }

                    int k = 0;
                    for (int i = 0; i < sz; i++) {
                        for (int j = 0; j < sz; j++) {
                            pA[k] = A[displacements[i] + j];                            
                            k++;
                        }
                    }

                    seq_t1 = MPI_Wtime();
                    seq_elap += seq_t1 - seq_t0;

                    if(proc != ROOT) {
                        MPI_Send(pA, 1, MPI_SUBMATRIX, proclA[proc], 
                                DISTRIBUTION_A, cartcom);
                    }
                }

            } else {
                MPI_Recv(pA, 1, MPI_SUBMATRIX, ROOT, 
                        DISTRIBUTION_A, cartcom, &status);
            }

            MPI_Barrier(cartcom);

            if(pid == ROOT) {
                seq_t0 = MPI_Wtime();
                int proclB[numprocs];
                for (int j = 0; j < dims[1]; j++) {
                    for (int i = 0; i < dims[0]; i++) {
                        if (i < dims[0] - j) { 
                            proclB[(i + j) * dims[0] + j] = 
                                i * dims[1] + j;
                        } else {
                            proclB[(i - dims[0] + j) * dims[1] + j] = 
                                i * dims[1] + j;
                        }
                    }
                }

                seq_t1 = MPI_Wtime();
                seq_elap += seq_t1 - seq_t0;

                for (int proc = numprocs - 1; proc >= 0; proc--) {
                    seq_t0 = MPI_Wtime();
                    /* indeks w macierzy A/B pierwszego elementu z k-tego wiersza */
                    int displacements[sz]; 
                    int start = (proc % dims[1]) * sz + 
                        (proc / dims[1]) * (dims[1] * blockSz);
                    displacements[0] = start;
                    for (int k = 1; k < sz; k++) {              
                        displacements[k] =  displacements[k-1] + sz * dims[1];
                    }

                    int k = 0;
                    for (int i = 0; i < sz; i++) {
                        for (int j = 0; j < sz; j++) {
                            pB[k] = B[displacements[i] + j];
                            k++;
                        }
                    }

                    seq_t1 = MPI_Wtime();
                    seq_elap += seq_t1 - seq_t0;

                    if(proc != ROOT) {
                        MPI_Send(pB, 1, MPI_SUBMATRIX, proclB[proc], 
                                DISTRIBUTION_B, cartcom);
                    }                    
                }            
            } else {                
                MPI_Recv(pB, 1, MPI_SUBMATRIX, ROOT, 
                        DISTRIBUTION_B, cartcom, &status);
            }
            
            for (int ii = 0; ii < sz; ii++) {
               for (int j = 0; j < sz; j++) {
                    for (int l = 0; l < sz; l++) {
                        pC[ii * sz + j] +=
                            pA[ii * sz + l] * pB[l * sz + j];
                    }
                }
            }

            /* dump data for first step  */
            if(arguments.steps) {
                char *dump_path = (char *)malloc(
                        (strlen(arguments.debugDir) + 12)
                        * sizeof(char));
                /* assuming that theres less than 9999 nodes */
                /* that gives less than 9999 steps to dump   */
                sprintf(dump_path, "%s%04d_0000_A", arguments.debugDir, pid);
                save_matrix(dump_path, pA,
                    sz, sz, sz, true, true);

                sprintf(dump_path, "%s%04d_0000_B", arguments.debugDir, pid);
                save_matrix(dump_path, pB,
                    sz, sz, sz, true, true);

                sprintf(dump_path, "%s%04d_0000_C", arguments.debugDir, pid);
                save_matrix(dump_path, pC,
                    sz, sz, sz, true, true);

                free(dump_path);
            }

            /* skewing */
            int top, bottom, left, right;
            MPI_Cart_shift(cartcom, 1, 1, &left, &right);
            MPI_Cart_shift(cartcom, 0, 1, &top, &bottom);
            
            for (int i = 1; i < dims[0]; i++) { /* if dims[0] == dims[1] */
                MPI_Sendrecv_replace(pA, 1, MPI_SUBMATRIX, left, 
                        SKEW_LEFTRIGHT, right, 
                        SKEW_LEFTRIGHT, cartcom, &status);
                MPI_Sendrecv_replace(pB, 1, MPI_SUBMATRIX, top, 
                        SKEW_BOTTOMUP, bottom, 
                        SKEW_BOTTOMUP, cartcom, &status);

                for (int ii = 0; ii < sz; ii++) {
                   for (int j = 0; j < sz; j++) {
                        for (int l = 0; l < sz; l++) {
                            pC[ii * sz + j] +=
                                pA[ii * sz + l] * pB[l * sz + j];
                        }
                    }
                }

                /* dump data for step 1-p */
                if(arguments.steps) {
                    char *dump_path = (char *)malloc(
                            (strlen(arguments.debugDir) + 12)
                            * sizeof(char));
                    /* assuming that theres less than 9999 nodes */
                    /* that gives less than 9999 steps to dump   */
                    sprintf(dump_path, "%s%04d_%04d_A", 
                            arguments.debugDir, pid, i);
                    save_matrix(dump_path, pA,
                        sz, sz, sz, true, true);

                    sprintf(dump_path, "%s%04d_%04d_B", 
                            arguments.debugDir, pid, i);
                    save_matrix(dump_path, pB,
                        sz, sz, sz, true, true);

                    sprintf(dump_path, "%s%04d_%04d_C", 
                            arguments.debugDir, pid, i);
                    save_matrix(dump_path, pC,
                        sz, sz, sz, true, true);

                    free(dump_path);
                }

            }

            MPI_Barrier(cartcom);

            if(pid == ROOT) {
                seq_t0 = MPI_Wtime();
                /* displacement for root process */
                int displacements[sz];
                displacements[0] = 0; 
                for(int k = 1; k < sz; k++) {              
                    displacements[k] =  
                        displacements[k-1] + sz * dims[1];
                }
                for(int i = 0; i < sz; i++) {
                    for(int j = 0; j < sz; j++) {
                        C[displacements[i] + j] = pC[i * sz + j];                        
                    }
                }
                seq_t1 = MPI_Wtime();
                seq_elap += seq_t1 - seq_t0;                
            }
            /* sending computed data from all over the grid to root process*/
            for(int proc = 1; proc < numprocs; proc++) {
                if(pid == proc) {
                    MPI_Send(pC, 1, MPI_SUBMATRIX, ROOT, 
                            COLLECTING, cartcom);
                }

                if (pid == ROOT) {
                    MPI_Recv(pC, 1, MPI_SUBMATRIX, proc, 
                            COLLECTING, cartcom, &status);

                    seq_t0 = MPI_Wtime();

                    // przesunięcia wierszy dla pozostałych procesów
                    int displacements[sz];
                    /*  proc % dims[1] * sz - first column */
                    /*  of first line of submatrix stored in p-process */
                    /*  (proc / dims[1])... - horizontal shifts */
                    int start = (proc % dims[1]) * sz +
                        (proc / dims[1]) * (dims[1] * blockSz);
                    displacements[0] = start;

                    for(int k = 1; k < sz; k++) {              
                        displacements[k] =
                            displacements[k-1] + sz * dims[1];
                    }

                    for(int i = 0; i < sz; i++) {
                        for(int j = 0; j < sz; j++) {
                            C[displacements[i] + j] = pC[i * sz + j];
                        }
                    }

                    seq_t1 = MPI_Wtime();
                    seq_elap += seq_t1 - seq_t0;
                }
            }
            mkl_free(pA);
            mkl_free(pB);
            mkl_free(pC);

            if (pid == ROOT) {
                total_t1 = MPI_Wtime();
                t1 = MPI_Wtime();
            }

            break;
        }


/*############################################################################*/
/*############################################################################*/            
/*############################################################################*/
/*############################################################################*/
/*############################################################################*/

        case CANNON_DGEMM:
        {
            if (pid == ROOT) total_t0 = MPI_Wtime();            
            //lets assume that dims[0] = dims[1]
            if (dims[0] != dims[1]) {
                printf("\nProcess mesh is not appropriate. Aborting.\n\n");
                exit(EXIT_FAILURE);
            }

            max = arguments.m;
            (max < arguments.k) && (max = arguments.k); 
            (max < arguments.n) && (max = arguments.n);

            int sz = max/dims[0];  // row length per block 

            if(max > sz * dims[0]) { 
                sz += 1;
                max = sz * dims[0];
            }

            int blockSz = sz * sz;

            MPI_Type_contiguous(blockSz, MPI_DOUBLE, &MPI_SUBMATRIX);
            MPI_Type_commit(&MPI_SUBMATRIX);

            pA = (double *) mkl_malloc(blockSz * sizeof(double), 64);
            pB = (double *) mkl_malloc(blockSz * sizeof(double), 64);
            pC = (double *) mkl_malloc(blockSz * sizeof(double), 64);
            tmp_pC = (double *) mkl_malloc(blockSz * sizeof(double), 64);

            if(pA == NULL || pB == NULL || pC == NULL || tmp_pC == NULL) {
                printf("\nCouldn't allocate memory. Aborting.\n\n");
                mkl_free(pA);
                mkl_free(pB);
                mkl_free(pC);
                mkl_free(tmp_pC);
                exit(EXIT_FAILURE);
            }

            memset(pC, 0, blockSz * sizeof(double));
            memset(tmp_pC, 0, blockSz * sizeof(double));

            if (pid == ROOT) {
                A = (double *) mkl_malloc(max * max * sizeof(double), 64);
                B = (double *) mkl_malloc(max * max * sizeof(double), 64);
                C = (double *) mkl_malloc(max * max * sizeof(double), 64);

                memset(C, 0, max * max * sizeof(double));

                if(A == NULL || B == NULL || C == NULL) {
                    printf("\nCouldn't allocate memory.\n\n");
                    mkl_free(A);
                    mkl_free(B);
                    mkl_free(C);
                    exit(EXIT_FAILURE);
                }

                bool resize = true;
                if(arguments.m == max &&
                    arguments.n == max &&
                    arguments.k == max) {
                    resize = false;
                }

                load_matrix(arguments.pathA, A, arguments.m, arguments.k, max, 
                        resize);
                load_matrix(arguments.pathB, B, arguments.k, arguments.n, max, 
                        resize);

                /* debug dump */
                if(arguments.steps) {
                    char *dump_path = (char *)malloc(
                            (strlen(arguments.debugDir) + 6)
                            * sizeof(char));
                    /* assuming that theres less than 9999 nodes */
                    /* that gives less than 9999 steps to dump   */
                    sprintf(dump_path, "%sA_raw", 
                            arguments.debugDir);
                    save_matrix(dump_path, B,
                        arguments.m, arguments.k, max, true, true);

                    sprintf(dump_path, "%sB_raw", 
                            arguments.debugDir);
                    save_matrix(dump_path, A,
                        arguments.k, arguments.n, max, true, true);

                    free(dump_path);
                }

                seq_t0 = MPI_Wtime();                
                t0 = MPI_Wtime();

                /* initial shift with procesor ranks */                
                int proclA[numprocs];
                for (int i = 0; i < dims[0]; i++) {
                    for (int j = 0; j < dims[1]; j++) {
                        if (j < dims[1] - i) {
                            proclA[i * dims[1] + j + i] = i * dims[1] + j;
                        } else {
                            proclA[i * dims[1] + j - (dims[1] - i)] = 
                                i * dims[1] + j;
                        }
                    }
                }
                seq_t1 = MPI_Wtime();
                total_elap = seq_t1 - seq_t0;

                for (int proc = numprocs - 1; proc >= 0; proc--) {
                    seq_t0 = MPI_Wtime();

                    int displacements[sz];
                    int start = (proc % dims[1]) * sz + 
                        (proc / dims[1]) * (dims[1] * blockSz);
                    displacements[0] = start;
                    for (int k = 1; k < sz; k++) {              
                        displacements[k] =  displacements[k-1] + sz * dims[1];
                    }

                    int k = 0;
                    for (int i = 0; i < sz; i++) {
                        for (int j = 0; j < sz; j++) {
                            pA[k] = A[displacements[i] + j];                            
                            k++;
                        }
                    }

                    seq_t1 = MPI_Wtime();
                    total_elap += seq_t1 - seq_t0;

                    if(proc != ROOT) {
                        MPI_Send(pA, 1, MPI_SUBMATRIX, proclA[proc], 
                                DISTRIBUTION_A, cartcom);
                    }
                }
            } else {
                MPI_Recv(pA, 1, MPI_SUBMATRIX, ROOT, 
                        DISTRIBUTION_A, cartcom, &status);
            }

            MPI_Barrier(cartcom);

            if(pid == ROOT) {
                seq_t0 = MPI_Wtime();
                int proclB[numprocs];
                for (int j = 0; j < dims[1]; j++) {
                    for (int i = 0; i < dims[0]; i++) {
                        if (i < dims[0] - j) { 
                            proclB[(i + j) * dims[0] + j] = 
                                i * dims[1] + j;
                        } else {
                            proclB[(i - dims[0] + j) * dims[1] + j] = 
                                i * dims[1] + j;
                        }
                    }
                }
                seq_t1 = MPI_Wtime();
                seq_elap += seq_t1 - seq_t0;
                for (int proc = numprocs - 1; proc >= 0; proc--) {
                    seq_t0 = MPI_Wtime();
                    /* indeks w macierzy A/B pierwszego elementu z k-tego wiersza */
                    int displacements[sz]; 
                    int start = (proc % dims[1]) * sz + 
                        (proc / dims[1]) * (dims[1] * blockSz);
                    displacements[0] = start;
                    for (int k = 1; k < sz; k++) {              
                        displacements[k] =  displacements[k-1] + sz * dims[1];
                    }

                    int k = 0;
                    for (int i = 0; i < sz; i++) {
                        for (int j = 0; j < sz; j++) {
                            pB[k] = B[displacements[i] + j];
                            k++;
                        }
                    }
                    seq_t1 = MPI_Wtime();
                    seq_elap += seq_t1 - seq_t0;
                    if(proc != ROOT) {
                        MPI_Send(pB, 1, MPI_SUBMATRIX, proclB[proc], 
                                DISTRIBUTION_B, cartcom);
                    }                    
                }            
            } else {                
                MPI_Recv(pB, 1, MPI_SUBMATRIX, ROOT, 
                        DISTRIBUTION_B, cartcom, &status);
            }
            

            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                    sz, sz, sz, 1.0, pA, sz, pB, sz, 0.0, pC, sz);

            /* dump data for first step  */
            if(arguments.steps) {
                char *dump_path = (char *)malloc(
                        (strlen(arguments.debugDir) + 12)
                        * sizeof(char));
                /* assuming that theres less than 9999 nodes */
                /* that gives less than 9999 steps to dump   */
                sprintf(dump_path, "%s%04d_0000_A", arguments.debugDir, pid);
                save_matrix(dump_path, pA,
                    sz, sz, sz, true, true);

                sprintf(dump_path, "%s%04d_0000_B", arguments.debugDir, pid);
                save_matrix(dump_path, pB,
                    sz, sz, sz, true, true);

                sprintf(dump_path, "%s%04d_0000_C", arguments.debugDir, pid);
                save_matrix(dump_path, pC,
                    sz, sz, sz, true, true);

                free(dump_path);
            }

            /* skewing */
            int top, bottom, left, right;
            MPI_Cart_shift(cartcom, 1, 1, &left, &right);
            MPI_Cart_shift(cartcom, 0, 1, &top, &bottom);
            
            for (int i = 1; i < dims[0]; i++) { /* if dims[0] == dims[1] */
                MPI_Sendrecv_replace(pA, 1, MPI_SUBMATRIX, left, 
                        SKEW_LEFTRIGHT, right, 
                        SKEW_LEFTRIGHT, cartcom, &status);
                MPI_Sendrecv_replace(pB, 1, MPI_SUBMATRIX, top, 
                        SKEW_BOTTOMUP, bottom, 
                        SKEW_BOTTOMUP, cartcom, &status);

               
                cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                    sz, sz, sz, 1.0, pA, sz, pB, sz, 0.0, tmp_pC, sz);

                for(int j = 0; j < blockSz; j++) {
                    pC[j] += tmp_pC[j];
                }

                /* dump data for step 1-p */
                if(arguments.steps) {
                    char *dump_path = (char *)malloc(
                            (strlen(arguments.debugDir) + 12)
                            * sizeof(char));
                    /* assuming that theres less than 9999 nodes */
                    /* that gives less than 9999 steps to dump   */
                    sprintf(dump_path, "%s%04d_%04d_A", 
                            arguments.debugDir, pid, i);
                    save_matrix(dump_path, pA,
                        sz, sz, sz, true, true);

                    sprintf(dump_path, "%s%04d_%04d_B", 
                            arguments.debugDir, pid, i);
                    save_matrix(dump_path, pB,
                        sz, sz, sz, true, true);

                    sprintf(dump_path, "%s%04d_%04d_C", 
                            arguments.debugDir, pid, i);
                    save_matrix(dump_path, pC,
                        sz, sz, sz, true, true);

                    free(dump_path);
                }

            }

            MPI_Barrier(cartcom);

            if(pid == ROOT) {
                seq_t0 = MPI_Wtime();
                /* displacement for root process */
                int displacements[sz];
                displacements[0] = 0; 
                for(int k = 1; k < sz; k++) {              
                    displacements[k] =  
                        displacements[k-1] + sz * dims[1];
                }
                for(int i = 0; i < sz; i++) {
                    for(int j = 0; j < sz; j++) {
                        C[displacements[i] + j] = pC[i * sz + j];                        
                    }
                }
                seq_t1 = MPI_Wtime();
                seq_elap += seq_t1 - seq_t0;
            }
            /* sending computed data from all over the grid to root process*/
            for(int proc = 1; proc < numprocs; proc++) {
                if(pid == proc) {
                    MPI_Send(pC, 1, MPI_SUBMATRIX, ROOT, 
                            COLLECTING, cartcom);
                }

                if (pid == ROOT) {
                    MPI_Recv(pC, 1, MPI_SUBMATRIX, proc, 
                            COLLECTING, cartcom, &status);
                    seq_t0 = MPI_Wtime();
                    // przesunięcia wierszy dla pozostałych procesów
                    int displacements[sz];
                    /*  proc % dims[1] * sz - first column */
                    /*  of first line of submatrix stored in p-process */
                    /*  (proc / dims[1])... - horizontal shifts */
                    int start = (proc % dims[1]) * sz +
                        (proc / dims[1]) * (dims[1] * blockSz);
                    displacements[0] = start;

                    for(int k = 1; k < sz; k++) {              
                        displacements[k] =
                            displacements[k-1] + sz * dims[1];
                    }

                    for(int i = 0; i < sz; i++) {
                        for(int j = 0; j < sz; j++) {
                            C[displacements[i] + j] = pC[i * sz + j];
                        }
                    }
                    seq_t1 = MPI_Wtime();
                    seq_elap += seq_t1 - seq_t0;
                }
            }
            mkl_free(pA);
            mkl_free(pB);
            mkl_free(pC);
            mkl_free(tmp_pC);            

            if (pid == ROOT) {
                total_t1 = MPI_Wtime();
                t1 = MPI_Wtime();
            }
            break;
        }


/*############################################################################*/
/*############################################################################*/            
/*############################################################################*/
/*############################################################################*/
/*############################################################################*/

        case CANNON_OMP:
        {
            if (pid == ROOT) total_t0 = MPI_Wtime();
            //lets assume that dims[0] = dims[1]
            if (dims[0] != dims[1]) {
                printf("\nProcess mesh is not appropriate. Aborting.\n\n");
                exit(EXIT_FAILURE);
            }

            max = arguments.m;
            (max < arguments.k) && (max = arguments.k); 
            (max < arguments.n) && (max = arguments.n);

            int sz = max/dims[0];  // row length per block 

            if(max > sz * dims[0]) { 
                sz += 1;
                max = sz * dims[0];
            }

            int blockSz = sz * sz;

            MPI_Type_contiguous(blockSz, MPI_DOUBLE, &MPI_SUBMATRIX);
            MPI_Type_commit(&MPI_SUBMATRIX);

            pA = (double *) mkl_malloc(blockSz * sizeof(double), 64);
            pB = (double *) mkl_malloc(blockSz * sizeof(double), 64);
            pC = (double *) mkl_malloc(blockSz * sizeof(double), 64);


            if(pA == NULL || pB == NULL || pC == NULL) {
                printf("\nCouldn't allocate memory. Aborting.\n\n");
                mkl_free(pA);
                mkl_free(pB);
                mkl_free(pC);
                exit(EXIT_FAILURE);
            }

            memset(pC, 0, blockSz * sizeof(double));

            if (pid == ROOT) {
                A = (double *) mkl_malloc(max * max * sizeof(double), 64);
                B = (double *) mkl_malloc(max * max * sizeof(double), 64);
                C = (double *) mkl_malloc(max * max * sizeof(double), 64);

                memset(C, 0, max * max * sizeof(double));

                if(A == NULL || B == NULL || C == NULL) {
                    printf("\nCouldn't allocate memory.\n\n");
                    mkl_free(A);
                    mkl_free(B);
                    mkl_free(C);
                    exit(EXIT_FAILURE);
                }

                bool resize = true;
                if(arguments.m == max &&
                    arguments.n == max &&
                    arguments.k == max) {
                    resize = false;
                }

                load_matrix(arguments.pathA, A, arguments.m, arguments.k, max, 
                        resize);
                load_matrix(arguments.pathB, B, arguments.k, arguments.n, max, 
                        resize);

                /* debug dump */
                if(arguments.steps) {
                    char *dump_path = (char *)malloc(
                            (strlen(arguments.debugDir) + 6)
                            * sizeof(char));
                    /* assuming that theres less than 9999 nodes */
                    /* that gives less than 9999 steps to dump   */
                    sprintf(dump_path, "%sA_raw", 
                            arguments.debugDir);
                    save_matrix(dump_path, B,
                        arguments.m, arguments.k, max, true, true);

                    sprintf(dump_path, "%sB_raw", 
                            arguments.debugDir);
                    save_matrix(dump_path, A,
                        arguments.k, arguments.n, max, true, true);

                    free(dump_path);
                }

                total_t0 = MPI_Wtime();
                t0 = MPI_Wtime();

                /* initial shift with procesor ranks */                
                int proclA[numprocs];
                for (int i = 0; i < dims[0]; i++) {
                    for (int j = 0; j < dims[1]; j++) {
                        if (j < dims[1] - i) {
                            proclA[i * dims[1] + j + i] = i * dims[1] + j;
                        } else {
                            proclA[i * dims[1] + j - (dims[1] - i)] = 
                                i * dims[1] + j;
                        }
                    }
                }

                seq_t1 = MPI_Wtime();
                total_elap = seq_t1 - seq_t0;

                for (int proc = numprocs - 1; proc >= 0; proc--) {
                    seq_t0 = MPI_Wtime();

                    int displacements[sz];
                    int start = (proc % dims[1]) * sz + 
                        (proc / dims[1]) * (dims[1] * blockSz);
                    displacements[0] = start;
                    for (int k = 1; k < sz; k++) {              
                        displacements[k] =  displacements[k-1] + sz * dims[1];
                    }

                    int k = 0;
                    for (int i = 0; i < sz; i++) {
                        for (int j = 0; j < sz; j++) {
                            pA[k] = A[displacements[i] + j];                            
                            k++;
                        }
                    }

                    seq_t1 = MPI_Wtime();
                    total_elap += seq_t1 - seq_t0;

                    if(proc != ROOT) {
                        MPI_Send(pA, 1, MPI_SUBMATRIX, proclA[proc], 
                                DISTRIBUTION_A, cartcom);
                    }
                }
            } else {
                MPI_Recv(pA, 1, MPI_SUBMATRIX, ROOT, 
                        DISTRIBUTION_A, cartcom, &status);
            }

            MPI_Barrier(cartcom);

            if(pid == ROOT) {
                seq_t0 = MPI_Wtime();

                int proclB[numprocs];
                for (int j = 0; j < dims[1]; j++) {
                    for (int i = 0; i < dims[0]; i++) {
                        if (i < dims[0] - j) { 
                            proclB[(i + j) * dims[0] + j] = 
                                i * dims[1] + j;
                        } else {
                            proclB[(i - dims[0] + j) * dims[1] + j] = 
                                i * dims[1] + j;
                        }
                    }
                }
                seq_t1 = MPI_Wtime();
                seq_elap += seq_t1 - seq_t0;

                for (int proc = numprocs - 1; proc >= 0; proc--) {
                    seq_t0 = MPI_Wtime();
                    /* indeks w macierzy A/B pierwszego elementu z k-tego wiersza */
                    int displacements[sz]; 
                    int start = (proc % dims[1]) * sz + 
                        (proc / dims[1]) * (dims[1] * blockSz);
                    displacements[0] = start;
                    for (int k = 1; k < sz; k++) {              
                        displacements[k] =  displacements[k-1] + sz * dims[1];
                    }

                    int k = 0;
                    for (int i = 0; i < sz; i++) {
                        for (int j = 0; j < sz; j++) {
                            pB[k] = B[displacements[i] + j];
                            k++;
                        }
                    }

                    seq_t1 = MPI_Wtime();
                    seq_elap += seq_t1 - seq_t0;

                    if(proc != ROOT) {
                        MPI_Send(pB, 1, MPI_SUBMATRIX, proclB[proc], 
                                DISTRIBUTION_B, cartcom);
                    }                    
                }            
            } else {                
                MPI_Recv(pB, 1, MPI_SUBMATRIX, ROOT, 
                        DISTRIBUTION_B, cartcom, &status);
            }
            int ii = 0;
            int j = 0;
            int l = 0;
            #pragma omp parallel for shared(pA, pB, pC) private(j,l) schedule(static)
            for (ii = 0; ii < sz; ii++) {
               for (j = 0; j < sz; j++) {
                    for (l = 0; l < sz; l++) {
                        pC[ii * sz + j] +=
                         pA[ii * sz + l] * pB[l * sz + j];
                    }
                }
            }            

            /* dump data for first step  */
            if(arguments.steps) {
                char *dump_path = (char *)malloc(
                        (strlen(arguments.debugDir) + 12)
                        * sizeof(char));
                /* assuming that theres less than 9999 nodes */
                /* that gives less than 9999 steps to dump   */
                sprintf(dump_path, "%s%04d_0000_A", arguments.debugDir, pid);
                save_matrix(dump_path, pA,
                    sz, sz, sz, true, true);

                sprintf(dump_path, "%s%04d_0000_B", arguments.debugDir, pid);
                save_matrix(dump_path, pB,
                    sz, sz, sz, true, true);

                sprintf(dump_path, "%s%04d_0000_C", arguments.debugDir, pid);
                save_matrix(dump_path, pC,
                    sz, sz, sz, true, true);

                free(dump_path);
            }

            /* skewing */
            int top, bottom, left, right;
            MPI_Cart_shift(cartcom, 1, 1, &left, &right);
            MPI_Cart_shift(cartcom, 0, 1, &top, &bottom);
            
            for (int i = 1; i < dims[0]; i++) { /* if dims[0] == dims[1] */
                MPI_Sendrecv_replace(pA, 1, MPI_SUBMATRIX, left, 
                        SKEW_LEFTRIGHT, right, 
                        SKEW_LEFTRIGHT, cartcom, &status);
                MPI_Sendrecv_replace(pB, 1, MPI_SUBMATRIX, top, 
                        SKEW_BOTTOMUP, bottom, 
                        SKEW_BOTTOMUP, cartcom, &status);

                int j = 0;
                int l = 0;
                #pragma omp parallel for shared(pA, pB, pC) private(j,l) schedule(static)
                for (int ii = 0; ii < sz; ii++) {
                   for (j = 0; j < sz; j++) {
                        for (l = 0; l < sz; l++) {
                            pC[ii * sz + j] += 
                                pA[ii * sz + l] * pB[l * sz + j];
                        }
                    }
                }

                /* dump data for step 1-p */
                if(arguments.steps) {
                    char *dump_path = (char *)malloc(
                            (strlen(arguments.debugDir) + 12)
                            * sizeof(char));
                    /* assuming that theres less than 9999 nodes */
                    /* that gives less than 9999 steps to dump   */
                    sprintf(dump_path, "%s%04d_%04d_A", 
                            arguments.debugDir, pid, i);
                    save_matrix(dump_path, pA,
                        sz, sz, sz, true, true);

                    sprintf(dump_path, "%s%04d_%04d_B", 
                            arguments.debugDir, pid, i);
                    save_matrix(dump_path, pB,
                        sz, sz, sz, true, true);

                    sprintf(dump_path, "%s%04d_%04d_C", 
                            arguments.debugDir, pid, i);
                    save_matrix(dump_path, pC,
                        sz, sz, sz, true, true);

                    free(dump_path);
                }

            }

            MPI_Barrier(cartcom);

            if(pid == ROOT) {
                seq_t0 = MPI_Wtime();
                /* displacement for root process */
                int displacements[sz];
                displacements[0] = 0; 
                for(int k = 1; k < sz; k++) {              
                    displacements[k] =  
                        displacements[k-1] + sz * dims[1];
                }
                for(int i = 0; i < sz; i++) {
                    for(int j = 0; j < sz; j++) {
                        C[displacements[i] + j] = pC[i * sz + j];                        
                    }
                }
                seq_t1 = MPI_Wtime();
                seq_elap += seq_t1 - seq_t0;
            }
            /* sending computed data from all over the grid to root process*/
            for(int proc = 1; proc < numprocs; proc++) {
                if(pid == proc) {
                    MPI_Send(pC, 1, MPI_SUBMATRIX, ROOT, 
                            COLLECTING, cartcom);
                }

                if (pid == ROOT) {
                    MPI_Recv(pC, 1, MPI_SUBMATRIX, proc, 
                            COLLECTING, cartcom, &status);
                    // przesunięcia wierszy dla pozostałych procesów
                    seq_t0 = MPI_Wtime();

                    int displacements[sz];
                    /*  proc % dims[1] * sz - first column */
                    /*  of first line of submatrix stored in p-process */
                    /*  (proc / dims[1])... - horizontal shifts */
                    int start = (proc % dims[1]) * sz +
                        (proc / dims[1]) * (dims[1] * blockSz);
                    displacements[0] = start;

                    for(int k = 1; k < sz; k++) {              
                        displacements[k] =
                            displacements[k-1] + sz * dims[1];
                    }

                    for(int i = 0; i < sz; i++) {
                        for(int j = 0; j < sz; j++) {
                            C[displacements[i] + j] = pC[i * sz + j];
                        }
                    }
                    seq_t1 = MPI_Wtime();
                    seq_elap += seq_t1 - seq_t0;
                }
            }
            mkl_free(pA);
            mkl_free(pB);
            mkl_free(pC);

            if (pid == ROOT) {
                total_t1 = MPI_Wtime();
                t1 = MPI_Wtime();
            }

            break;
        }
    }

    /* comunication time collecting */
    if(pid == ROOT) {    
        switch(arguments.mode) {
            case VERBOSE: {
                if(arguments.method == CANNON || 
                        arguments.method == CANNON_DGEMM ||
                        arguments.method == CANNON_OMP) {
                    for(int i = 0; i < arguments.m; i++) {
                        for(int j = 0; j < arguments.n; j++) {
                            printf("%.0lf\t", C[i * max + j]);
                            if(j == arguments.n - 1) printf("\n");
                        }
                    }
                } else {
                    for(int i = 0; i < arguments.m; i++) {
                        for(int j = 0; j < arguments.n; j++) {
                            printf("%.0lf\t", C[i * arguments.n + j]);
                            if(j == arguments.n - 1) printf("\n");
                        }
                    }
                }
            }
            case QUIET:
            {
                if(arguments.time == true || arguments.mode == VERBOSE)
                    printf("\nETA: %lf\n", t1-t0);

                if (arguments.pathC != NULL) {
                    if(arguments.method == CANNON || 
                        arguments.method == CANNON_DGEMM ||
                        arguments.method == CANNON_OMP) {
                            save_matrix(arguments.pathC, C,
                                arguments.m, arguments.n, max, 
                                false, false);
                    } else {
                        save_matrix(arguments.pathC, C,
                                arguments.m,  arguments.n, arguments.n, 
                                false, false);
                    }
                }

                if(arguments.debugDir != NULL) {
                    char filename[15 + strlen(arguments.debugDir)];             
                    char method[10];
                    
                    sprintf(filename,"%sdebug_%d",
                            arguments.debugDir, (unsigned)time(NULL));

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
                            sprintf(method, "OMP");
                            break;
                        }
                        case 3: {
                            sprintf(method, "CANNON");
                            break;
                        }
                        case 4: {
                            sprintf(method, "CANNON_DGEMM");
                            break;
                        }
                        case 5: {
                            sprintf(method, "CANNON_OMP");
                            break;
                        }
                    }

                    save_info(filename, t1 - t0, method, 
                            arguments.m, arguments.k, arguments.n, numprocs, 
                            arguments.omp_threads, seq_elap, total_t1 - total_t0);
                }
                break;
            }
        }
        mkl_free(A);
        mkl_free(B);
        mkl_free(C);        
    }

    MPI_Comm_free(&cartcom);
    MPI_Finalize();
    return EXIT_SUCCESS;
}
