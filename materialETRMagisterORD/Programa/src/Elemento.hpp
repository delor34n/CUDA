#ifndef _Elemento_
#define _Elemento_
#include "Nodo_2D.hpp"
/// Clase que define un elemento finito, donde se indica el tipo de elemento y el indice del elemento
class Elemento
{
    protected:
    /// Indice del Elemento
	unsigned long long id_elemento;
	/// 1 - Lineal, 2 - Triangular, 3 - Cuadrilatero, 4 - Tetrahedro
	int tipo_elemento; 
    
	public:
	/// Constructor de la clase	 
	Elemento(){}
    /// Destructor de la clase
	~Elemento(){}
		
    /// Asigna Id del elemento
    void Set_Id_Elemento(unsigned long long id)
    {id_elemento=id;}	
	/// Asigna el tipo de elemento
	void Set_Id_Tipo(int tipo)
    {tipo_elemento=tipo;}
	///Retorna el id del elemento
	unsigned long long Get_Id_Elemento()
    {return id_elemento;}
    ///Retorna el tipo de elemento 
    int Get_Tipo_Elemento()
    {return tipo_elemento;}	
	
     	
};
#endif
