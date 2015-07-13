#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "bar.h"

int load_matrix(const char *filename, double *out, int m, int n, int max) {
	FILE *file = fopen(filename, "r");

	char *buffer;
	int length = max * max;;
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
//	printf("Loading data, please wait...\n");	
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
					if(j >= n) {
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
//			progres_bar(i, length);				
			free(buffer);	
	}

	while(i < length) {
		out[i] = 0;
		i++;
		progres_bar(i, length);				
	}


	fclose(file);
//	printf("\nDane wczytano poprawnie.\n");
	return 0;
}
