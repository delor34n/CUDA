#include <iostream>
#include <random>
#include <pthread.h>

#define NUM_THREADS 10
#define VECTOR_SIZE 1000000

#define LOWER_RAND_BOUND 1
#define UPPER_RAND_BOUND 1000

unsigned int * thread_ids;
double *vector_a;
double *vector_b;
double *vector_c;
pthread_mutex_t sum_mutex;

bool init_vectors();

double * allocate_vector();
void destroy_vector(double *vector);

void fill_vector(double *vector);
void fill_vector();
void display_vector(double *vector);
void display_vectors();
double get_random_double();

void *vector_add(void *threadarg){
	int local_n = VECTOR_SIZE/NUM_THREADS;	// number of elements to handle
	int local_id = * (int *) threadarg; // get thread's ID
	int start = local_id * local_n; // starting index
	int end;

	if(local_id != (NUM_THREADS-1))
		end = start + local_n;	// Ending index
 	else
        end = VECTOR_SIZE;		// Ending index

    
    for(;start<end;start++){    // Find min in my range
    	/* Lock the mutex and update the global sum, then exit */
		pthread_mutex_lock (&sum_mutex);
		vector_c[start] = vector_a[start] + vector_b[start];
    	pthread_mutex_unlock (&sum_mutex);
    }

	pthread_exit(NULL);
}

int main(){
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	void *status;

	if(!init_vectors()){
		std::cout << "SOMETHING WENT WRONG ALLOCATING MEMORY" << std::endl;
		return 0;
	}

	display_vectors();

	// Initialize and set thread joinable
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	// Initialize sum_mutex
	pthread_mutex_init(&sum_mutex, NULL);

	for(int i=0, rc; i < NUM_THREADS; i++ ){
  		std::cout << "main() : creating thread, " << i << std::endl;

  		thread_ids[i] = i;
  		
      	rc = pthread_create(&threads[i], NULL, vector_add, (void *)&thread_ids[i]);
      	if (rc){
	 		std::cout << "Error:unable to create thread, " << rc << std::endl;
	     	exit(-1);
      	}
   }

	// free attribute and wait for the other threads
	pthread_attr_destroy(&attr);
	for(int i=0, rc; i < NUM_THREADS; i++ ){
  		rc = pthread_join(threads[i], &status);
		if(rc){
			std::cout << "Error:unable to join," << rc << std::endl;
			exit(-1);
	  	}
		std::cout << "Main: completed thread id :" << i ;
		std::cout << "  exiting with status :" << status << std::endl;
   	}

   	display_vector(vector_c);

    /*
	 * Destroying allocated vectors memory
	 */
	destroy_vector(vector_a);
	destroy_vector(vector_b);
	destroy_vector(vector_c);

	std::cout << "Main: program exiting." << std::endl;
	pthread_mutex_destroy(&sum_mutex);
	pthread_exit(NULL);
}

bool init_vectors(){
	/*
	 * Allocates vectors memory
	 */
	vector_a = allocate_vector();
	if(vector_a == nullptr){
		std::cout << "ERROR ALLOCATING VECTOR A" << std::endl;
		return false;
  	}

	vector_b = allocate_vector();
	if(vector_b == nullptr){
		std::cout << "ERROR ALLOCATING VECTOR B" << std::endl;
		return false;
  	}

	vector_c = allocate_vector();
	if(vector_c == nullptr){
		std::cout << "ERROR ALLOCATING VECTOR C" << std::endl;
		return false;
  	}

  	thread_ids = new (std::nothrow) unsigned int[NUM_THREADS];
  	if(thread_ids == nullptr){
		std::cout << "ERROR ALLOCATING IDs VECTOR" << std::endl;
		return false;
  	}

	/*
	 * Fills vectors
	 */
	fill_vector(vector_a);
	fill_vector(vector_b);

	return true;
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

void fill_vector(){
	for (unsigned int i = 0; i< VECTOR_SIZE; i++)
        thread_ids[i] = 0;
}

void display_vector(double *vector){
    std::cout << "[ ";
	for (unsigned int i = 0; i< VECTOR_SIZE; i++)
		std::cout << vector[i] << " ";
	std::cout << "]" << std::endl;
}

void display_vectors(){
	std::cout << "VECTOR A" << std::endl;
	display_vector(vector_a);

	std::cout << "VECTOR B" << std::endl;
	display_vector(vector_b);
}

double get_random_double(){
	std::random_device rd;
    std::default_random_engine generator(rd());
	std::uniform_real_distribution<double> unif(LOWER_RAND_BOUND, UPPER_RAND_BOUND);
	return unif(rd);
}