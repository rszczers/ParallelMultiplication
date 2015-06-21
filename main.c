#include <loader.h>

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

int matrix_load(const char* filename, int rows, int cols, int xoffset, 
		int yoffset, float** result) {

  FILE *file = fopen(filename, "r"); 
	char* buffer;
	int ret, row = 0;

	char delims[] = " \t";
	char *result = NULL;


	// alokacja pamięci
	float **matrix = (float **)malloc(rows * sizeof(float *));
	for (int i = 0; i < rows; ++i) {
		matrix[i]	= (float *)malloc(cols * sizeof(float));
	}

	if (file == NULL) {
		perror(filename);
		return -1;
	}

	while(!feof(file)) {
		buffer = malloc(sizeof(char) * 4096);
		memset(buffer, 0, 4096);
		ret = fscanf(file, "%4095[^\n]\n", buffer);
		if (ret != EOF) {
			int field = 0;
			result = strtok(buffer, delims);
			while(result != NULL) {
				if (field > 4)
					break;
				matrix[row][field] = atof(result);
				result = strtok(NULL, delims);
				field++;
			}
			++row;
		}
		free(buffer);
	}

	fclose(file);
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < cols; j++) {
			printf("%f%s", matrix[i][j], j < cols-1 ? "\t" : "\n");
		}
    free(matrix[i]);
	}
	free(matrix);
	return 0;
}
