#include <stdio.h>
#include <string.h>


#include "Matriz.hpp"

/// Muestra la matriz
void Matriz::Visualiza()
{
   
   unsigned long long i, j; 
   unsigned long long  NoNulos=0;	
   for (i = 0; i < Fil; i++) 
   {
	   printf("\n");
       for (j = 0; j < Col; j++) 
	   {
		   printf(" %f ",M[i][j]);
		   if(M[i][j]!=0.0)
		   	NoNulos++;   
       }
   }
    printf("\n Matriz de %Ldx%Ld con %Ld elementos no nulos\n",Fil,Col,NoNulos);
}

/// Asigna un nombre a la matriz como identificador
void Matriz::AsignaNombre(const char *nmb)
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
void Matriz::Solicita_Memoria(unsigned long long  fil, unsigned long long  col)
{
 
   M = new double *[fil];
   if (!M)
	   {
	  printf("\nError no hay memoria para la matriz\n");    
      exit(1);
	   }

   for (unsigned long long  i = 0; i < fil; i++) {      
      M[i] = new double[col]; 
	   for(unsigned long long  j=0;j<col;j++)
	    M[i][j]=0.0;
	   
      if (M[i]==NULL)
	  {
	  printf("\nError no hay memoria para columnas en la matriz\n");    
      exit(1);
		}
   }

   Col = col;
   Fil = fil;

}


/// Libera la memoria solicitada para la matriz
void Matriz::Libera_Memoria()
{
   unsigned long long  i;
   if (M)
   {

      for (i = 0; i < Fil; i++)
      {
		 delete M[i]; 
         M[i] = NULL;
      }
  }
  
  delete []Nmb;
  
}

/// Revisa si tienen la misma dimension regresando 1 en caso afirmativo y cero en caso contrario
int Matriz::Misma_Dimension(Matriz *a)
{
   if (a->Columnas() == Col && a->Filas() == Fil) return 1;
   return 0;
}

/// Copia matriz a una matriz nueva
void Matriz::Copia(Matriz *a)
{
   unsigned long long  i, j;
   for (i = 0; i < Fil; i++)
   {
      for (j = 0; j < Col; j++) a->Asigna(i,j,Retorna(i,j));
   }
}

/// Suma las matrices a y b
void Matriz::Suma(Matriz *a, Matriz *b)
{
   
   if (a->Columnas() != b->Columnas() || a->Filas() != b->Filas() )
   {
      printf("Error en dimensión de las matrices para su Suma\n");
      Ver_Informacion();
      a->Ver_Informacion();
      b->Ver_Informacion();
      exit(1);
   }


   unsigned long long  i, j;   
   for (i = 0; i < Fil; i++) 
   {
       for (j = 0; j < Col; j++) Asigna(i,j,a->Retorna(i,j) + b->Retorna(i,j));
   }
}


/// Suma a la matriz el contenido de la matriz a
void Matriz::Suma(Matriz *a)
{
  
   if (Columnas() != a->Columnas() || Filas() != a->Filas() )
   {
      printf("Error en dimensión de las matrices para su Suma\n");
      Ver_Informacion();
      a->Ver_Informacion();
      exit(1);
   }

 printf("Sumando Matriz\n");
   unsigned long long  i, j;   
   for (i = 0; i < Fil; i++) 
   {
       for (j = 0; j < Col; j++) 
	   {	   Asigna(i,j,Retorna(i,j) + a->Retorna(i,j));
       	 
	   if(i==1&&j==2)
	   printf("Suma = %f",Retorna(i,j));	   
   }    
   }
}


/// Resta a la matriz A la matriz B
void Matriz::Resta(Matriz *a, Matriz *b)
{
   
   if (a->Columnas() != b->Columnas() || a->Filas() != b->Filas() )
   {
      printf("Error en dimensión de las matrices para su Resta\n");
      Ver_Informacion();
      a->Ver_Informacion();
      b->Ver_Informacion();
      exit(1);
   }

   
   unsigned long long  i, j;
   for (i = 0; i < Fil; i++) 
   {
       for (j = 0; j < Col; j++) Asigna(i,j, a->Retorna(i,j) - b->Retorna(i,j));
   }
}


