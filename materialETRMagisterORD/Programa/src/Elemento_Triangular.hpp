#ifndef _Elemento_Triangular_
#define _Elemento_Triangular_
#include "Elemento.hpp"
#include "Matriz.hpp"
#include "Matriz_Entera.hpp"
///Coeficiente a de funcion de forma en Matriz
#define ai 0 
///Coeficiente b de funcion de forma en Matriz
#define bi 1 
///Coeficiente c de funcion de forma en Matriz
#define ci 2 
///Constante PI
#define PI 3.14159265 
///Propuesta standar - derivacion (qij) - Solo se necesitan los nodo que estan en el borde
#define BordeML 	 		  1 
///Propuesta Hibrida de Tesis Tarvainen 2006, DA-ETR
#define BordeHibrido 		  2 
///Propuesta Clasica Arridge 1993, Modelo DA-ETR - Se debe saber el elemento y puede ser usada para ambas Bij o Bi (Clasica)
#define BordeNormalGradiente  3 

///Clase que define un elemento finito triangular lineal de 3 nodos
class Elemento_Triangular:public Elemento
{
    protected:
	
	/// Nodos locales del elemento, se utiliza como un vector de objetos Nodo_2D[]
	Nodo_2D *Nodos_Elemento;
	
	/// Nodos almacenados como coordenadas locales (Solo se utiliza si se quieren utilizar formulacion Isoparametrica o bien en coordenadas locales)
	Nodo_2D *Coordenadas_locales;
	
	///Puntos internos en un elemento (REVISAR POR QUE LO DEFINI)
	Nodo_2D *Puntos_internos;
	
	/// Numero de nodos del elemento 
    int numero_nodos_elemento;
	
	///Dominio al cual pertenece el elemento
	int dominio;
	
	///Indica si el elemento contiene a una fuente
	int eFuente;
	
	/// Densidad aproximada del elemento
    double eDensidad;   
	
	///Area del elemento
	double area;
	
	///Aporte que hace el elemento a la fuente
	double f;
	
	///Variable fisica 1 del elemento asociada al comportamiento fisico-geometrico 
	double kVariable;
	
	///Variable fisica 2 del elemento asociado a la fisico-masa
	double mVariable;
	
	///Variable secundaria de dispersion del elemento
	double sVariable;
	
	///Vector de Cargas del elemento
	Vector *Carga;
	
	///Vector Gradiente de un nodo del Elemento
	Vector *GradienteV;
	
	///Centro geometrico del elemento
	Vector *centroide;
	
	///Matriz Gradientes del elemento 
	Matriz *GradienteM;
	
	///Coeficientes ai,bi,ci de la funcion de forma del elemento
	Matriz *Coeficientes_Permutacion;
	
	///Matriz Rigidez Elemento - Stiffness
	Matriz *MRE; 
	
	///Matriz de Masa Elemento 
	Matriz *MMC; 
	
	///Matriz de pertenencia global (numeracion nodal global) del Elemento 
	Matriz_Entera *Pertenencia;
	
	public:
    
	///Constructor de la CLase
	Elemento_Triangular(){}


    ///Destructor de la Clase
	~Elemento_Triangular(){}
	
    ///Asigna el numero de nodos del elemento		
	void Set_Numero_Nodos_Elemento(int NNE)
    {numero_nodos_elemento=NNE;}
	
	///Reserva memoria y define un arreglo de objetos Nodo_2D[] que pertenecen al elemento
	void Set_Reserva_Memoria_Nodos_Triangulo(int NNE)
    {Nodos_Elemento=new Nodo_2D[NNE];}
	
	///Reserva memoria y define un arreglo de objetos Nodo_2D[] que pertenecen al elemento en caso de utilizar coordenadas locales
	void Set_Reserva_Memoria_Coordenadas_locales(int NNE)
    {Coordenadas_locales=new Nodo_2D[NNE];}
	
