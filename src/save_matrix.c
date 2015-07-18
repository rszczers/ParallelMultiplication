#include <stdio.h>
#include <stdlib.h>

int save_matrix(char *filename, double *data, int length) {
	FILE *file = fopen(filename, "wb");	
	if(file == NULL) {
		perror(filename);
		exit(EXIT_FAILURE);
	}
	for(int i = 0; i < length; i++) 
		fprintf(file, "%lf ", data[i]);

	fclose(file);
	return EXIT_SUCCESS;;
}
