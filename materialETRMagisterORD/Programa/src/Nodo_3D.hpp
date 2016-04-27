#ifndef _Nodo_3D_
#define _Nodo_3D_
#include "Nodo.hpp"
/// Clase para definir un nodo tridimensional
class Nodo_3D:public Nodo
{
	protected:
	/// Posicion nodal en el eje x	
	double valorX;
    
	/// Posicion nodal en el eje y	
	double valorY;
	
	/// Posicion nodal en el eje z	
	double valorZ;
	
    public:
	/// Contructor de la clase 	
    Nodo_3D(void){}
		
	/// Destructor de la clase
	~Nodo_3D(void){}	
		
    /// Asigan un valor de posicion nodal en el eje x	
	void Set_X(double x)
    {
	valorX=x;	
	}
	
	/// Asigan un valor de posicion nodal en el eje y	
	void Set_Y(double y)
    {
	valorY=y;	
	}
    
	/// Asigan un valor de posicion nodal en el eje z	
	void Set_Z(double z)
    {
	valorZ=z;	
	}

	/// Retorna el valor de x
	double Get_X()
    {
	return x;	
	}
 
	/// Retorna el valor de y
	double Get_Y()
    {
	return y;	
	}

	///Retorna el valor de z
	double Get_Z()
    {
	return z;	
	}
};
#endif
