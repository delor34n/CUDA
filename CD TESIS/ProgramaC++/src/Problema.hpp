#ifndef _Problema_
#define _Problema_

///Clase que permite definir problemas a solucionar con elementos finitos - Esta es una clase de enlace a futuros problemas a resolver con el metodo FEM
class Problema 
{
     protected:
	
	 /// Tipo de problema, 1-Eliptico, 2-Parabolico, 3 - Hiperbolico etc.....	
	 int tipo_problema;
	 
	 /// Tipo derivacion de la ecuacion diferencial a resolver,  Borde Vectorial 1 - matricial 2
	 int tipo_derivacion; 
	
	 /// Tipo constribucion de vector de cargas,  Carga Nula 0, Carga constante 1, Carga tipo fuente (Funcion de distribucion de Dirac) 2
     int tipo_contribucion_carga;	
	 
     public:
     /// Constructor de la clase   
	 Problema(void)
	 {}
	   
     /// Destructor de la clase
     ~Problema()
    {}
   
    
   
};
#endif
