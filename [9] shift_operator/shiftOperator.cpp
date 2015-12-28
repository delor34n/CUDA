#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <time.h>
#include <math.h>

#define POBLACION 10
#define N 5
#define PROB_MUTACION 0.001
#define GENERATIONS 10

typedef struct {
	float **B;
	float *aptitud;
} Poblacion;

Poblacion init_poblacion(Poblacion poblacion);
Poblacion init_selection(Poblacion selection);
void display_poblacion(Poblacion poblacion);

void crossover(Poblacion poblacion, int individual_a, int individual_b);
float bitwise_mutation_operator(float a);
void mutacion_poblacion(Poblacion *p);
Poblacion tournament_selection(Poblacion poblacion);

float fitness(int **A,int *Vector_b, int *Vector_c);
float ranged_rand(int min, int max);

int main(int argc, char **argv){
	srand(time(NULL));

	Poblacion poblacion;	
	poblacion = init_poblacion(poblacion);
	
	float **Matrix = (float **)malloc(N * sizeof(float*)); //matriz del problema
	float *Vector_b = (float *)malloc(N * sizeof(float)); // vector que resta
	int *Vector_d = (int *)malloc(N * sizeof(int)); //vector resultado

	for(int i=0;i<GENERATIONS;i++){
		display_poblacion(poblacion);
		mutacion_poblacion(&poblacion);
		printf("CROMOSOMAS MUTADOS\n");
		poblacion = tournament_selection(poblacion);
	}
}

Poblacion init_poblacion(Poblacion poblacion){
	poblacion.B = (float **) malloc (sizeof(Poblacion)*POBLACION);
	for(int i=0;i<POBLACION;i++){
		poblacion.B[i] = (float *) malloc (sizeof(float)*N);
		for(int j=0;j<N;j++)
			poblacion.B[i][j] = ranged_rand(-10, 10);
	}
	poblacion.aptitud = (float *) malloc (sizeof(Poblacion)*POBLACION);
	for(int i=0;i<POBLACION;i++)
		poblacion.aptitud[i] = ranged_rand(2, 10);
	return poblacion;
}

Poblacion init_selection(Poblacion selection){
	selection.B = (float **) malloc (sizeof(Poblacion)*POBLACION);
	for(int i=0;i<POBLACION;i++)
		selection.B[i] = (float *) malloc (sizeof(float)*N);
	
	selection.aptitud = (float *) malloc (sizeof(Poblacion)*POBLACION);
	return selection;
}

void display_poblacion(Poblacion poblacion){
	int i,j;
	printf("\n####### DISPLAY POBLACION #######\n");
	for(i=0;i<POBLACION;i++){
		printf("Valor del cromosoma |");
		for(j=0;j<N;j++){
			printf(" %f |",poblacion.B[i][j]);
		}
		printf("\nAptitud :%f \n", poblacion.aptitud[i]);
	}
	printf("################################\n");
}

void mutacion_poblacion(Poblacion *p){
	printf("\n####### MUTACION POBLACION #######\n");
	for(int i=0;i<POBLACION;i++){
		for(int j=0;j<N;j++){
			if ((double)rand()/(RAND_MAX+1.0) < PROB_MUTACION)
				p->B[i][j] = bitwise_mutation_operator(p->B[i][j]);
			//printf("p.B[%d][%d] , %f\n", i, j, p.B[i][j]);
		}
	}
	//printf("################################\n");
}

/*  Debemos obtener una forma de cambiar un bit no tan drasticamente, ya que si no lo hacemos bien,
	los numeros pueden llegar a cambiar demasiado.
	//x = a/(rand()%a);
    Obtenido de esta pagina: http://www.cprogramming.com/tutorial/bitwise_operators.html
	n_use = in_use ^ 1<<car_num;
*/
float bitwise_mutation_operator(float a){
	unsigned char *c = reinterpret_cast<unsigned char *>(&a);
	int x = ranged_rand(0,7);
	int quarter = ranged_rand(0,3);

	/*
	printf("\na: %f, quarter: %d; c length: %lu; c[%d]: ", a, quarter, sizeof(c), x);
	std::cout << std::bitset<8>(c[x])[x] << std::endl << std::endl;
	for(size_t i=0; i<sizeof a; ++i){
		std::cout << std::bitset<8>(c[i]) << std::endl;
	}
	*/

    c[quarter] = c[quarter] ^ (1<<x);

    /*
    for(size_t i=0; i<sizeof a; ++i){
		std::cout << std::bitset<8>(c[i]) << std::endl;
	}
	std::cout << std::endl;
	*/

	//printf("a despues: %f \n", *a2);
    return (float) *(reinterpret_cast<float *>(c));
}

