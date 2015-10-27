#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define POBLACION 10
#define N 5
#define PROB_MUTACION 0.01

typedef struct {
	int **B;
	int *aptitud;
} Poblacion;

Poblacion init_poblacion(Poblacion poblacion);
void display_poblacion(Poblacion poblacion);
int bitwise_mutation_operator(int a,int x);
void mutacion_poblacion(Poblacion p);
void bitwise_crossover_operator(int a, int b);
void bitwise_crossover_operator_2(int a, int b);

int main(int argc, char **argv){
	srand(time(NULL));

	Poblacion poblacion;
	poblacion = init_poblacion(poblacion);
	display_poblacion(poblacion);
	int i;
	/*for(i=0;i<2;i++){
		mutacion_poblacion(poblacion);
		printf("CROMOSOMAS CAMBIADOOOS\n");
		display_poblacion(poblacion);
	}*/

	bitwise_crossover_operator_2(2, 4);

	/*for(i=100;i<110;i+=2){
		bitwise_crossover_operator(i, i+1);
	}*/
}

Poblacion init_poblacion(Poblacion poblacion){
	poblacion.B = (int **) malloc (sizeof(Poblacion)*POBLACION);
	int i,j;
	for(i=0;i<POBLACION;i++){
		poblacion.B[i] = (int *) malloc (sizeof(int)*N);
		for(j=0;j<N;j++)
			poblacion.B[i][j] = (10.0*rand()/(RAND_MAX+1.0));
	}
	poblacion.aptitud = (int *) malloc (sizeof(Poblacion)*POBLACION);
	return poblacion;
}

void display_poblacion(Poblacion poblacion){
	int i,j;
	for(i=0;i<POBLACION;i++){
		//printf("Genotipo número %d \n",i);
		printf("Valor del cromosoma ");
		for(j=0;j<N;j++){
			printf(" %d ",poblacion.B[i][j]);
		}
		printf(" \n");
	}
}
void mutacion_poblacion(Poblacion p){
	int i,j,x;
	for(i=0;i<POBLACION;i++) {
		for(j=0;j<N;j++)
			if ((double) rand()/(RAND_MAX+1.0) < PROB_MUTACION){
				x = rand()/(RAND_MAX+1.0);
				printf("Valor de la wea %d\n", x);
				p.B[i][j] = bitwise_mutation_operator(p.B[i][j],x);
			}
		}
}

int bitwise_mutation_operator(int a, int x){

	/* Debemos obtener una forma de cambiar un bit no tan drasticamente, ya que si no lo hacemos bien,
	los numeros pueden llegar a cambiar demasiado.*/
	//x = a/(rand()%a);
	/* Obtenido de esta pagina: http://www.cprogramming.com/tutorial/bitwise_operators.html
	n_use = in_use ^ 1<<car_num; */
	return a ^ 1<<x;
}

/*
int iChrom1 = 101010 
int iChrom2 = 110110

//pick cross over point
int iChosenPoint = 3;
int iMask = 1; // 000001
iMask = iMask<<iChosenPoint; //  001000
iMask = iMask - 1; //mask is now 000111

//get tail end
int iChrom1_end = iChrom1 & iMask; //000010
int iChrom2_end = iChrom2 & iMask; //000110

//clear old tails
int iMask2 = ~iMask; // 111000
iChrom1 = iChrom1 & iMask2; //101000
iChrom2 = iChrom2 & iMask2; //110000

//swap tails
iChrom1 = iChrom1 | iChrom2_end; //101110
iChrom2 = iChrom2 | iChrom1_end; //110010
*/
/*int bitwise_crossover_operator(int a, int b){

	printf("=============");
	printf("\na: %d; b: %d", a, b);

	int mask = 1;
	int chosenPoint = 4;

	mask = mask << chosenPoint;
	mask = mask - 1;

	int A_1 = a & mask;
	int B_1 = b & mask;

	printf("\nmask: %d; A_1: %d; B_1: %d\n", mask, A_1, B_1);

	int mask_2 = ~mask;
	a = a & mask_2;
	b = b & mask_2;

	printf("mask_2: %d; a: %d; b: %d\n", mask_2, a, b);

	a = a | B_1;
	b = b | A_1;

	printf("a: %d; b: %d", a, b);
	printf("\n=============");
	printf("\n\n");
}
*/

/*mask1 = ((0xffff >> 16*p) << 16*p)
mask2 = 0xffff ^ mask1
output1 = (input1 & mask1) ^ (input2 & mask2)
output2 = (input1 & mask2) ^ (input2 & mask1)*/

void bitwise_crossover_operator(int a, int b){
	printf("=============\n");
	printf("a: %d; b: %d\n", a, b);
	int chosenPoint = 8;
	int mask1 = ((0xffff >> 32*chosenPoint) << 32*chosenPoint);
	int mask2 = 0xffff ^ mask1;
	int output1 = (a & mask1) ^ (b & mask2);
	int output2 = (a & mask2) ^ (b & mask1);
	printf("output1: %d; output2: %d\n", output1, output2);
	printf("\n=============");
	printf("\n\n");

}
/*dataLength= 8*sizeof(Datatype);
realLength=ceil(solutionLength/dataLength);
word=crossoverPoint/dataLength;
wordPoint=crossoverPoint%dataLength;
restW P=dataLength-wordPoint;
snew1[word] = ((si[word]>>restWP)<<restWP)|((s2[word]<<wordPoint)>>wordPoint);
snew2[word] = ((s2[word]>>restWP)<<restWP)|((s1[word]<<wordPoint)>>wordPoint);
*/
void bitwise_crossover_operator_2(int a, int b){

	printf("a = %d, b = %d \n",a,b);	
	int dataLength = 32;
	int realLength = 6;
	int crossoverPoint = 1;
	int Mask = 255;
	int word = crossoverPoint/realLength;
	int wordPoint = crossoverPoint%realLength;
	int restWP = dataLength-realLength;
	int snew1 = ((a>>crossoverPoint)<<crossoverPoint)|(((b<<wordPoint)&Mask)>>wordPoint);
	printf("crossoverPoint %d , wordPoint = %d \n", crossoverPoint,wordPoint);
	int snew2 = ((b>>crossoverPoint)<<crossoverPoint)|(((a<<wordPoint)&Mask)>>wordPoint);
	printf("snew1 = %d, snew2 =%d  \n",snew1,snew2);


}

/*
void fitness(){
	//Coming soon...
}
Poblacion seleccion_poblacion(){

}
void cruzamiento_poblacion(){

}
void aptitud{

}*/


