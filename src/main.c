#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <argp.h>
#include <argz.h>
#include <stdbool.h>

#include "load_matrix.h"

const char *argp_program_version = "pmm 0.1";
const char *argp_program_bug_address = "<rafal.szczerski@gmail.com>";
static char doc[] = "Parallel matrix multiplication";
static char args_doc[] = "[FILENAME] [FILENAME]";

static struct argp_option options[] = { 
		{ "algorithm", 'a', 0, "OPTION_ARG_OPTIONAL", "Algorithm used."},
    { "input", 'i', "PATH", 0, "Input matrices: A(m x k), B(k x n)."},
    { "output", 'o', "PATH", 0, "Path to output file."},
    { 0, 'm', 0, 0, "m."},
		{ 0, 'k', 0, 0, "k."},
		{ 0, 'n', 0, 0, "n."},
		{ 0, 'h', 0, OPTION_HIDDEN, 0 },
    { 0 } 
};

struct arguments {
	char *argz;
	size_t argz_len;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *a = state->input;
    switch (key) {
    case ARGP_KEY_ARG: return 0;
    default: return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char *argv[]) {

	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	
	int m = 256;
	int n = 256;		
	if(argc >= 2) {
		printf("%s\n", argv[1]);
	}

	double *out = (double *)malloc(256 * 256 * sizeof(double));
    load_matrix("./resources/a.dat", out, &m, &n);
	free(out);
    return 0;
}
