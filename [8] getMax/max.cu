#include <stdio.h>
#include <cuda.h>

#define N 26
#define BLOCKSIZE 8
#define WARPSIZE 32
#define uint64 unsigned long long

typedef struct {
    char id[1];
    double num;
} Dato;

__inline__ __device__
double warpAllReduceCompare(double val) {
    for (unsigned int mask = WARPSIZE/2; mask > 0; mask /= 2){
        val = fmax(val,__shfl_down(__double2float_rd(val), mask, WARPSIZE));
    }
    return val;
}

__inline__ __device__
double blockReduceCompare(double val) {
    static __shared__ double shared[WARPSIZE]; // Shared mem for 32 partial sums
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


__device__ void AtomicMax(double * const address, const double value) {
    if (* address >= value)
        return;
    uint64 * const address_as_i = (uint64 *)address;
    uint64 old = * address_as_i, assumed;
    do {
        assumed = old;
        if (__longlong_as_double(assumed) >= value)
            break;
        old = atomicCAS(address_as_i, assumed, __double_as_longlong(value));
    } while (assumed != old);
}

__global__
void eliteKernel(Dato * device_datos){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int k = i;
    double MAX = -1;
    for(;i<N;i+=blockDim.x * gridDim.x){
        MAX = MAX < device_datos[i].num ? device_datos[i].num : MAX;
    }

    MAX = blockReduceCompare(MAX);
    if(threadIdx.x & (warpSize-1)==0)
        AtomicMax(&device_datos[k].num,MAX);
}

void fill(Dato *host_datos);

int main(void){
    srand(time(NULL));
    printf("[HOST] STARTING SCRIPT\n");

    Dato *host_datos;
    Dato *device_datos;

    host_datos = (Dato *) malloc (sizeof(Dato)*N);
    device_datos = (Dato *) malloc (sizeof(Dato)*N);
    cudaMalloc((void**)&device_datos, sizeof(Dato)*N);

    fill(host_datos);

    int GRIDSIZE = (N/4+BLOCKSIZE-1)/BLOCKSIZE;
    dim3 block(BLOCKSIZE, 1, 1);
    dim3 grid(GRIDSIZE, 1, 1);

    cudaMemcpy(host_datos, device_datos, sizeof(Dato)*N, cudaMemcpyHostToDevice);
    eliteKernel<<<grid,block>>>(device_datos);
    cudaMemcpy(device_datos, host_datos, sizeof(Dato)*N, cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
    cudaDeviceReset();

    printf("\nMAX = %s[%f]\n\n", host_datos[0].id, host_datos[0].num);
    printf("[HOST] SCRIPT EXECUTION FINISHED\n");
    return 0;
}

void fill(Dato *host_datos){
    int i;
    int letter=97;
    for(i=0;i<N;i++,letter++){
        host_datos[i].id[0]=(char)letter;
        host_datos[i].num=(double)(rand()/(RAND_MAX/(99.0-0.1)));

        printf("%s:%f\n", host_datos[i].id, host_datos[i].num);
    }
}
