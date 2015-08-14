#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int load_matrix(const char *filename, double *out, int m, int n, int max, bool resize) {
    FILE *file = fopen(filename, "r");

    char *buffer;
    int length;
    if(resize) {
        length = max * max;
    } else {
        length = m * n; 
    }

    char sep[] = " ";
    
    if(file == NULL) {
        perror(filename);
        return EXIT_FAILURE;;
    }
        
    fseek(file, 0L, SEEK_END);
    long size = ftell(file); // file size
    rewind(file);
    
    int fg;
    int i = 0;
    int j = 0; 
    int el = 0; // number of elements
    char *token;
    while((el < m * n) && !feof(file)) {
        buffer = (char *)malloc(sizeof(char) * 4096);
        memset(buffer, 0, 4096);
        fg = fscanf(file, "%4095c", buffer);

        if(fg != EOF) {
            token = strtok(buffer, sep);            
            while(token != NULL && el < m*n) {
                out[i] = atof(token);
                token = strtok(NULL, sep);
                i++; // sprawdza czy nie wyleciało z tablicy
                j++; // liczba kolumn czy się zgadza
                el++; 
                if(resize) {
                    if(j >= m) {
                        for(; j < max; j++) {
                            out[i] = 0.0;
                            i++;                        
                        }
                        j = 0;
                    }
                }
            }
//            if(token = NULL) {
//                i--;
//            }
        }           
        free(buffer);   
    }

    while(resize && i < length) {
        out[i] = 0.0;
        i++;
    }


    fclose(file);
    return EXIT_SUCCESS;
}
