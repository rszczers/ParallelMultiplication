#include <stdio.h>
#include <stdlib.h>
#include "mkl.h"

int sequential_product(double *A, double *B, double *C, int m, int k, int n, double alpha, double beta) {
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                m, n, k, alpha, A, k, B, n, beta, C, n);
    return(0);
}