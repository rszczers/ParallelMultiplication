#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int load_matrix(const char *filename, double *out, int m, int n, int max, bool resize) {
    int fd;
    struct stat fstatus;
    char *pmap;

    fd = open(filename, O_RDWR);

    if(fd == -1) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    if(fstat(fd, &fstatus) < 0) {
        perror(filename);
        close(fd);
        exit(EXIT_FAILURE);
    }

    pmap = (char *)mmap(0, fstatus.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

    if(pmap == MAP_FAILED) {
        perror(filename);
        close(fd);
        exit(EXIT_FAILURE);
    }

    int length;
    if(resize) {
        length = max * max;
    } else {
        length = m * n; 
    }

    char sep[] = " ";
    
    int i = 0;
    int j = 0; 
    int el = 0; // number of elements
    char *token;
    token = strtok(pmap, sep); 

    while(token != NULL && el < m * n) {
        out[i] = atof(token);
        if(el < m * n - 1) {
            token = strtok(NULL, sep);
        }
        i++;
        el++; 
        if(resize) {
            j++; // column number indicator 
            if(j == n) {
                for(; j < max; ++j) {
                    out[i] = 0.0;
                    i++;                        
                }
                j = 0;
            }
        }

    }
    while(resize && i < length) {
        out[i] = 0.0;
        i++;
    }

    munmap(pmap, fstatus.st_size);
    close(fd);
    return EXIT_SUCCESS;
}
