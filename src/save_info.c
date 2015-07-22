#include <stdio.h>
#include <stdlib.h>

int save_info(const char *filename, double elapsedTime, const char *method, int m, int k, int n, int nproc) {
    FILE *file = fopen(filename, "wb");
    if(file == NULL)     {
        perror(filename);
        exit(EXIT_FAILURE);
    }
    fprintf(file, "METHOD:\t%s\nETA:\t%lf\nARGS:\tm=%d, k=%d, n=%d, nprocs=%d\n", method, elapsedTime, m, k, n, nproc);

    fclose(file);
    return(EXIT_SUCCESS);
}
