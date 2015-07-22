#include <stdio.h>
#include <stdlib.h>

int save_matrix(char *filename, double *data, int y, int x) {
    FILE *file = fopen(filename, "wb"); 
    if(file == NULL) {
        perror(filename);
        return EXIT_FAILURE;
    }
    for(int i = 0; i < y; i++) {
        for(int j = 0; j < x; j++) {
            fprintf(file, "%lf ", data[i * x + j]);
        }
    }

    fclose(file);
    return EXIT_SUCCESS;;
}
