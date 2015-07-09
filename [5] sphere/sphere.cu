#include <stdio.h>
#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>
#include <time.h>

#define POBLACION 100
#define LONG_COD 20
#define LIMITE -5.12
#define PROB_CRUCE 0.3
#define PROB_MUTACION 0.001
#define INTERVALO 10.24/pow(2,LONG_COD/2)

#define nThreads 10
#define nBlocks 10

typedef struct {
    int genotipo[LONG_COD];
    double aptitud;
} Individuo;

/* PROC fitness (double p1, double p2) DEV (double)
 * MODIFICA nada
 * EFECTO recibe dos valores que representan los puntos que caracterizan a un individuo
 *  la funcion sirve para calcular la aptitud o fitness de cierto individuo segun sus
 *  puntos. este valor de aptitud es el que devuelve la funcion.
 */
__host__ __device__ double fitness (double p1, double p2){
    return pow(p1,2) + pow(p2,2);
}

__global__
void tournamentSelectionKernel(Individuo * dev_poblacion, Individuo * dev_seleccion, curandState *dev_state){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int rand_a = (int)truncf((((POBLACION/nThreads)-1)+0.999999)*curand_uniform(&dev_state[i]));
    int rand_b = (int)truncf((((POBLACION/nThreads)-1)+0.999999)*curand_uniform(&dev_state[i]));
    printf("\nSELECTION[%d]: dev_poblacion[%d]: %f, RANDOM 1 = %d, RANDOM 2 = %d", threadIdx.x, i, dev_poblacion[i].aptitud, rand_a, rand_b);

    /*if(i<POBLACION){
        Individuo candidato_a, candidato_b;

        candidato_a = dev_poblacion[(int) (((double) POBLACION)*rand()/(RAND_MAX+1.0))];
        candidato_b = dev_poblacion[(int) (((double) POBLACION)*rand()/(RAND_MAX+1.0))];

        if (candidato_a.aptitud < candidato_b.aptitud)
            dev_seleccion[i] = candidato_a;
        else
            dev_seleccion[i] = candidato_b;
    }*/
}

__global__
void crossSelectionKernel(Individuo * dev_seleccion){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    printf("\nCROSS: antes dev_seleccion[%d]: %f", i, dev_seleccion[i].aptitud);
    dev_seleccion[i].aptitud = 3.14151617;
    printf("\nCROSS: despues dev_seleccion[%d]: %f", i, dev_seleccion[i].aptitud);
}

__global__
void eliteKernel(Individuo * dev_poblacion){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    printf("\nELITE: dev_poblacion[%d]\n", i);
}

__global__
void init_rand(curandState *dev_state) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    curand_init(1234, idx, 0, &dev_state[idx]);
}

Individuo generarIndividuo (void);
Individuo * generarPoblacion (void);
Individuo init();

int generarBinario (void);
void decoder (double *, double *, int *);

int main (void) {
    srand(time(NULL));
    printf("[HOST] Starting script\n");

    Individuo * host_seleccion= generarPoblacion(), * host_poblacion = generarPoblacion();
    Individuo * dev_seleccion, * dev_poblacion;

    /*
    * Random initialization.
    **/
    curandState *dev_state;
    cudaMalloc(&dev_state, nThreads*nBlocks);
    init_rand<<<nThreads,nBlocks>>>(dev_state);

    cudaMalloc((void**)&dev_poblacion, sizeof(Individuo)*POBLACION);
    cudaMalloc((void**)&dev_seleccion, sizeof(Individuo)*POBLACION);
    cudaMemcpy(dev_poblacion, host_poblacion, sizeof(Individuo)*POBLACION, cudaMemcpyHostToDevice);
    cudaMemcpy(dev_seleccion, host_seleccion, sizeof(Individuo)*POBLACION, cudaMemcpyHostToDevice);

    tournamentSelectionKernel<<<nThreads,nBlocks>>>(dev_poblacion, dev_seleccion, dev_state);
    cudaMemcpy(dev_seleccion, host_seleccion, sizeof(Individuo)*POBLACION, cudaMemcpyDeviceToHost);
    crossSelectionKernel<<<nThreads,nBlocks>>>(dev_seleccion);

    cudaDeviceSynchronize();

    cudaFree(dev_poblacion);
    cudaFree(dev_seleccion);
    cudaFree(dev_state);

    cudaDeviceReset();
    return 0;
}

/* PROC generarPoblacion (void) DEV (Individuo *)
 * MODIFICA nada
 * EFECTO esta funcion genera una poblacion con la cantidad de individuos dada por la
 *  macro POBLACION. para generar cada individuo utiliza la funcion generarIndividuo()
 *  y una vez ha terminado el bucle, devuelve el puntero al primer individuo
 */
Individuo * generarPoblacion(void) {
    Individuo * poblacion;
    int i;

    poblacion = (Individuo *) malloc(sizeof(Individuo)*POBLACION);
    for(i=0;i<POBLACION;i++)
        poblacion[i] = generarIndividuo();

    return poblacion;
}

/* PROC generarIndividuo (void) DEV (Individuo)
 * MODIFICA nada
 * EFECTO se encarga de generar un individuo utilizando valores aleatorios. primero crea
 *  la cadena de bits del genotipo utilizando la funcion generaBinario y luego evalua
 *  la aptitud del individuo utilizando la funcion decoder para decodificar el genotipo
 *  y la funcion fitness para obtener la aptitud.
 */
Individuo generarIndividuo (void){
    Individuo ind;
    int i;
    double x, y;

    for (i=0; i<LONG_COD; i++)
        ind.genotipo[i]=generarBinario();

    decoder(&x, &y, ind.genotipo);
    ind.aptitud = fitness(x,y);

    return ind;
}

/* PROC generarBinario (void) DEV (void)
 * MODIFICA nada
 * EFECTO se encarga de devolver un valor entero que siempre sera cero o uno. lo vamos a
 *  utilizar para generar los individuos al principio dado que su genoma es una cadena
 *  binaria que se genera aleatoriamente
 */
int generarBinario (void) {
    if (1 + (int) (10.0*rand()/(RAND_MAX+1.0)) > 5)
        return 1;
    else
        return 0;
}

/* PROC decoder (double *x, double *y, int *genotipo) DEV (double)
 * MODIFICA (double *x double *y)
 * EFECTO recibe un vector de enteros compuesto de 0 y 1 que representa dos numeros
 *  codificados en binario. se encarga de convertir estos dos numeros binarios a su
 *  equivalente en decimal con ayuda de la macro INTERVALO (incremento del valor entre
 *  cada binario) y la macro LIMITE que es el valor del limite inferior de la repre-
 *  sentacion que en el problema es -5.12
 */
void decoder (double * x, double * y, int * genotipo) {
    int i;
    *x = *y = 0.0;

    // calculo del primer decimal
    for(i=0; i<LONG_COD/2; i++)
        *x += genotipo[i] * pow(2, (LONG_COD/2)-(i+1));
    *x = (*x) * INTERVALO + LIMITE;

    //calculo del segundo decimal
    for(;i<LONG_COD;i++)
        *y += genotipo[i] * pow(2, LONG_COD-(i+1));
    *y = (*y) * INTERVALO + LIMITE;
}