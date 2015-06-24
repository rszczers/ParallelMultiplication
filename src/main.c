#include <stddef.h>
#include <malloc.h>
#include <stdio.h>
#include "load_matrix.h"

int main(int argc, char *argv[]) {
	int m = 256;
	int n = 256;		
	double *out = (double *)malloc(256 * 256 * sizeof(double));
    load_matrix("./resources/a.dat", out, &m, &n);
	free(out);
    return 0;
}
