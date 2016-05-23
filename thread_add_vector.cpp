#include <iostream>
#include <random>
#include <pthread.h>

#define NUM_THREADS 5
#define VECTOR_SIZE 10

#define LOWER_RAND_BOUND 0
#define UPPER_RAND_BOUND 10000

struct thread_vector_data{
   int thread_id;
   double *vector;
};

double * allocate_vector();
void destroy_vector(double *vector);

void fill_vector(double *vector);
void display_vector(double *vector);
double get_random_double();

void *vector_add(void *threadarg){
	struct thread_vector_data *local_data;

	local_data = (struct thread_vector_data *) threadarg;

	std::cout << "Thread ID : " << local_data->thread_id ;
	std::cout << "Vector : [ ";

	for(int i; i < VECTOR_SIZE; i++)
		std::cout << local_data->vector[i] << " ";
	std::cout << "]" << std::endl;

	pthread_exit(NULL);
}

int main(){
	pthread_t threads[NUM_THREADS];
	struct thread_vector_data td[NUM_THREADS];
	pthread_attr_t attr;
	void *status;

	// Initialize and set thread joinable
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for(int i=0, rc; i < NUM_THREADS; i++ ){
  		std::cout << "main() : creating thread, " << i << std::endl;
  		td[i].thread_id = i;
  		td[i].vector = allocate_vector();
  		if(td[i].vector == nullptr){
  			std::cout << "ERROR ALLOCATING VECTOR " << i << std::endl;
  			return 0;
      	}

      	fill_vector(td[i].vector);
      	std::cout << "###############" << std::endl;
      	display_vector(td[i].vector);

      	rc = pthread_create(&threads[i], NULL, vector_add, (void *)&td[i]);
      	if (rc){
     		std::cout << "Error:unable to create thread, " << rc << std::endl;
         	exit(-1);
      	}
   }

   // free attribute and wait for the other threads
   pthread_attr_destroy(&attr);
   for(int i=0, rc; i < NUM_THREADS; i++ ){
      rc = pthread_join(threads[i], &status);
      if (rc){
         std::cout << "Error:unable to join," << rc << std::endl;
         exit(-1);
      }
      std::cout << "Main: completed thread id :" << i ;
      std::cout << "  exiting with status :" << status << std::endl;
   }

   for(int i=0; i < NUM_THREADS; i++ ){
  		std::cout << "main() : destroying vector, " << i << std::endl;
  		destroy_vector(td[i].vector);
   }

   std::cout << "Main: program exiting." << std::endl;
   pthread_exit(NULL);
}

double * allocate_vector(){
	return new (std::nothrow) double[VECTOR_SIZE];
}

void destroy_vector(double *vector){
	delete [] vector;
}

void fill_vector(double *vector){
	for (unsigned int i = 0; i< VECTOR_SIZE; i++)
        vector[i] = get_random_double();
}

void display_vector(double *vector){
    std::cout << "Vector : [ ";
	for (unsigned int i = 0; i< VECTOR_SIZE; i++)
		std::cout << vector[i] << " ";
	std::cout << "]" << std::endl;
}

double get_random_double(){
	std::uniform_real_distribution<double> unif(LOWER_RAND_BOUND, UPPER_RAND_BOUND);
	std::default_random_engine re;
	return unif(re);
}