float fitness(int **A,int *Vector_b, int *Vector_c){
	int F0,F1;
	int F2[N];
	int i,j;
	int prod,E,rmse;
	for(i=0;i<N;i++){
		prod = 0;
		for(j=0;j<N;j++){//hasta N en el caso de una matriz cuadrada.
			prod = prod + A[i][j]*Vector_b[j];
		}
	E = E + abs(prod-Vector_c[i]);
	}
	rmse = sqrt(E/N);
	F0 = rmse;

	//if (N == 289 || N == 1089){ //Condiciones originales de fitness implementada en el paper de Arturo.

	int sum_b,sum_c;
	for (i=0;i<N;i++){
		sum_b = sum_b + Vector_b[i];
		sum_c = sum_c + Vector_c[i];
	}
	F1 = (sum_c <= sum_b)?0:45;

	/*Revisar F2, ya que no entendemos su funcionamiento.
    %sum nodos <= sumFuente
  if sum(w) <= sum(b)
      F1 = 0;
  else
      F1 = 30; %se castiga los q no cumplen la cond
  end

   nodos = size(A,1);
   fronteras = sort(front); % primera fila estan los nodos frontera, 1x64
   cant_front = length(fronteras);
   f2 = rand(cant_front,1); % vector de 64X1 para 289 nodos

%condicion borde 0, 
   i=1;
   for nodo=1:nodos
      if find(fronteras == nodo) % el nodo esta en la frontera
          if w(nodo) == 0
             f2(i) = 0;
          else
              if w(nodo) <= 100
                  f2(i) = 5;
              else
                if w(nodo) <= 1000
                    f2(i) = 20;
                else
                    f2(i) = 50; %se castiga a los w cuyo valor en los nodos esta alejado de 0
                end
              end
          end
           i = i+1;
           if i > cant_front
              break;
           end
      end
   end  
	F2 = sum(f2);

	  if N == 289 || N == 1089
      fitness = F0 + F1 + F2;
  		else
      fitness = F0;
  	  end*/

  	if (N==289 || N== 1089){
  		return F0+F1;
  	}else
  		return F0;
  	
	//} 
}
/*void cruzamiento_poblacion(){
dataLength= 8*sizeof(Datatype);
realLength=ceil(solutionLength/dataLength);
word=crossoverPoint/dataLength;
wordPoint=crossoverPoint%dataLength;
restW P=dataLength-wordPoint;
snew1[word] = ((si[word]>>restWP)<<restWP)|((s2[word]<<wordPoint)>>wordPoint);
snew2[word] = ((s2[word]>>restWP)<<restWP)|((s1[word]<<wordPoint)>>wordPoint);

got it from = https://www.lri.fr/~hansen/proceedings/2011/GECCO/companion/p439.pdf
*/
void crossover(Poblacion poblacion, int individual_a, int individual_b){
	
}

Poblacion tournament_selection(Poblacion poblacion){
	Poblacion selection;
	selection = init_selection(selection);
	int cand_a, cand_b;

	for(int i=0;i<POBLACION-1;i++){
		cand_a = (int) (((double) POBLACION)*rand()/(RAND_MAX+1.0));
		cand_b = (int) (((double) POBLACION)*rand()/(RAND_MAX+1.0));

		if(poblacion.aptitud[cand_a]<poblacion.aptitud[cand_b]){
			selection.B[i] = poblacion.B[cand_a];
			selection.aptitud[i] = poblacion.aptitud[cand_a];
		}else{
			selection.B[i] = poblacion.B[cand_b];
			selection.aptitud[i] = poblacion.aptitud[cand_b];

		}
	}
	return selection;
}

float ranged_rand(int min, int max){
    return min + ((float)(max - min) * (rand() / (RAND_MAX + 1.0)));
}