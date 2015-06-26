#include <stdlib.h>
#include <stdio.h>
#include <argp.h>
#include <string.h>

#include "load_matrix.h"

const char *argp_program_version = "pmm v0.1";
const char *argp_program_bug_address = "<rafal.szczerski@gmail.com>";
static char doc[] = "Parallel matrix multiplication with Intel Math Kernel Library";
static char args_doc[] = "[FILENAME]";

static struct argp_option options[] = {
    { "method", 'a', "METHOD", 0, "Algorithm used."},
    { "inputA", 'A', "FILE", 0, "Path to input FILE containing matrix A data."},
    { "inputB", 'B', "FILE", 0, "Path to input FILE containing matrix B data."},
    { "output", 'o', "FILE", OPTION_ARG_OPTIONAL, "Path to output FILE containing matrix C data."},
    { 0, 'm', "NUM", 0, "m."},
    { 0, 'k', "NUM", 0, "k."},
    { 0, 'n', "NUM", 0, "n."},
    { "list", 'l', 0, 0, "Show list of available algorithms."},
    { "time", 't', 0, 0, "Show elapsed time."},
    { "quiet", 't', 0, 0, "Do not show any computations."},
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

            for (int i = 0; i<9 ; arg++, i++)
                name[i] = tolower(*arg);

            if(strcmp("strassen", name) == 0)
                arguments->method = STRASSEN;
            else if(strcmp("cannon", name) == 0)
                arguments->method = CANNON;
			else if(strcmp("sequential", name) == 0)
                arguments->method = SEQUENTIAL;
            // NAIVE is set as default
            printf("%d\n", arguments->method);
            
            free(name);         
            break;
        }
        case 'A': {
            arguments->pathA = arg;
            break;
        }
        CASE 'B': {
            arguments->pathB = arg;
            break;
        }
        case 'l': {
            printf("Available methods:\n");
            char *availableMethods[] = {"sequential", "naive", "strassen", "cannon"};
            for (int i = 0; i <= CANNON; ++i)
                printf("%d - %s\n", i, availableMethods[i]);
            break;            
        }
        case 'o':
            arguments->pathC = arg;
            break;
        case 'm':
            arguments->m = arg;
            break;
        case 'n':
            arguments->n = arg;
            break;
        case 'k':
            arguments->k = arg;
            break;
        case ARGP_KEY_ARG:
            break;  
        default: return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char *argv[]) {
    struct arguments arguments;
    arguments.method = NAIVE;
    arguments.mode = VERBOSE;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);
        
    //int m = 256;
    //int n = 256;      

    //double *out = (double *)malloc(256 * 256 * sizeof(double));
    //load_matrix("./resources/a.dat"", out, &m, &n);
    //free(out);
  return 0;
}
