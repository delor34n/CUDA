#ifndef _Elemento_Frontera_
#define _Elemento_Frontera_
#include "Elemento_Triangular.hpp"
/// Clase que define los elementos que estan en la frontera del dominio, util para un futuro calculo de matrices elementales frontera  
class Elemento_Frontera:public Elemento_Triangular
{
    protected:
	/// Arreglo de objetos Nodo_2D[] que contendran informacion sobre la frontera	
	Nodo_2D *Nodos_Frontera;
	/// Indices globales de los nodos de la frontera
	Vector_Entero *Nodos_Borde;
	/// Matriz de borde Integral
	Matriz *MIB;
	/// Vector de borde Integral
	Vector *VIB;
    /// Dominio de la frontera	
	int dominio_frontera;
	/// Factor experimental de Fressnel (No se usa, pero se deja para una futura implementacion)
    double R; 
	/// Factor asociado a la difusion  (No se usa, pero se deja para una futura implementacion)
    double A_Factor;
    /// Factor asociado a la dispersion	 (No se usa, pero se deja para una futura implementacion)
    double Yn;
	/// Constante asociada al scattering
	double kScatering;
	/// Norma - Distancia, usada para el calculo integral entre dos puntos
	double Norma;
	/// Vector normal
	Vector *Normal;
	
	
	public:
	
	/// Constructor de la clase	 
	Elemento_Frontera()
	{
	Nodos_Borde=new Vector_Entero(2);	
	}
		
	/// Destructor de la clase (RECORDAR LIBERAR RAM)
	~Elemento_Frontera(){}
	
		
	///Retorna la matriz borde	
	 Matriz *Get_MIB()
    {return MIB;}
	
	///Retorna la vector borde	
	 Vector *Get_VIB()
    {return VIB;}
	
	///Retorna la matriz pertenencia borde	
	 Vector_Entero *Get_pertenencia_Borde()
    {return Nodos_Borde;}
	
	/// Asigna un valor al vector de pertenencia en la posicion col
	void Set_Vector_Pertenencia_Borde(int col,int valor)
    {   
		Nodos_Borde->Asigna(col,valor);
	}
	
	
	/// Retorna el indice global de un nodo frontera	
	int Get_Valor_Nodo_Frontera(int nNodo)
	{
	return Nodos_Frontera[nNodo].Get_Id_Nodo();
	}	
	
	/// Retorna el arreglo Nodo_2D[] que contiene informacion de los nodos de la frontera
	Nodo_2D *Get_Nodo_Frontera()
	{
	return Nodos_Frontera;
	}	
	
	/// Reserva memoria para las matrices asociadas al borde
	void Set_Reserva_Memoria_Matriz_Borde(int fil,int col)
    {MIB=new Matriz(fil,col);}   
	
	/// Reserva memoria para las matrices asociadas al borde
	void Set_Reserva_Memoria_Vector_Borde(int col)
    {VIB=new Vector(col);} 
	
	/// Reserva  memoria a una matriz 2x2 que contendra informacion del elemento frontera (Variante de alternativa directa)
    void Set_Reserva_Memoria_MIB()
    {
	  MIB=new Matriz(2,2);
	  
	}
	
	/// Asigna un valor a la matriz de borde en fila - columna
	void Set_Matriz_Borde(int fil,int col,double valor)
    {   
		MIB->Asigna(fil,col,valor);
	}
	
	/// Asigna un valor al vector borde en columna
	void Set_Vector_Borde(int col,double valor)
    {   
		VIB->Asigna(col,valor);
	}
	
	/// Reserva memoria para alamacenar informacion de los nodos frontera
	void Set_Reserva_Memoria_Nodos_Frontera(int NNE)
    {Nodos_Frontera=new Nodo_2D[NNE];}	
	
	/// Asigna el indice global a un nodo frontera
    void Set_Nodo_Frontera(int Posicion_Nodal,int nodo_global)
    {Nodos_Frontera[Posicion_Nodal].Set_Id_Nodo(nodo_global);}	
	
	/// Asigna el dominio al cual pertenece el nodo
	void Set_Dominio_Frontera(int Dom)
    {dominio_frontera=Dom;}
	
	/// Retorna el dominio al cual pertenece el nodo frontera
	int Get_Dominio_Frontera()
    {return dominio_frontera;}
	
	/// Asigna la norma al elemento frontera
	void Set_Norma(double nNorma)
    {Norma=nNorma;}
	
		
	/// Asigna la norma al elemento frontera
	double Get_Norma()
    {return Norma;}
	
	/// Reserva memoria y asigna el vector normal a un elemento frontera
	 void Set_Vector_Normal(Vector vNormal)
    {
	Normal=new Vector(2);
	Normal->Asigna(0,vNormal.Retorna(0));
	Normal->Asigna(1,vNormal.Retorna(1));
	}
	
	/// Calculo de la matriz asociada al borde
	double Calculo_MBorde_ML(double qi,double nNorma,int peso)
	{
	double ML;
	ML=qi*nNorma*(1.0 + (double)peso)/6.0;
    return ML;		
	}
	
	/// Calculo del Vector asociado al borde
	double Calculo_VBorde_ML(double qi,double nNorma)
	{
	double ML;
	ML=qi*nNorma/2.0;
    return ML;		
	}
    

	
};
#endif
