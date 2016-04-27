#ifndef _Geometria_
#define _Geometria_
#include "Elemento.hpp"
#include "Archivo.hpp"
/// Clase base para generar una geometria
class Geometria
{

  protected:

      
      /// Numero de nodos
      unsigned long long Numero_Nodos;
   
      /// Numero de elementos
      unsigned long long Numero_Elementos;
   
      /// Dimension a trabajar
      int DIM;
   
      /// Numero de nodos por elemento
      int Numero_Nodos_Elemento;

      /// Numero de fronteras
      unsigned long Numero_Fronteras;
  
      /// Numero de fuentes
      int Numero_Fuentes;
  
      

  public:

     /// Constructor de la clase
   Geometria(void)
   {}

   /// Destructor de la clase
   ~Geometria()
   {
       
   }
   
      /// Retorna el numero de nodos
      void Asigna_Numero_de_Nodos(int num)
      {
         Numero_Nodos=num;
      }
	  
	  /// Asigna el numero de elementos de la geometria
      void Asigna_Numero_de_Elementos(int num)
      {
         Numero_Elementos=num;
      }
	  
	  /// Asigna el numero de fronteras de la geometria
	  void Asigna_Numero_de_Fronteras(int num)
      {
         Numero_Fronteras=num;
      }
	  
	  /// Asigna el numero de nodos por elemento de la geometria (En caso de utilizar otros tipos de elementos)
	  void Asigna_Numero_de_Nodos_en_Elementos(int num)
      {
         Numero_Nodos_Elemento=num;
      }
	  
	  /// Asigna el numero de fuentes de energia, para problemas de difusion
	  void Asigna_Numero_de_Fuentes(int num)
      {
         Numero_Fuentes=num;
      }
	  
	  /// Retorna el numero de fuentes
	   int Retorna_Numero_de_Fuentes(void)
      {
         return Numero_Fuentes;
      }
	  
	  /// Retorna el nuemro de fronteras
	   unsigned long Retorna_Numero_Fronteras(void)
      {
         return Numero_Fronteras;
      }
	  
	  /// Retorna el numero de nodos
      unsigned long Retorna_Numero_Nodos(void)
      {
         return Numero_Nodos;
      }
   
      /// Retorna el numero de elementos
      unsigned long Retorna_Numero_Elementos(void)
      {
         return Numero_Elementos;
      }
   
      /// Retorna el número de nodos por elemento
      int Retorna_nodos_elemento(void)
      {
         return Numero_Nodos_Elemento;
      }


};
#endif
