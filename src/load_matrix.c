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
	int progress;

	char sep[] = " ";
	
	if(file == NULL) {
		perror(filename);
		return -1;
	}
		
	fseek(file, 0L, SEEK_END);
	long size = ftell(file); // file size
	rewind(file);
	
	int fg;
	int i = 0;
	int j = 0; 
	int f = 0; // number of columns read
	char *token;
	while((i < length) && !feof(file)) {
			buffer = (char *)malloc(sizeof(char) * 4096);
			memset(buffer, 0, 4096);
			fg = fscanf(file, "%4095c", buffer);

			if(fg != EOF) {
				token = strtok(buffer, sep);			
				while(token != NULL && i < length) {
					out[i] = atof(token);
					token = strtok(NULL, sep);
					i++;
					j++;
					if(resize && j >= n) {
						for(; j < max; j++) {
							out[i] = 0;
							i++;						
						}
						j = 0;
						f++;
					}
				}
				if(token == NULL) {
					i--;
				}
			}			
			free(buffer);	
	}

	while(resize && i < length) {
		out[i] = 0;
		i++;
	}


	fclose(file);
	return 0;
}