	///Reserva memoria para la matriz de Rigidez del elemento 
	void Set_Reserva_Memoria_Matriz_Rigidez_Elemento()
    {MRE=new Matriz(3,3);} 
	
	///Reserva memoria para la matriz de masa del elemento 
	void Set_Reserva_Memoria_Matriz_Masa_Elemento()
    {MMC=new Matriz(3,3);}    
	
	///Reserva memoria para la matriz de pertenencia del elemento 
	void Set_Reserva_Memoria_Matriz_Pertenencia_Elemento()
    {Pertenencia=new Matriz_Entera(3,3);}    
	
	//Asigna el identificador global de un nodo al elemento a la posicion local "Posicion_Nodal" 
	void Set_Nodo_Elemento(int Posicion_Nodal,int nodo_global)
    {Nodos_Elemento[Posicion_Nodal].Set_Id_Nodo(nodo_global);}
	
   /// Asigna constante Fuente, solo se utiliza para problemas con distribucion de energia constante en todo el dominio 
	void Set_Constante_Fuente(double fi)
    { 
		f=Get_Area()/3.0;
	}
	
	/// Reserva memoria para el centroide y asigna el valor del centro geometrico del elemento
	void Set_Centroide(double x, double y)
    {  centroide=new Vector(2);
		centroide->Asigna(0,x);
		centroide->Asigna(1,y);
	}
	
	/// Reserva memoria y asigna el vector gradiente del elemento
	void Set_Vector_Gradiente(double Bi, double Ci)
    {  GradienteV=new Vector(2);
		GradienteV->Asigna(0,Bi);
		GradienteV->Asigna(1,Ci);
	}
	
	/// Reserva memoria para vector de carga del elemento
	void Set_Reserva_Memoria_Vector_Carga()
    {Carga=new Vector(3);}
	
	/// Asigna los valores al vector de carga del elemento
	void Set_Vector_Carga(double fi, double fj,double fk)
    {  
		Carga->Asigna(0,fi);
		Carga->Asigna(1,fj);
		Carga->Asigna(2,fk);
	}
	
	/// Reserva memoria para ma matriz de permutacion del elemento
	void Set_Reserva_Memoria_Permutacion()
    {Coeficientes_Permutacion=new Matriz(3,3);}
	
	/// Asigna un valor a la matriz de permutacion del elemento
	void Set_Permutacion(int fil,int col,double valor)
    {   
		Coeficientes_Permutacion->Asigna(fil,col,valor);
	}
	
	/// Asigna un valor a la matriz de rigidez en la posicion fil, col
	void Set_Matriz_Rigidez_Elemento(int fil,int col,double valor)
    {   
		MRE->Asigna(fil,col,valor);
	}
	
	/// Asigna un valor a la matriz de masa en la posicion fil, col
 	void Set_Matriz_Masa_Elemento(int fil,int col,double valor)
    {   
		MMC->Asigna(fil,col,valor);
	}
	
	/// Asigna un valor a la matriz de pertenencia en la posicion fil, col
	void Set_Matriz_Pertenencia_Elemento(int fil,int col,int valor)
    {   
		Pertenencia->Asigna(fil,col,valor);
	}
	
	
	/// Reserva memoria y asigna los valores de la matriz gradiente 
	void Set_Matriz_Gradiente(double Bi,double Ci)
    {  
		GradienteM=new Matriz(3,2);
		GradienteM->Asigna(0,0,Bi);
		GradienteM->Asigna(0,1,0.0);
		GradienteM->Asigna(1,0,0.0);
		GradienteM->Asigna(1,1,Ci);
		GradienteM->Asigna(2,0,Ci);
		GradienteM->Asigna(2,1,Bi);
	}
	
	/// Asigna el area del elemento
	void Set_Area(double area_elemento)
    {area=area_elemento;}
	
