#ifndef _Nodo_
#define _Nodo_
#include "Vector_Entero.hpp"
///Clase utilizada para definir un nodo de un elemento finito
class Nodo
{
    protected:
	///Identificador del nodo
    unsigned long long id_nodo;
	
	///Dimension del nodo
    int dimension_nodo;
    
    public:
	
	/// Constructor de la clase Nodo	 
	Nodo(void){}
	
	/// Destructor de la clase Nodo
	~Nodo(void){}
	
	/// Asigna dimension del nodo	
    void Set_Dimension_Nodo(int dimension)
    {dimension_nodo=dimension;}
    
	/// Asigna el identificador del nodo
    void Set_Id_Nodo(int id)
    {id_nodo=id;}	
	
	/// Retorna la dimension del nodo
	int Get_Dimension_Nodo()
    {return dimension_nodo;}
    
	/// Retorna el identificador del nodo
    unsigned long long Get_Id_Nodo()
    {return id_nodo;}	

     	
};
#endif
