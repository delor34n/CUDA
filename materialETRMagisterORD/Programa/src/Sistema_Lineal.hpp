#ifndef _Sistema_Lineal_
#define _Sistema_Lineal_
#include <math.h>
#include <deque>
#include <algorithm>
#include <vector>
#include "Archivo.hpp"
#include "Matriz_Dispersa.hpp" 
#include "Rutinas_Paralelas.hpp"
/// Clase para trabajar con sistema lineales
class Sistema_Lineal
{

  public:
  /// Numero de variables desconocidas
  unsigned long  long nDesconocido;  
  /// Numero de procesos
  int NP;
  /// Numero de iteraciones para uso en metodos iterativos
  unsigned long long Iteraciones;
  /// Indica si se desea alamcenar en archivo la convergencia del error
  int Traza;
  /// Usado en el calculo paralelo para indicar fila inicial global  
  Vector_Entero *INDICE_FILAS;
  /// Tiempo utilizado en resolver sistema
  double tSistema;
  /// Costructor de la clase
  Sistema_Lineal()
   {
	 //  Traza=0; // Por defecto no se exporta el error
   }
 
  Sistema_Lineal(int traza)
   {
      Traza=traza;
   }
   
   /// Destructor de la clase
    ~Sistema_Lineal()
   {
       
   }
    
   /// Asigna numero de iteraciones
   	void Set_Traza(int traza)
   {
	Traza=traza;   
   }   
   
    /// Asigna numero de iteraciones
   	int Get_Traza()
   {
	return Traza;   
   }   
   
   /// Asigna numero de iteraciones
   	unsigned long long  Get_Iteraciones()
   {
	return Iteraciones;   
   }   
      /// Asigna numero de procesos -  procesadores
   	void Set_Iteraciones(int Iter)
   {
	Iteraciones=Iter;   
   }   
   
    /// Asigna numero de procesos -  procesadores
   	void SetNP(int procesos)
   {
	NP=procesos;   
   }   
   
   /// Retorna numero de procesos - procesadores
    int GetNP()
   {
	return NP;   
   }  
   
   void Asigna_Indice_FIla(Vector_Entero *ENTRADA)
   {
	INDICE_FILAS = new Vector_Entero(ENTRADA->Columnas());
    INDICE_FILAS = ENTRADA;	   
   }
   
   /// Retorna el numero de incognitas desconocidas   
   unsigned long long Get_nDesconocido()
   {
	return nDesconocido;   
   }

  /// Asigna el numeros de incognitas del sistema   
   void Set_nDesconocido(unsigned long long  variables_X)
   {
	nDesconocido=variables_X;   
   }   
   
  // double Error_Norma2(Vector *AX,Vector *b);
   
   void LSRQ_Matriz_Dispersa(Matriz *A,Matriz_Dispersa *MDA)
   {
	   MDA=new Matriz_Dispersa(A);   
	}
   
   /// Algoritmo LSQR paralelo para matrices dispersas  
   void LSQR_Resolver_Paralelo(Vector *x, Vector *b,unsigned long long *iParticion,Vector_Entero *COLUMNAS,Vector_Entero *INDICES,Vector *DATOS,int Tipo_Parada,int traza);

   /// Algoritmo LSQR para matrices dispersas
   void LSQR_Resolver(Matriz_Dispersa *MDA, Vector *x, Vector *b,int Tipo_Parada,int traza);
   
   /// Algoritmo LSQR 
   void LSQR_Resolver_Normal(Matriz *A, Vector *x, Vector *b,int Tipo_Parada,int traza);	
	
   /// Redimensiona un sistema de ecuaciones representados matricialmente, indicando que filas-columnas se desean eliminar	
   void Redimensionar_Sistema(Matriz *a,Vector *b,Vector_Entero *frontera,unsigned long  long  DimFrontera);
   
   /// Redimensiona un sistema de ecuaciones representados matricialmente con Hashing , indicando que filas-columnas se desean eliminar	
   void Redimensionar_Sistema_Hashing(map<unsigned long long,double> A_FEM ,map<unsigned long long,unsigned long long> mFila, map<unsigned long long,unsigned long long > mColumna,Vector *b,Vector_Entero *frontera,unsigned long DimActual);	
	
   /// Resuelve Ax=b usando el metodo Jacobi
   void Resuelve_Jacobi(Matriz *A, Vector *x, Vector *b, double ep, int Tipo_Parada,int traza);
	
   ///  Resuelve Ax=b usando el metodo Gauss Seidel
   void Resuelve_Gauss_Seidel(Matriz *a, Vector *x, Vector *b, double ep,int Tipo_Parada,int traza);
	
   /// Resuelve el sistema Ax=b usando factorizacion LU
   void Resuelve_Factorizacion_LU(Matriz *a, Vector *x, Vector *b);

   /// Factoriza la matriz A en L y U dejandolas en la misma matriz
   void Factoriza_LU(Matriz *a);

   /// Resuelve el sistema Ax=LUx = b 
   void Solve_LU(Matriz *a, Vector *x, Vector *b);

   /// Resuelve el sistema LY=B, dados L y B
   void Solve_L(Matriz *a, Vector *y, Vector *b);
   
   void Solve_LLT(Matriz *a, Vector *x,Vector *b);

   /// Resuelve el sistema UX=Y, dados U  y Y
   void Solve_U(Matriz *a, Vector *x, Vector *y);
   
   /// Factoriza la matriz A en L y LT dejandolas en la misma matriz
   void Factoriza_LLT(Matriz *a);
	  


};
#endif
