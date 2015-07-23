#include <stdio.h>
#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>
#include <time.h>

#define POBLACION 1000
#define LONG_COD 20
#define LIMITE -5.12
#define CROSS_PROBABILITY 0.3
#define MUTATION_PROBABILITY 0.001
#define INTERVALO 10.24/__powf(2,LONG_COD/2)

#define BLOCKSIZE 128

typedef struct {
    char genotipo[LONG_COD];
    double aptitud;
} Individuo;

__device__ void decoder(double * x, double * y, char * genotipo) {
    int i;
    *x = *y = 0.0;

    // calculo del primer decimal
    for(i=0; i<LONG_COD/2; i++)
        *x += (int)(genotipo[i]) * __powf(2, (LONG_COD/2)-(i+1));
    *x = (*x) * INTERVALO + LIMITE;

    //calculo del segundo decimal
    for(;i<LONG_COD;i++)
        *y += (int)(genotipo[i]) * __powf(2, LONG_COD-(i+1));
    *y = (*y) * INTERVALO + LIMITE;
}

__host__ __device__ double fitness (double p1, double p2){
    return (p1*p1) + (p2*p2);
}

__global__
void tournamentSelectionKernel(Individuo * dev_poblacion, Individuo * dev_selection, curandState *dev_state){
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if(idx<POBLACION){
        curandState lstate = dev_state[idx];

        Individuo candidato_a, candidato_b;

        candidato_a = dev_poblacion[(int) (curand_uniform(&lstate)*(POBLACION-0.00001))];
        candidato_b = dev_poblacion[(int) (curand_uniform(&lstate)*(POBLACION-0.00001))];

        if (candidato_a.aptitud < candidato_b.aptitud)
            dev_selection[idx] = candidato_a;
        else
            dev_selection[idx] = candidato_b;

        dev_state[idx] = lstate;
    }
}

__device__
void sonMutationBlade(Individuo *sons, curandState *dev_state, int idx){
    int i,j;
    double randProbability;
    curandState lstate = dev_state[idx];
    for(i=0;i<2;i++)
        for(j=0;j<LONG_COD;j++)
            randProbability = (((double) LONG_COD)*curand_uniform(&lstate)*(POBLACION-0.00001));
            if(randProbability<MUTATION_PROBABILITY){
                if(sons[i].genotipo[j])
                    sons[i].genotipo[j]=0;
                else
                    sons[i].genotipo[j]=1;
            }
    dev_state[idx] = lstate;
}

__global__
void crossSelectionKernel(Individuo * dev_poblacion, Individuo * dev_selection, curandState *dev_state){
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if(idx<POBLACION-1){
        if(idx==0 || !idx%2){
            curandState lstate = dev_state[idx];
            double crossProbability = (((double) LONG_COD)*curand_uniform(&lstate)*(POBLACION-0.00001));
            if(crossProbability < MUTATION_PROBABILITY){
                int point, j, aux;
                point = (int) (((double) LONG_COD)*curand_uniform(&lstate)*(POBLACION-0.00001));
                for(j=point; j<LONG_COD; j++){
                    aux=dev_selection[idx].genotipo[j];
                    dev_selection[idx].genotipo[j]=dev_selection[idx+1].genotipo[j];
                    dev_selection[idx+1].genotipo[j]=aux;
                }

                sonMutationBlade(&dev_selection[idx], dev_state, idx);
            }
            dev_state[idx] = lstate;
        }
    }
}

__global__
void eliteKernel(Individuo * dev_poblacion){
    //int i = blockIdx.x * blockDim.x + threadIdx.x;
    //printf("\nELITE: dev_poblacion[%d]\n", i);
}

__global__
void init_rand(curandState *dev_state, int seed) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if(idx < POBLACION)
        curand_init(idx + seed, 0, 0, &dev_state[idx]);
}

__global__
void init_poblacion(Individuo * dev_poblacion, curandState *dev_state){
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if(idx < POBLACION){
        int i;
        double x, y;
        curandState lstate = dev_state[idx];
        for(i=0; i<LONG_COD; i++)
            dev_poblacion[idx].genotipo[i] = curand_uniform(&lstate) > 0.5 ? 1.0 : 0.0;
        decoder(&x, &y, dev_poblacion[idx].genotipo);
        dev_poblacion[idx].aptitud = fitness(x,y);
        dev_state[idx] = lstate;
    }
}

void print_selection(Individuo *host_seleccion);

int main (void) {
    srand(time(NULL));
    printf("[HOST] Starting script\n");

    int GRIDSIZE = (POBLACION+BLOCKSIZE-1)/BLOCKSIZE;
    dim3 block(BLOCKSIZE, 1, 1);
    dim3 grid(GRIDSIZE, 1, 1);

    /*
    * Random initialization.
    **/
    curandState *dev_state;
    cudaMalloc(&dev_state, sizeof(curandState)*POBLACION);
    init_rand<<<grid, block>>>(dev_state, rand());

    Individuo * host_seleccion, * host_poblacion;
    Individuo * dev_seleccion, * dev_poblacion;

    host_poblacion = (Individuo *) malloc (sizeof(Individuo)*POBLACION);
    host_seleccion = (Individuo *) malloc (sizeof(Individuo)*POBLACION);
    cudaMalloc((void**)&dev_poblacion, sizeof(Individuo)*POBLACION);
    cudaMalloc((void**)&dev_seleccion, sizeof(Individuo)*POBLACION);

    init_poblacion<<<grid, block>>>(dev_poblacion, dev_state);
    tournamentSelectionKernel<<<grid, block>>>(dev_poblacion, dev_seleccion, dev_state);
    crossSelectionKernel<<<grid, block>>>(dev_poblacion, dev_seleccion, dev_state);

    cudaMemcpy(host_poblacion, dev_poblacion, sizeof(Individuo)*POBLACION, cudaMemcpyDeviceToHost);
    cudaMemcpy(host_seleccion, dev_seleccion, sizeof(Individuo)*POBLACION, cudaMemcpyDeviceToHost);
    printf("\n");

    cudaDeviceSynchronize();

    cudaFree(dev_poblacion);
    cudaFree(dev_seleccion);
    cudaFree(dev_state);

    cudaDeviceReset();
    return 0;
}

void print_selection(Individuo *host_seleccion){
    int i, j;
    for(i=0; i<POBLACION; i++){
        printf("\nhost_seleccion[%d] = %f", i, host_seleccion[i].aptitud);
        for(j=0; j<LONG_COD; j++){
            printf("\nhost_genotipo[%d] = %d", i, host_seleccion[i].genotipo[j]);
        }
    }

}
