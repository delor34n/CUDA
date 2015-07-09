#include <stdio.h>

#define SIZE 12

#define nThreads 2
#define nBlocks 6

__global__ void splitBladeKernel(int * dev_a){
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    printf("\n[DEVICE; BLOCK:%d; THREAD:%d] dev_a[%d] = %d",blockIdx.x, threadIdx.x, idx, dev_a[idx]);
    __syncthreads();
}

void fill_array(int *host_a);
void show_array(int *host_a);

int main (void){
    printf("[HOST] InitBlade\n");

    int * host_a = (int *) malloc(sizeof(int)*SIZE);
    fill_array(host_a);
    show_array(host_a);
    printf("\n");

    int * dev_a;
    cudaMalloc((void**)&dev_a, sizeof(int)*SIZE);
    cudaMemcpy(dev_a, host_a, sizeof(int)*SIZE, cudaMemcpyHostToDevice);

    splitBladeKernel<<<nBlocks,nThreads>>>(dev_a);

    cudaDeviceSynchronize();
    cudaFree(dev_a);

    printf("\n");
    return 0;
}

void fill_array(int *host_a){
    int i;
    for(i=0; i<SIZE; i++)
        host_a[i] = i + 1;
}

void show_array(int *host_a){
    int i;
    for(i=0; i<SIZE; i++)
        printf("\nhost_a[%d] = %d",i, host_a[i]);
}