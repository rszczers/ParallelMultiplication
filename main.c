#include <stddef.h>
#include <malloc.h>
#include <stdio.h>
#include "counter.h"

int main(int argc, char *argv[]) {
	int m = 256;
	int n = 256;		
	double *out = (double *)malloc(256 * 256 * sizeof(double));
    load_matrix("a.dat", out, &m, &n);
	free(out);
    return 0;
}
