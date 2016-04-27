#ifndef _Vector_Disperso_
#define _Vector_DIsperso_
#include "Vector_Entero.hpp"
#include "Vector.hpp"


/// Clase para el trabajar con vectores dispersos de tipo double
class Vector_Disperso
{
   protected:

      /// Vector entero que contiene datos de columnas
      Vector_Entero *vdColumnas;
   
      /// Vector double que contiene los datos del vector disperso
      Vector *vdDato;
   
     /// Numero de elementos del vector no ceros
     unsigned long nzDatos;
   
     unsigned long dimOriginal;
   
    public:

   
      /// Constructor de la clase
      Vector_Disperso(void)
      {
	  }
      /// Constructor sobrecargado de la clase
      Vector_Disperso(unsigned long columnas)
      {
         vdColumnas = new Vector_Entero(columnas);
         vdDato = new Vector(columnas);	  
	  }
	  
       /// Destructor de la clase
      ~Vector_Disperso()
	  {
		  vdColumnas->~Vector_Entero();
		  vdDato->~Vector();
      }
	  
	  /// Almacena en formato vector - columnas VDC
      void Almacenar_Formato_VDC(Vector *ENTRADA);
	  
	  /// Retorna el numero de columnas del vector origen
      unsigned long Retorna_dimOriginal(void)
      {
         return dimOriginal;
      }

	  /// Cuenta el numero de elementos no ceros de un vector
	  unsigned long Conteo_Datos_VD(Vector *Entrada);
	  
	  /// Retorna el numero de elementos no ceros del vector disperso
	  unsigned long Retorna_nzDatos()
	  {return nzDatos;}
	 

};
#endif
