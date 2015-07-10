#include <stdio.h>
#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>
#include <time.h>

#define POBLACION 20
#define LONG_COD 20
#define LIMITE -5.12
#define PROB_CRUCE 0.3
#define PROB_MUTACION 0.001
#define INTERVALO 10.24/pow(2,LONG_COD/2)

#define nThreads 2
#define nBlocks 5

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
void tournamentSelectionKernel(Individuo * dev_poblacion, Individuo * dev_selection, curandState *dev_state){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int rand_a = (int)truncf((((POBLACION/nThreads)+1)+0.999999)*curand_uniform(&dev_state[i]));
    int rand_b = (int)truncf((((POBLACION/nThreads)+1)+0.999999)*curand_uniform(&dev_state[i]));

    printf ("\nSELECTION[%d][%d]: dev_selection[%d] = %f", blockIdx.x, threadIdx.x, i, dev_poblacion[i].aptitud);
    if(i<POBLACION){
        Individuo candidato_a, candidato_b;

        candidato_a = dev_poblacion[rand_a];
        candidato_b = dev_poblacion[rand_b];

        if (candidato_a.aptitud < candidato_b.aptitud)
            dev_selection[i] = candidato_a;
        else
            dev_selection[i] = candidato_b;
        //printf ("\nSELECTION[%d][%d]: dev_selection[%d] = %f", blockIdx.x, threadIdx.x, i, dev_selection[i].aptitud);
    }
}

__global__
void crossSelectionKernel(Individuo * dev_selection){
    //int i = blockIdx.x * blockDim.x + threadIdx.x;
    //printf("\nCROSS: dev_selection[%d]: %f", i, dev_selection[i].aptitud);
}

__global__
void eliteKernel(Individuo * dev_poblacion){
    //int i = blockIdx.x * blockDim.x + threadIdx.x;
    //printf("\nELITE: dev_poblacion[%d]\n", i);
}

__global__
void init_rand(curandState *dev_state) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    curand_init(1234, idx, 0, &dev_state[idx]);
}

Individuo generarIndividuo (void);
Individuo * generatePopulation (void);
Individuo init();

Individuo * fillSelection(void);
Individuo initFellow(void);

int generarBinario (void);
void decoder (double *, double *, int *);
void print_selection(Individuo *host_seleccion);

int main (void) {
    srand(time(NULL));
    printf("[HOST] Starting script\n");

    Individuo * host_seleccion = fillSelection(), * host_poblacion = generatePopulation();
    Individuo * dev_seleccion, * dev_poblacion;

    print_selection(host_poblacion);

    /*
    * Random initialization.
    **/
    curandState *dev_state;
    cudaMalloc(&dev_state, nThreads*nBlocks);
    init_rand<<<nBlocks, nThreads>>>(dev_state);

    cudaMalloc((void**)&dev_poblacion, sizeof(Individuo)*POBLACION);
    cudaMalloc((void**)&dev_seleccion, sizeof(Individuo)*POBLACION/2);
    cudaMemcpy(dev_poblacion, host_poblacion, sizeof(Individuo)*POBLACION, cudaMemcpyHostToDevice);
    cudaMemcpy(dev_seleccion, host_seleccion, sizeof(Individuo)*POBLACION/2, cudaMemcpyHostToDevice);

    tournamentSelectionKernel<<<nBlocks, nThreads>>>(dev_poblacion, dev_seleccion, dev_state);
    cudaMemcpy(host_seleccion, dev_seleccion, sizeof(Individuo)*POBLACION/2, cudaMemcpyDeviceToHost);
    print_selection(host_seleccion);
    printf("\n");
    //crossSelectionKernel<<<nBlocks, nThreads>>>(dev_seleccion);

    cudaDeviceSynchronize();

    cudaFree(dev_poblacion);
    cudaFree(dev_seleccion);
    cudaFree(dev_state);

    cudaDeviceReset();
    return 0;
}

void print_selection(Individuo *host_seleccion){
    int i;
    for(i=0; i<POBLACION; i++)
        printf("\nhost_seleccion[%d] = %f", i, host_seleccion[i].aptitud);
}

Individuo * fillSelection(void) {
    Individuo * aux;
    int i;

    aux = (Individuo *) malloc(sizeof(Individuo)*POBLACION/2);
    for(i=0;i<POBLACION/2;i++)
        aux[i] = initFellow();
    return aux;
}

Individuo initFellow(void){
    Individuo ind;
    int i;
    double x, y;

    for (i=0; i<LONG_COD; i++)
        ind.genotipo[i]=generarBinario();

    decoder(&x, &y, ind.genotipo);
    ind.aptitud = 0;

    return ind;
}

/* PROC generarPoblacion (void) DEV (Individuo *)
 * MODIFICA nada
 * EFECTO esta funcion genera una poblacion con la cantidad de individuos dada por la
 *  macro POBLACION. para generar cada individuo utiliza la funcion generarIndividuo()
 *  y una vez ha terminado el bucle, devuelve el puntero al primer individuo
 */
Individuo * generatePopulation(void) {
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