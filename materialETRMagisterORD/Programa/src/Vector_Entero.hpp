#ifndef _Vector_Entero_
#define _Vector_Entero_
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include "math.h" 
/// Clase para el trabajar con vectores enteros long long sin signo
class Vector_Entero
{
   protected:

      /// Numero de columnas
      unsigned long long Columna;
   
      /// Nombre del vector
      char *Nmb;
   
      /// Puntero al Vector entero 
      unsigned long long *V;
   
      /// Solicita la memoria para el vector
      void Solicita_Memoria(unsigned long long col, unsigned long long val);


   public:

   ///Constructor de la clase
       Vector_Entero()
      {
	  }
      /// Constructor sobrecargado de la clase
      Vector_Entero( unsigned long long col)
      {
         Solicita_Memoria(col,0);
         AsignaNombre(" ");
      }

      /// Constructor sobrecargado de la clase
      Vector_Entero(unsigned long long col, int val)
      {
         Solicita_Memoria(col, val);
         AsignaNombre(" ");
      }

      /// Constructor sobrecargado de la clase
      Vector_Entero(unsigned long long col, char *nmb)
      {
         Solicita_Memoria(col,0);
         AsignaNombre(nmb);
      }

      /// Destructor de la clase
      ~Vector_Entero()
	  {
	  Libera_Memoria();
      }
	  
	  /// Retorna el puntero donde se almacena el vector 
	  unsigned long long*Retorna_Puntero_Datos()
	  { return V; }
	  
	  void Libera_Memoria()
	  {
	   if (V) delete []V;
	   V = NULL;
	   if (Nmb) delete []Nmb;
	   Nmb = NULL;
	  }
	  
      /// Asigna nombre al vector
      void AsignaNombre(const char *nmb);

      /// Retorna el nombre al vector
      char *Nombre(void)
      {return Nmb;}

      /// Retorna el numero de columnas
      unsigned long long Columnas(void)
      {return Columna;}

	  /// Busca un valor en el vector y retorna 1 en caso verdadero (solo indica si existe, no cuenta)
	  unsigned long long Buscar_Valor(unsigned long long valor);
    
	  /// Busca un valor en el vector hasta un cierto indice y retorna 1 en caso verdadero (solo indica si existe, no cuenta)
	  unsigned long long Buscar_Valor_Limite(unsigned long long valor,unsigned long long limite);
	  
	  /// Inicializa con val el vector
      void Inicializa( unsigned long long val);

      /// Asigna un valor en posicion col
      void Asigna(unsigned long long col,unsigned long long val);
    
      /// Retorna valor de col
      unsigned long long Retorna( unsigned long long col);
          
      /// Visualiza el vector
      void Visualiza();

      /// Copia el vector a otro vector a
      void Copia(Vector_Entero *a);

      
      /// Tamaño del vector (aproximado) en Kb
      float Tamano(void)
      {
         float t = (((unsigned long long ) sizeof(V) * Columna) / 1024.0);
         return t;   
      }
      /// Sumatoria de datos del vector
	  unsigned long long Sumatoria_Vector();
	  
      /// Visualiza información general de la matriz
      void Ver_Informacion(void)
      {
         printf("\nVector \"%s\" de dimensión: %Ld \n",Nmb,Columna);
      }

};
#endif
