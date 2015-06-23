#include <stdio.h>

void progres_bar(int i, int size) {
	float progres = ((float)i)/((float)size);
	int perc = progres*100.0;
	// printf("%f\n", progres);
	float barWidth = 70.0;
	printf("[");
	int pos = (int)(barWidth * progres);
	for (int j = 0; j < barWidth; ++j) {
		if(j < pos) printf("=");
		else if (j == pos) printf(">");
		else printf(" ");
	}
	printf("] %i%%\r", perc);
	fflush(stdout);
}