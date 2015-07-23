#include <stdio.h>
#include <curl/curl.h>

#define ATTACKS 1000
#define BLOCKSIZE 128

__host__ __device__ void curl_run(){
    CURL *curl;
    curl = curl_easy_init();
}

__global__
void curlKernel(){
    curl_run();
}

int main(){
    printf("[HOST] STARTING SCRIPT\n");

    int GRIDSIZE = (ATTACKS+BLOCKSIZE-1)/BLOCKSIZE;
    dim3 block(BLOCKSIZE, 1, 1);
    dim3 grid(GRIDSIZE, 1, 1);

    curlKernel<<<grid,block>>>();
    cudaDeviceSynchronize();
    cudaDeviceReset();

    printf("[HOST] SCRIPT EXECUTION FINISHED\n");
    return 0;
}
