#ifndef _FEM2D_
#define _FEM2D_
#include "FEM.hpp"

/// Clase donde se define un problema FEM de dos dimensiones
class FEM2D:public FEM
{
   public:
     
   //Matriz que contiene datos sobre las pertenencias nodales de los elementos del dominio 
   Matriz_Entera *PerteneciaEL;
   //Matriz de rigidez de los elementos del dominio 
   Matriz *RigidezEl;
   //Matriz de Masa de los elementos del dominio 
   Matriz *MasaEl;
   //Elementos triangulares del dominio
   Elemento_Triangular *EL;
   //Elementos triangulares que pertenecen a la frontera del dominio
   Elemento_Frontera *ELF;
   //Vector de cargas de los elementos
   Vector *QEl;
   //Vector de pertenencias de los elementos frontera
   Vector_Entero *PerteneciaELF;
   //Matriz que contiene informacion del borde de los elementos
   Matriz *BordeEl;
   
   FEM2D(void)
   {
   }

   /// Destructor de la clase
   ~FEM2D()
   {
       Libera_Memoria_Carga();
   }
   
   /// Libera carga de memoria	
   void Libera_Memoria_Carga();   
   
   /// Reserva memoria para el sistema de ecuaciones, segun geometria utilizada
    void Inicializar_FEM2D(int Tipo_resolucion_AX);
   
   /// Realiza un llamada al metodo de contruccion de la vector de carga global B segun la geometria utilizada 
   void Llena_Matriz_B_FEM();
  
   /// Realiza un llamada al metodo de contruccion de la matriz global A segun la geometria utilizada 
	void Llena_Matriz_A_FEM(int borde,int tipo_solucion_matricial);
	
	/// Suma un aporte nodal al vector de cargas global B del sistema de ecuaciones
	void Set_Suma_Nodal_B_FEM(unsigned long long i,double aNodal);

   /// Suma un aporte nodal a la matriz global A del sistema de ecuaciones
    void Set_Suma_Nodal_A_FEM(unsigned long long i,unsigned long long j,double aNodal);

   /// Reserva memoria para la matriz global A del sistema de ecuaciones
   	void Set_Reserva_Memoria_A_FEM(unsigned long long nNodo);
 
	/// Reserva memoria para la matriz global B del sistema de ecuaciones	
	void Set_Reserva_Memoria_B_FEM(unsigned long long nNodos);
 
	/// Construye el vector de cargas global B del sistema de ecuaciones calculada con elementos finitos triangulares
	void Llena_Matriz_B_FEM_2D_Triangulos();

   /// Construye la matriz global A del sistema de ecuaciones a partir de las matrices de rigidez y de masa, calculada con elementos finitos triangulares
	void Llena_Matriz_A_FEM_2D_Triangulos(int tipo_AX);
  
   /// Aporta a la matriz global A del sistema de ecuaciones los calculos del borde global 
	void Aporte_Matriz_A_FEM_Borde_2D_Triangulos(int tipo_AX);
   
  
   

};
#endif
