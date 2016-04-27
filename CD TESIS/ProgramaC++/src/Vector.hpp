#ifndef _Vector_
#define _Vector_
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <math.h>

/// Clase para el trabajar con vectores de tipo double
class Vector
{
   protected:

      /// Numero de columnas
      unsigned long long Columna;
   
      /// Nombre del vector
      char *Nmb;
   
      /// Puntero al Vector flotante 
      double *V;
   
    public:

   
      /// Constructor de la clase
      Vector(void)
      {
	  }
      /// Constructor sobrecargado de la clase
      Vector(unsigned long long col)
      {
         Solicita_Memoria(col,0.0);
         AsignaNombre(" ");
      }

      /// Constructor sobrecargado de la clase
      Vector(unsigned long long col, const double val)
      {
         Solicita_Memoria(col, val);
         AsignaNombre(" ");
      }

      /// Constructor sobrecargado de la clase
      Vector(unsigned long long col, char *nmb)
      {
         Solicita_Memoria(col,0.0);
         AsignaNombre(nmb);
      }

      /// Destructor de la clase
      ~Vector()
	  {
          Libera_Memoria();
      }
	  
	  /// Libera memoria
	  void Libera_Memoria()
	  {
		  	   if (V) delete []V;
	   V = NULL;
	   if (Nmb) delete []Nmb;
	   Nmb = NULL;
	  }
	  
	  ///Retorna puntero del vector
	  double *Retorna_Puntero_Datos()
	  {
		  return V;
	  }
	  
	  /// Solicita la memoria para el vector
      void Solicita_Memoria(unsigned long long col, const double val);

      /// Asigna un nombre al vector
      void AsignaNombre(const char *nmb);

      /// Retorna el nombre al vector
      char *Nombre(void)
      { return Nmb;}

      /// Retorna el numero de columnas
      unsigned long long Columnas(void)
      {
         return Columna;
      }

      /// Inicializa con valores val el vector
      void Inicializa(double val);

      /// Asigna valor de una elemento del vector en columna col
      void Asigna(unsigned long long col, double val);
    
      /// Retorna valor de la columna col del  vector
      double Retorna(unsigned long long col);
     
 
      /// Visualiza el vector
      void Visualiza();

      /// Copia el vector a otro vector a
      void Copia(Vector *a);

      /// Convierte el arreglo de tamaño tam a vector
      void Convierte(double *a, unsigned long long tam);

      /// Suma los vectores a y b
      void Suma(Vector *a, Vector *b);

      /// Suma al vector a 
      void Suma(Vector *a);

      /// Resta los vectores a menos b
      void Resta(Vector *a, Vector *b);

      /// Resta el vector a 
      void Resta(Vector *a);
      
      /// Multiplica el vector por el escalar esc
      void Multiplica(double esc);
           
      /// Calcula la norma infinito
      double Norma_inf(void);
      
      /// Calcula el producto punto
      double Producto_Punto();
      
      /// Calcula el producto punto
      double Producto_Punto(Vector *a);

      /// Calcula el vector Normal
      double Vector_Normal();
	  
	  double Error_Norma2(Vector *a,Vector *b);
	  
      /// Tamaño del vector (aproximado) en Kb
      double Tamano(void)
      {
         double t = (((unsigned long long) sizeof(V) * Columna) / 1024.0);
         return t;   
      }
      /// Sumatoria elementos del vector
	  double Sumatoria_Vector();
	  
      /// Visualiza información general de la matriz
      void Ver_Informacion(void)
      {
         printf("\nVector \"%s\" de dimensión: %Ld \n",Nmb,Columna);
      }
	  
	  
	  /// Cuenta el numero de elementos no ceros de un vector
		unsigned long long Conteo_Datos()
		{
		unsigned long long contadorDatos=0;
		
		
				for(unsigned long long j=0;j<Columna;j++)	
				{
					  if(V[j]!=0.0)
					  {contadorDatos++;}
				}
				
		return contadorDatos;		
		}




};
#endif
