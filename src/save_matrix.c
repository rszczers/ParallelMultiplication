#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int save_matrix(char *filename, double *data, int y, int x, int max,
        bool newlines) {
    FILE *file = fopen(filename, "wb"); 
    if(file == NULL) {
        perror(filename);
        return EXIT_FAILURE;
    }
    for(int i = 0; i < y; i++) {
        for(int j = 0; j < x; j++) {
            fprintf(file, "%.0lf ", data[i * max + j]);
            if(newlines && ((i * max + j) % max == max - 1)) {
                fprintf(file, "\n");
            }
        }
    }

    fclose(file);
    return EXIT_SUCCESS;;
}
