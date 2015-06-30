#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>

#include "load_matrix.h"
#include "save_matrix.h"

//#include "sequential.h"

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
            // if(state->arg_num < 5)
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

    double *A = (double *)malloc(arguments.m * arguments.k * sizeof(double));
    double *B = (double *)malloc(arguments.k * arguments.n * sizeof(double));
    double *C = (double *)malloc(arguments.m * arguments.n * sizeof(double));
    load_matrix(arguments.pathA, A, arguments.m, arguments.k);
    load_matrix(arguments.pathB, B, arguments.k, arguments.n);
    //load_matrix(arguments.pathC, C, arguments.m, arguments.n);

    switch(arguments.method) {
        case SEQUENTIAL:
        {               
//            sequential_product(A, B, C, arguments.m, arguments.k, arguments.m, 1.0, 0.0);
            break;
        }
        case NAIVE:
        {
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

    switch(arguments.mode) {
        case VERBOSE:
        {
            for (int i = 0; i <= arguments.m; ++i)            
                for (int j = 0; j <= arguments.n; ++j)
                    printf("%lf ", C[i*(j+1)]);
                printf("\n");
        }
        case QUIET:
        {            
            if (arguments.pathC != NULL)
            {
                save_matrix(arguments.pathC, A, arguments.m * arguments.k);            
            }
            break;
        }
    }
  
    free(A);
    free(B);
    free(C);
    return EXIT_SUCCESS;
}
