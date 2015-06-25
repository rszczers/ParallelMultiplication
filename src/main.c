#include <stdlib.h>
#include <stdio.h>
#include <argp.h>
#include <string.h>

#include "load_matrix.h"

const char *argp_program_version = "pmm v0.1";
const char *argp_program_bug_address = "<rafal.szczerski@gmail.com>";
static char doc[] = "Parallel matrix multiplication with Intel Math Kernel library";
static char args_doc[] = "[FILENAME] [FILENAME]";

static struct argp_option options[] = {
	{ "method", 'a', "NAME", 0, "Algorithm used."},
    { "inputA", 'A', "FILE", 0, "Path to input FILE containing matrix A data."},
    { "inputB", 'B', "FILE", 0, "Path to input FILE containing matrix B data."},
    { "output", 'o', "FILE", 0, "Path to output FILE containing matrix C data."},
    { 0, 'm', 0, 0, "m."},
	{ 0, 'k', 0, 0, "k."},
	{ 0, 'n', 0, 0, "n."},
    { 0 } 
};

struct arguments {
	enum {NAIVE, STRASSEN, CANNON} method;
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
			char *name = (char *)malloc(8 * sizeof(char));

			for (int i = 0; i<9 ; arg++) {
				name[i] = tolower(*arg);
				i++;
			}

			if(strcmp("strassen", name) == 0)
				arguments->method = STRASSEN;
			else if(strcmp("cannon", name) == 0)
				arguments->method = CANNON;
			// NAIVE is set as default

			printf("%d\n", arguments->method);
			free(name);			
			break;
		}
		case 'A': {
			break;
		}
		case 'o':
			break;
		case 'm':
			break;
		case 'n':
			break;
		case 'k':
			break;
    case ARGP_KEY_ARG:
			//	argz_add(&->argz, &a->argz_len, arg);
			break;	
    default: return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char *argv[]) {
	struct arguments arguments;
	arguments.method = NAIVE;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);
		
	//int m = 256;
	//int n = 256;		

	//double *out = (double *)malloc(256 * 256 * sizeof(double));
	//load_matrix("./resources/a.dat"", out, &m, &n);
	//free(out);
  return 0;
}
