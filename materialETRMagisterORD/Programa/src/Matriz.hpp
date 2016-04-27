#ifndef _Matriz_
#define _Matriz_
#include "Vector.hpp"

/// Clase para el trabajar con matrices de tipo double
class Matriz
{
   protected:
      ///Puntero de 2 dimensiones
      double **M;
      /// Numero de columnas
      unsigned long long  Col;
      /// Numero de filas
      unsigned long long   Fil;
      /// Nombre de la matriz
      char *Nmb;

   
   public:
	   
      ///Constructor de la clase
      Matriz(){}
		  
      ///Constructor sobrecargado de la clase
      Matriz(unsigned long long   fil,unsigned long long   col)
      {Solicita_Memoria(fil,col);}
	  
      ///Destructor de la clase
      ~Matriz()
	  {Libera_Memoria();}
		  
	  /// Muestra la matriz 
	  void Visualiza();
	  
      /// Asigna nombre a la matriz
      void AsignaNombre(const char *nmb);
   
	  /// Retorna el puntero donde se almacena la matriz 
	  double **Retorna_Puntero_Datos()
	  {return M;}
	  
      /// Retorna el nombre de la matriz 
      char *Nombre(void)
      { return Nmb;}
	  
      /// Asignar numero de filas
      void SetFilas(unsigned long long   fil)
      { Fil=fil; }

      /// Asignar numero de columnas
      void SetColumnas(unsigned long long   col)
      { Col=col; }
      /// Retorna el numero de filas
      unsigned long long   Filas(void)
      { return Fil; }

      /// Retorna el numero de columnas
      unsigned long long   Columnas(void)
      { return Col; }
      
	  ///Asigna memoria
      void Solicita_Memoria(unsigned long long   fil,unsigned long long   col);
	  
	  ///Libera memoria
	  void Libera_Memoria();
	  
	  
      /// Retorna 1 si es matriz cuadrada en caso contrario regresa 0
      int Matriz_Cuadrada(void)
      { return (Col == Fil ? 1: 0); }
      
      /// Revisa si tienen la misma dimension regresando 1 en caso afirmativo y cero en caso contrario
      int Misma_Dimension(Matriz *a);
      
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
      void Asigna(unsigned long long   fila, unsigned long long   columna, const double valor)
	  {
		M[fila][columna]=valor; 
	  }

      /// Retorna valor en fila, columna
      double Retorna(unsigned long long   fila, unsigned long long   columna)
	  {
		  return M[fila][columna];
	  }
      
      /// Tamaño del vector (aproximado) en Kb
      double Tamano(void);

      /// Inicializa la matriz con el valor val
      void Inicializa(const double valor);

      /// Visualiza la matriz
      void Visualiza(const int tp);

      /// Copia el contenido de la matriz a la matriz a 
      void Copia(Matriz *a);

      /// Suma las matrices A y B
      void Suma(Matriz *a, Matriz *b);

      /// Suma a la matriz el contenido de la matriz A 
      void Suma(Matriz *a);

      /// Resta a la matriz A la matriz B
      void Resta(Matriz *a, Matriz *b);

      /// Resta a la matriz el contenido de la matriz A 
      void Resta(Matriz *a);

      /// Multiplica la matriz por el escalar ESC
      void Multiplica(double escalar);

      /// Multiplica la matriz A por la matriz B
      void Multiplica(Matriz *a, Matriz *b);

      /// Multiplica la matriz A por el vector B
      void Multiplica(Matriz *a, Vector *b);

      /// Multiplica la matriz A por el vector B dejando el Resultado en R
      void Multiplica(Vector *b, Vector *r);
	  
	  /// Verifica si es estrictamente dominante la matriz
	  int Dominante();

};
#endif
