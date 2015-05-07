#include <stdlib.h>
#include <stdio.h>

int main(void) {
    //Cantidad de elementos que tendra el array.
    int num_elements = 16;
    //Se calcula la cantidad de bytes que se utilizara en total (int=4bytes => 16x4=64bytes)
    int num_bytes = num_elements * sizeof(int);

    //Un puntero para la GPU
    int *device_array = 0;
    //Un puntero para la CPU
    int *host_array = 0;

    //Se realiza el allocate del host (CPU)
    host_array = (int*)malloc(num_bytes);

    //Se realiza el allocate del device (GPU)
    cudaMalloc((void**)&device_array, num_bytes);

    //Llenamos de cero el array que se encuentra en la GPU
    cudaMemset(device_array, 12, num_bytes);

    //Copiamos el contenido del array que se encuentra en la GPU en el array de la CPU
    cudaMemcpy(host_array, device_array, num_bytes, cudaMemcpyDeviceToHost);

    //Imprimimos cada elemento del array del host (CPU)
    for(int i = 0; i < num_elements; ++i)
        printf("%d ", host_array[i]);

    //Liberamos la memoria solicitada para el host
    free(host_array);

    //Liberamos la memoria solicitada para el device
    cudaFree(device_array);

    return 0;
}
