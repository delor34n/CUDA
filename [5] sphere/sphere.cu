#include <stdio.h>
#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>
#include <time.h>
#include <helper_cuda.h>
#include <helper_functions.h>

#define POBLACION 100
#define LONG_COD 20
#define LIMITE -5.12
#define CROSS_PROBABILITY 0.3
#define MUTATION_PROBABILITY 0.001
#define INTERVALO 10.24/__powf(2,LONG_COD/2)

#define BLOCKSIZE 256
#define WARPSIZE 32

int N=1024;

#define uint64 unsigned long long

typedef struct {
    char genotipo[LONG_COD];
    float aptitud;
} Individuo;

__device__ void decoder(float * x, float * y, char * genotipo) {
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

__host__ __device__ float fitness (float p1, float p2){
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
void sonMutation(Individuo *sons, curandState *dev_state, int idx){
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
                float x, y;
                point = (int) (((double) LONG_COD)*curand_uniform(&lstate)*(POBLACION-0.00001));
                for(j=point; j<LONG_COD; j++){
                    aux=dev_selection[idx].genotipo[j];
                    dev_selection[idx].genotipo[j]=dev_selection[idx+1].genotipo[j];
                    dev_selection[idx+1].genotipo[j]=aux;
                }

                sonMutation(&dev_selection[idx], dev_state, idx);

                decoder(&x, &y, dev_selection[idx].genotipo);
                dev_selection[idx].aptitud=fitness(x,y);

                decoder(&x, &y, dev_selection[idx+1].genotipo);
                dev_selection[idx+1].aptitud=fitness(x,y);
            }
            dev_state[idx] = lstate;
        }
    }
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
        float x, y;
        curandState lstate = dev_state[idx];
        for(i=0; i<LONG_COD; i++)
            dev_poblacion[idx].genotipo[i] = curand_uniform(&lstate) > 0.5 ? 1.0 : 0.0;
        decoder(&x, &y, dev_poblacion[idx].genotipo);
        dev_poblacion[idx].aptitud = fitness(x,y);
        dev_state[idx] = lstate;
    }
}

/*****************************/
/*****************************/
/*****************************/

__inline__ __device__
float warpAllReduceCompare(float val) {
    for (unsigned int mask = WARPSIZE/2; mask > 0; mask /= 2){
        val = fmax(val,__shfl_down(val, mask, WARPSIZE));
    }
    return val;
}

__inline__ __device__
float blockReduceCompare(float val) {
    static __shared__ float shared[WARPSIZE]; // Shared mem for 32 partial sums
    int lane = threadIdx.x % WARPSIZE;
    int wid = threadIdx.x / WARPSIZE;

    val = warpAllReduceCompare(val);     // Each warp performs partial reduction

    if (lane==0) shared[wid]=val; // Write reduced value to shared memory
        __syncthreads();              // Wait for all partial reductions

    //read from shared memory only if that warp existed
    val = (threadIdx.x < blockDim.x / WARPSIZE) ? shared[lane] : 0;

    if (wid==0) val = warpAllReduceCompare(val); //Final reduce within first warp
        return val;
}


__device__ float atomicMaxf(float* address, float val) {
    int *address_as_int =(int*)address;
    int old = *address_as_int, assumed;
    while (val > __int_as_float(old)) {
        assumed = old;
        old = atomicCAS(address_as_int, assumed,
        __float_as_int(val));
    }
    return __int_as_float(old);
}

__global__
void eliteKernel(Individuo * dev_seleccion){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    float MAX = -1.0f;
    MAX = MAX < dev_seleccion[i].aptitud ? dev_seleccion[i].aptitud : MAX;

    MAX = blockReduceCompare(MAX);
    if(threadIdx.x==0){
        atomicMaxf(&dev_seleccion[0].aptitud, MAX);
    }
}

/*****************************/
/*****************************/
/*****************************/

void print_selection(Individuo *host_seleccion);

int main (void) {
    srand(time(NULL));
    printf("[HOST] Starting script\n");

    int GRIDSIZE = (N+BLOCKSIZE-1)/BLOCKSIZE;
    dim3 block(BLOCKSIZE, 1, 1);
    dim3 grid(GRIDSIZE, 1, 1);

    Individuo BEST;
    unsigned int generation = 0;

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

    do{
        tournamentSelectionKernel<<<grid, block>>>(dev_poblacion, dev_seleccion, dev_state);
        crossSelectionKernel<<<grid, block>>>(dev_poblacion, dev_seleccion, dev_state);
        eliteKernel<<<grid,block>>>(dev_seleccion);
        generation++;

        cudaDeviceSynchronize();
        cudaMemcpy(&BEST, dev_seleccion, sizeof(Individuo), cudaMemcpyDeviceToHost);
    }while(BEST.aptitd > pow(10,-2));

    eliteKernel<<<grid,block>>>(dev_seleccion);
    cudaMemcpy(&BEST, dev_seleccion, sizeof(Individuo), cudaMemcpyDeviceToHost);
    float x, y;
    decoder(&x, &y, BEST.genotipo);

    printf ("*************************************\n");
    printf ("*          FIN DEL ALGORITMO        *\n");
    printf ("*************************************\n");
    printf (" - En el punto (%.5f, %.5f)\n", x, y);
    printf (" - Su fenotipo es %.5f\n", BEST.aptitud);
    printf (" - Es la generacion numero %i\n", generation);
    printf ("*************************************\n");

    free(host_poblacion);
    free(host_seleccion);
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
