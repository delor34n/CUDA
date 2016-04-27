#ifndef _Matriz_Entera_
#define _Matriz_Entera_
#include "Vector_Entero.hpp"

/// Clase para el trabajar con matrices Enteras sin signo 
class Matriz_Entera
{
   protected:
      /// Puntero bidimensional que almacenara los valores de una matriz
      unsigned long long **M;
      /// Numero de columnas
      unsigned long long  Col;
      /// Numero de filas
      unsigned long long  Fil;
      /// Nombre de la matriz
      char *Nmb;
   
   
   public:
	  /// Constructor de la clase
      Matriz_Entera(){}
	  
      /// Constructor sobrecargado de la matriz
      Matriz_Entera(unsigned long long  fil, unsigned long long  col)
      {Solicita_Memoria(fil, col);}
	  
	  /// Destructor de la clase
      ~Matriz_Entera(){}

      void Visualiza();

	  /// Asigna nombre a la matriz
      void AsignaNombre(const char *nmb);
   
      /// Retorna el nombre de la matriz 
      char *Nombre(void)
      { return Nmb;}
      
	  /// Asignar numero de filas
      void SetFilas(unsigned long long  fil)
      { Fil=fil; }

      /// Asignar numero de columnas
      void SetColumnas(unsigned long long  col)
      { Col=col; }
      
	  /// Retorna el numero de filas
      unsigned long long  Filas(void)
      { return Fil; }

      /// Retorna el numero de columnas
      unsigned long long  Columnas(void)
      { return Col; }

	  /// Reserva memoria para la matriz
      void Solicita_Memoria(unsigned long long  fil, unsigned long long  col);
	  
	  /// Libera memoria de la matriz
	  void Libera_Memoria();
	  
	  /// Retorna 1 si es matriz cuadrada en caso contrario regresa 0
      int Matriz_Cuadrada(void)
      { return (Col == Fil ? 1: 0); }
      
      /// Revisa si tienen la misma dimension regresando 1 en caso afirmativo y cero en caso contrario
      int Misma_Dimension(Matriz_Entera *a);

 	  /// Visualiza el error de falta de memoria para soportar la matriz
      void FaltaMemoria(void)
      {
         printf("\nError no hay memoria para la matriz\n");
         exit(1);
      }
      
      /// Visualiza información general de la matriz
      void Ver_Informacion(void)
      {
         printf("\"%s\" de dimensión: %Ld x %Ld\n",Nmb,Fil,Col);
      }
   
      /// Asigna valor en fila, columna
      void Asigna(unsigned long long  fila, unsigned long long  columna, unsigned long long  valor)
	  {
		M[fila][columna]=valor; 
	  }

      /// Retorna valor en fila, columna
      unsigned long long  Retorna(unsigned long long  fila, unsigned long long  columna)
	  {
		  return M[fila][columna];
	  }
      
      /// Copia el contenido de la matriz a la matriz a 
      void Copia(Matriz_Entera *a);



};
#endif
