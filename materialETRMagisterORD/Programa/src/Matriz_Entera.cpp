#include "Matriz_Entera.hpp"


/// Muestra la matriz
void Matriz_Entera::Visualiza()
{
   
   unsigned long long i, j; 
   unsigned long long  NoNulos=0;	
   for (i = 0; i < Fil; i++) 
   {
	   printf("\n");
       for (j = 0; j < Col; j++) 
	   {
		   printf(" %Ld ",M[i][j]);
		   if(M[i][j]!=0.0)
		   	NoNulos++;   
       }
   }
    printf("\n Matriz de %Ldx%Ld con %Ld elementos no nulos\n",Fil,Col,NoNulos);
}


/// Copia el contenido de la matriz a la matriz a 
void Matriz_Entera::AsignaNombre(const char *nmb)
{
   Nmb = new char[strlen(nmb)+2];
   if (!Nmb)
   {
      printf("\nError no hay memoria para el nombre de la matriz\n");
      exit(1);
   }
   strcpy(Nmb, nmb);   
}

/// Solicita memoria para la matriz
void Matriz_Entera::Solicita_Memoria(unsigned long long  fil, unsigned long long  col)
{
   unsigned long long  i;
   M = new unsigned long long  *[fil];
   if (M == NULL) FaltaMemoria();

   for (i = 0; i < fil; i++) {      
      M[i] = new unsigned long long [col]; 
      if (M[i] == NULL) FaltaMemoria();
   }

   Col = col;
   Fil = fil;

}


// Libera la memoria solicitada para la matriz
void Matriz_Entera::Libera_Memoria()
{
   unsigned long long  i;
   if (M)
   {

      for (i = 0; i < Fil; i++)
      {
         delete []M[i];
         M[i] = NULL;
      }
  }
   
  delete []Nmb;
}
      /// Copia el contenido de la matriz a la matriz a 
void Matriz_Entera::Copia(Matriz_Entera *a)
{
   unsigned long long  i, j;
   for (i = 0; i < Fil; i++)
   {
      for (j = 0; j < Col; j++) a->Asigna(i,j,Retorna(i,j));
   }
}