/// Resta a la matriz el contenido de la matriz A 
void Matriz::Resta(Matriz *a)
{

   if (Columnas() != a->Columnas() || Filas() != a->Filas() )
   {
      printf("Error en dimensión de las matrices para su Resta\n");
      Ver_Informacion();
      a->Ver_Informacion();
      exit(1);
   }

   
   unsigned long long  i, j;
   for (i = 0; i < Fil; i++) 
   {
       for (j = 0; j < Col; j++) Asigna(i,j,Retorna(i,j) - a->Retorna(i,j));
   }
}
/// Verifica si es estrictamente dominante la matriz
int Matriz::Dominante()
{
   unsigned long long  i, j;
   double suma;
   double Aii;	
   for (i = 0; i < Fil; i++) 
   {
	   suma=0.0;
       for (j = 0; j < Col; j++)
	   {
		   if(i!=j) 
		   {
			   suma = suma +  Retorna(i,j);
		   }
	   }
	   suma = (double)sqrt((double)pow(suma,2.0));
	   Aii=Retorna(i,i);
	   Aii=(double)sqrt((double)pow(Aii,2.0));
	  
	   if(Aii<=suma)
	   {	
       printf("******* LA MATRIZ NO ES DIAGONALMENTE DOMINANTE *******");		   
	   return 0;
	   }
   }
 return 1;  
}

/// Multiplica la matriz M por el escalar 
void Matriz::Multiplica(double escalar)
{
   unsigned long long  i, j;
   
   for (i = 0; i < Fil; i++) 
   {
       for (j = 0; j < Col; j++) Asigna(i,j,Retorna(i,j) * escalar);
   }
}


/// Multiplica la matriz A por la matriz B
void Matriz::Multiplica(Matriz *a, Matriz *b)
{
   
   if (a->Columnas() != b->Filas())
   {
      printf("Error en dimensión de las matrices para su multiplicarción\n");
      Ver_Informacion();
      a->Ver_Informacion();
      b->Ver_Informacion();
      exit(1);
   }

   
   unsigned long long  i, j, k;
   double v;
   for (i = 0; i < a->Filas(); i++) 
   {
      for (j = 0; j < b->Columnas(); j++) 
      {
         v = 0; 
         for (k = 0; k < a->Columnas(); k++) 
         {
            v = v + a->Retorna(i,k) * b->Retorna(k,j);
         }
         Asigna(i,j,v);
      }
   }
}


/// Multiplica la matriz A por el vector B
void Matriz::Multiplica(Matriz *a, Vector *b)
{  
   if (a->Columnas() != b->Columnas())
   {
      printf("Error en dimensión de las matrices para su multiplicarción\n");
      Ver_Informacion();
      a->Ver_Informacion();
      b->Ver_Informacion();
      exit(1);
   }

   
   unsigned long long  i, j, k;
   double v;
   for (i = 0; i < a->Filas(); i++) 
   {
      for (j = 0; j < 1; j++) 
      {
         v = 0; 
         for (k = 0; k < a->Columnas(); k++) 
         {
            v = v + a->Retorna(i,k) * b->Retorna(k);
         }
         Asigna(i,j,v);
      }
   }
}


/// Multiplica la matriz A por el vector B dejando el Resultado en el vector R
void Matriz::Multiplica(Vector *b, Vector *r)
{
   
   if (Columnas() != b->Columnas() || Columnas() != r->Columnas())
   {
      printf("Multiplica Mb=r Error en dimensión\n");
      Ver_Informacion();
      b->Ver_Informacion();
      r->Ver_Informacion();
      exit(1);
   }

   
   unsigned long long  i, k;
   double v;
   

   for (i = 0; i < Filas(); i++) 
   {
      v = 0; 
      for (k = 0; k < Columnas(); k++) 
      {
         v = v + Retorna(i,k) * b->Retorna(k);
      }
      r->Asigna(i,v);
   }
}
