#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "bar.h"

int load_matrix(const char *filename, double *out, int m, int n, int max) {
	printf("%d, %d, %d", m, n, max);
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
	printf("Wszytywanie danych, proszę poczekać...\n");	
	while((i < length) && !feof(file)) {
		buffer = (char *)malloc(sizeof(char) * 4096);
		memset(buffer, 0, 4096);
		fg = fscanf(file, "%4095c", buffer);			
		if(fg != EOF) {
			token = strtok(buffer, sep);			
			while(token != NULL && i < length) {
				out[i] = atof(token);
				printf("%d, %lf\n", i, out[i]);
				token = strtok(NULL, sep);
				i++;
				j++;
				if(j >= n) {
					for(int l = 0; l < (max - n); l++) {
						out[i] = 0;
						printf("%d, %lf\n", i, out[i]);
						i++;						
					}
					j = 0;
					f++;
				}
			}

			if(f < max) {
				for(int l = 0; l < (max - m); l++) {
					for(int h = 0; h < max; h++) {
						out[i] = 0;
						i++;
						printf("%d, %lf\n", i, out[i]);
					}
				}
			}

		}

		progres_bar(i, length);				
		free(buffer);	
	}




	fclose(file);
	printf("\nDane wczytano poprawnie.\n");
	return 0;
}