	/// Asigna el dominio al cual pertenece el elemento
	void Set_Dominio_Elemento(int Dom)
    {dominio=Dom;}
	
	/// Asigna un 1 si es un elemento que contiene a una fuente
	void Set_Elemento_Fuente(int asignacion)
    {eFuente=asignacion;}
	
	/// Asigna constante kVariable del elemento, generamente asociada al comportamiento fisico-geometrico del elemento
	void Set_kVariable_Elemento(double Valor)
    {kVariable=Valor;}
	
	/// Asigna constante sVariable del elemento asociada al comportamiento fisico del elemento
	void Set_sVariable_Elemento(double Valor)
    {sVariable=Valor;}
	
	/// Asigna la densisad del elemento 
	void Set_Densidad_Elemento(double Valor)
    {eDensidad=Valor;}
	
	/// Asigna constante mVariable del elemento, generamente asociada al comportamiento fisico-masa del elemento
	void Set_mVariable_Elemento(double Valor)
    {mVariable=Valor;}
	
	/// Asigna el valor de una coordenada local del elemento 
    void Set_Coordenada_Local_Elemento(int idNodo,double xCord,double yCord)
	{
	Coordenadas_locales[idNodo].Set_X(xCord);
	Coordenadas_locales[idNodo].Set_Y(yCord);	
	}
 	
	/// Retorna la numeracion nodal global del nodo local "Posicion_Nodal" 
	unsigned long long Get_Valor_Nodo(int Posicion_Nodal)
	{
	return Nodos_Elemento[Posicion_Nodal].Get_Id_Nodo();
	}
	
	/// Retorna el dominio del elemento
	int Get_Dominio()
    {return dominio;}
	
	///retorna el area del elemento
	double Get_Area()
    {return area;}
	
	///Retorna el valor de la fuente del elemento
	double Get_f()
    {return f;}
	
	/// Retorna la kVariable del elemento
	double Get_kVariable()
    {return kVariable;}
	
	/// Retorna la mVariable del elemento
	double Get_mVariable()
    {return mVariable;}
	
	/// Retorna la sVariable del elemento
	double Get_sVariable()
    {return sVariable;}
	
	/// Retorna el objeto Nodo_2D Coordenadas_locales[id] que tiene informacion sobre las coordenadas locales del elemento
	Nodo_2D Get_Coordenada_Local(int id)
	{
	return Coordenadas_locales[id];}
	
	/// Retorna el objeto Nodo_2D Nodos_Elemento[id], que tiene informacion sobre los nodos del elemento
	Nodo_2D Get_Nodos_Elemento(int id)
	{
	return Nodos_Elemento[id];}
	
    /// Retorna la matriz de permutacion del elemento    
	Matriz *Get_Coeficientes_Permutacion()
	{return Coeficientes_Permutacion;}
	
	/// Retorna la matriz de Rigidez del elemento    
	Matriz *Get_Matriz_Rigidez_Elemento()
	{return MRE;}
	
	/// Retorna la matriz de masa del elemento    
	Matriz *Get_Matriz_Masa_Elemento()
	{return MMC;}
	
	/// Retorna la matriz de pertenencia del elemento    
	Matriz_Entera *Get_Matriz_Pertenecia_Elemento()
	{return Pertenencia;}
	
	/// Retorna el vector que contiene el centro geometrico del elemento   
	Vector *Get_Centroide()
    {return centroide;}
	
	/// Retorna el vector que contiene el gradiente del elemento   
	Vector *Get_GradienteV()
    {return GradienteV;}
	
	/// Retorna el vector de cargas del elemento   
	Vector *Get_CargasV()
    {return Carga;}
	
	/// Retorna la matriz gradiente del elemento
	Matriz *Get_GradienteM()
    {return GradienteM;}
	
	/// Retorna el numero de nodos del elemento
	int Get_Numero_Nodos_Elemento()
    {return numero_nodos_elemento;}
	     	
};
#endif
