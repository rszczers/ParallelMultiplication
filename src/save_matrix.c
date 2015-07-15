#include <stdio.h>
#include <stdlib.h>

int save_matrix(char *filename, double *data, int length) {
	FILE *file = fopen(filename, "wb");	

	for(int i = 0; i < length; i++) 
		fprintf(file, "%lf ", data[i]);

	fclose(file);
	return 0;
}
