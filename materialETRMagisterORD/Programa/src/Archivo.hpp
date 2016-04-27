#ifndef _Archivo_
#define _Archivo_
#include "Matriz.hpp"
#define LEE_ARCHIVO       0
#define GRABA_ARCHIVO     1
#define GRABA_LINEA_ARCHIVO     3

/// Clase para manipular archivos
class Archivo   
{
  private:
  /// Indica el estado de uso del archivo pasivo-activo
  int              Estado;                      
  /// Puntero a un archivo
  FILE            *ARCHIVO_TRABAJO;
  /// Nombre del archivo 
  char            *ARCHIVO;
  /// Modo en el cual se esta utilizando el archivo Lectura - escritura
  int 			   MODO; 
  /// Definicion de permisos del archivo 
  char            *ACCESO;  
  
  public:

    ///Contructor de la Clase
	Archivo()
	{
		Estado = 0;
	}

	/// Destructor de la Clase
	~Archivo()
	{
	   Cerrar();
	   if (ARCHIVO) delete []ARCHIVO;
	   Estado = 0;
	}

   /// Escribe un Vector a un archivo (Recordar cambiar a Escribe_Vector)
   void Escribe_Vector(Vector *a,unsigned long long nn);

   /// Escribe una matriz cuadrada a un archivo
   void Escribe_MatrizCuadrada(Matriz *a,unsigned long long nodos);
	
  /// Escribe una matriz a un archivo
   void Escribe_Matriz(Matriz *a);
   
   ///Asigna un nombre al archivo
   void SetNombre(char *nombre)
   {ARCHIVO=nombre;}
   
   ///Define el tipo de acceso al archivo
   void SetModo(int modo)
   {MODO=modo;}
   
   ///Retrona Nombre del archivo
   char* GetNombre()
   {return ARCHIVO;}
   
   ///Retorna el archivo
   FILE* GetArchivo()
   {return ARCHIVO_TRABAJO;}
   
   ///Retorna el numero de lineas del archivo
   unsigned long long Numero_Lineas();
   
   ///Abre el archivo
   void Abrir(); 
 
   /// Cierra el archivo
   void Cerrar();
   
   /// Muestra el nombre del archivo
   void VerNombre();
   
};
#endif
