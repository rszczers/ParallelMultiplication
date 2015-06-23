#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

int load_matrix(const char *filename, double *out, int *m, int *n) {
	FILE *file = fopen(filename, "r");

	char *buffer;
	int length = (*m) * (*n);
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
	char *token;

	while(i < length && !feof(file)) {
		buffer = (char *)malloc(sizeof(char) * 4096);
		memset(buffer, 0, 4096);
		fg = fscanf(file, "%4095c", buffer);			
		if(fg != EOF) {
			token = strtok(buffer, sep);			
			while(token != NULL && i < length) {
				out[i] = atof(token);				
				token = strtok(NULL, sep);			
				printf("%d, %lf\n", i, out[i]);
				i++;	
			}	
			i++;
		}		
		free(buffer);	
	}

	fclose(file);

	return 0;
}
