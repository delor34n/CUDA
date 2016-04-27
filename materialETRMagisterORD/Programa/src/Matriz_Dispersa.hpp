#ifndef _Matriz_Dispersa_
#define _Matriz_Dispersa_
#include "Matriz.hpp"
#include "Vector_Entero.hpp"
#include <map>
 using namespace std; 

/// Clase para el trabajar con matrices dispersas de tipo double
class Matriz_Dispersa
{
      public:
	     
      /// Vector entero que contiene datos de columnas
      Vector_Entero *vColumna; 
      
      /// Vector entero que contiene datos de filas
      Vector_Entero *vFila;
   
      /// Vector Entero que contiene los indices de cada nueva fila
      Vector_Entero *vIndiceFila;
         
      /// Vector flotante que contiene los datos de la matriz dispersa
      Vector *vDato;
   
     /// Numero de elementos matriz no ceros
     unsigned long long nDatos;
   
     /// Dimension vector Columnas con datos
     unsigned long long nColumnasDatos;
   
     /// DImension Vector indices   
     unsigned long long nIndices;
   
   
      ///Constructor de la clase
      Matriz_Dispersa(){}
		  
      ///Destructor de la clase
      ~Matriz_Dispersa()
	  {}

	  ///Constructor sobrecargado de la clase, entrada directa de vectores Fila, Columna Indices y Datos y quedan almacenado en los mismos punteros de referencia en memoria
      Matriz_Dispersa(Vector_Entero *FILAS,Vector_Entero *COLUMNAS,Vector_Entero *INDICES,Vector *DATOS)
      {
		vFila=FILAS;
        vColumna=COLUMNAS;
        vIndiceFila=INDICES;		  
		vDato=DATOS;  
	  }
	  
	  
      ///Constructor sobrecargado de la clase con entrada Matriz y almacenamiento en formato CSR
      Matriz_Dispersa(Matriz *ENTRADA)
      {  Almacenar_Formato_CSR(ENTRADA);}
	  	  
	  ///Constructor sobrecargado de la clase con entrada Matriz
      Matriz_Dispersa(map<unsigned long long,double> ENTRADA,unsigned long long nFilas,map<unsigned long long,unsigned long long> mFILA,map<unsigned long long,unsigned long long> mCOLUMNA)
      {  Almacenar_Formato_CSR_Hash(ENTRADA,nFilas, mFILA,mCOLUMNA);}
	  	  
	  ///Producto matriz dispersa por un vector
	  void Producto_MA_Vector(Vector *x,Vector *Sslida);
	  
	  /// Almacena en formato CSR de una entrada de tipo Matriz
	  void Almacenar_Formato_CSR(Matriz *Entrada);
	  
      /// Almacena en formato CSR de una entrada de tipo map (Hash)
	  void Almacenar_Formato_CSR_Hash(map<unsigned long long ,double> ENTRADA,unsigned long long nFilas,map<unsigned long long,unsigned long long> mFILA,map<unsigned long long,unsigned long long> mCOLUMNA);
	  
	  ///Retorna el numero de datos de la Matriz		  
	  unsigned long long Conteo_Datos(Matriz *Entrada);  
      
	  /// Producto puntos vectores DIsperso
      double Producto_Punto_Vectores_Disperso(double *x, double *y, unsigned long long length);

      /// Producto matriz dispersa por vector 
      Vector *Producto_Matriz_Vector(Vector *x);

  	  /// Retorna Vector_Entero de indices
	  Vector_Entero *Retorna_Vector_Indices()
	  {return vIndiceFila;}	  
         
	  /// Retorna el vector con indices de columnas
      Vector_Entero *Retorna_Vector_vColumna()
	  {return vColumna;}

	  /// Retorna el vector con indices de filas
      Vector_Entero *Retorna_Vector_vFila()
	  {return vFila;}	 	
	
	  /// Retorna el vector que contiene los datos de la matriz dispersa
	  Vector *Retorna_Vector_vDato()
	  {return vDato;}	 	

};
#endif
