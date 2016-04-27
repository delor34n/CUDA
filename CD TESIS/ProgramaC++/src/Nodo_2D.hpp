#ifndef _Nodo_2D_
#define _Nodo_2D_
#include "Nodo.hpp"
/// Clase para definir un nodo bidimensional
class Nodo_2D:public Nodo
{
	protected:
	/// Posicion nodal en el eje x	
	double valorX;
    
	/// Posicion nodal en el eje y	
	double valorY;
	
	/// Valor del nodo
	double valorNodal;
	
	/// Gradiente nodal 
	double gradienteNodal;
	
	///Numero de elementos que contienen al nodo
	int NV;
	
	/// Indicador de nodo frontera donde es 1 si es nodo frontera
	int pBorde;
	
	/// Vector que contiene a los elementos que contienen al nodo 
    Vector_Entero *Vecindad_Nodo;
	
	/// Bandera usada para marcar un nodo ya calculado, util para el calculo de fuentes que comparten un mismo nodo
	int bandera_nodo;
	
	public:
	/// Contructor de la clase 	
    Nodo_2D(void)
	{
	bandera_nodo=0;	
	}
	
	/// Destructor de la clase
	~Nodo_2D(void){}
	
	///Asigna el valor de la bandera
    void Set_bandera(int valor)
    {
		bandera_nodo=valor;	
	}		
	
	///Retorna el valor de la bandera
     int Get_bandera()
    {
	return bandera_nodo;	
	}	
	
	/// Asigna un valor flotante en x
	void Set_X(double x)
    {
	valorX=x;	
	}
	
	/// Asigna un valor flotante en y
	void Set_Y(double y)
    {
	valorY=y;	
	}
	
    /// Asigna como nodo borde	
    void Set_pBorde(int valor)
    {
	pBorde=valor;	
	}
	
	/// Asigna el valor flotante del nodo
	void Set_vNodal(double valor)
    {
	valorNodal=valor;	
	}
	
	/// Asigna el valor gradiente del nodo 
	void Set_gNodal(double valor)
    {
	gradienteNodal=valor;	
	}
	
	/// Suma al gradiente un desplazamiento
	void Set_gNodal_Suma(double valor)
    {
	gradienteNodal=gradienteNodal+valor;	
	}
	
	/// Asigna el numero de elementos que contienen al nodo y crea un vector de largo orden
	void Set_Orden_Vecindad(int orden)
    {
	NV=orden;	
	Vecindad_Nodo=new Vector_Entero(orden);
	}
	
	/// Asigna al vector Vecindad_Nodo en posicion id un indicador de elemento contenedor de nodo (REVISAR)
	void Set_Vecindad(int id,int valor)
    {
	Vecindad_Nodo->Asigna(id,valor);
	}
	
	/// Retorna los elementos vecinos
	Vector_Entero *Get_Vecindad()
	{
	return Vecindad_Nodo;	
	}
	
	/// Retorna el numero de elementos que contienen al Nodo
	int Get_Orden_Vecindad()
    {
	return NV;	
	}
	
	/// Retorna 1 si pertenece a la frontera del dominio el nodo
	int Get_pBorde()
    {
	return pBorde;	
	}
	
	/// Retorna el valor de x del nodo
	double Get_X()
    {
	return valorX;	
	}
	
	/// Retorna el valor de y del nodo
	double Get_Y()
    {
	return valorY;	
	}
	
	/// Retorna el valor del nodo
	double Get_Densidad()
    {
	return valorNodal;	
	}
	
	/// Retorna la gradiente nodal (REVISION)
	double Get_Gradiente()
    {
	return gradienteNodal;	
	}
	
	
};
#endif
