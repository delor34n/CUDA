#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define POBLACION 10
#define N 100

typedef struct {
	int **B;
	int *aptitud;
} Poblacion;

Poblacion init_poblacion(Poblacion poblacion);

int main(int argc, char **argv){
	srandr(time(NULL));
	int a = 327679;

	/*
	 * LEFT SHIFT:
	 * a * 2^k
	 * */
	printf("%d\n", a << 2);

	/*
	 * RIGHT SHIFT:
	 * a / 2^k
	 * */
	printf("%d\n", a >> 2);
	printf("%d\n", ~a);

	Poblacion poblacion;
	poblacion = init_poblacion(poblacion);
}

Poblacion init_poblacion(Poblacion poblacion){
	poblacion.B = (int **) malloc (sizeof(Poblacion)*POBLACION);
	int i,j;
	for(i=0;i<POBLACION;i++){
		poblacion.B[i] = (int *) malloc (sizeof(int)*N);
		for(j=0;j<N;j++)
			poblacion.B[i][j] = (10.0*rand()/(RAND_MAX+1.0);
	}
	poblacion.aptitud = (int *) malloc (sizeof(Poblacion)*POBLACION);
}
