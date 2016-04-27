#ifndef _Nodo_1D_
#define _Nodo_1D_
#include "Nodo.hpp"
/// Clase para definir un nodo unidimensional
class Nodo_1D:public Nodo
{
	protected:
	/// Posicion nodal en el eje coordenado	
	double valorX;
    
    public:
    /// Contructor de la clase 	
    Nodo_1D(void){}
		
	/// Destructor de la clase
	~Nodo_1D(void){}
		
	/// Asigna un valor flotante al nodo
	void Set_X(double x)
    {
	valorX=x;	
	}
    
	/// Retorna un valor del nodo
	double Get_X()
    {
	return x;	
	}
	
	
};
#endif
