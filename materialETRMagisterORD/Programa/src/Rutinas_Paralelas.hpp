#ifndef _Rutinas_Paralelas_
#define _Rutinas_Paralelas_
#include <mpi.h>
//#include <math.h>
//#include "Archivo.hpp"
#include "Vector_Entero.hpp"
#include "Vector.hpp"
#include "Matriz_Dispersa.hpp" 
#include "Vector_Disperso.hpp" 
#include "Geometria2D.hpp"

/// Clase para trabajar con rutinas paralelas en almacenamiento y calculos
class Rutinas_Paralelas
{

  public:
	 
  /// Numero de valores desconocidos de la grilla
  unsigned long long  nDesconocido;
   
  /// Numero de procesadores a utilizar
   int nProcesadores;
  
  /*
  Se utilizo estas forma de almacenar datos de nodos y elementos para el envio y recepcion de informacion entre los procesadores
  ya que es mucho mas comodo el utilizar punteros a matrices o vectores que a arreglos de objetos y el costo de almacenaje es del orden de los milisegundos
  por lo cual no consume un gasto significativo de procesamiento de datos.
  */
  
  /// Matriz que contiene todo los datos de los elementos (id elemento, area, coeficientes de permutacion, aproximaciones nodales y lista de nodos)
  Matriz *Datos_Elementos; // [i,nodo1,nodo2,nodo3,area,x1,x2,x3,matriz de permutacion]

  Vector *Datos_Elementos_Vector;
  
  /// Vector que contiene datos de las coordenadas X para el calculo paralelo de la grilla FEM
  Vector *puntosX; 
 
  /// Vector que contiene datos de las ordenadas Y para el calculo paralelo de la grilla FEM
  Vector *puntosY;
  
  /// Vector que contiene las densidades calculadas en el proceso paralelo del calculo de la grilla
  Vector *Densidad_Grilla;
  
  /// Vector que contiene datos de los nodos en X del dominio FEM
  Vector *nodoX;
  
  /// Vector que contiene datos de los nodos en Y del dominio FEM
  Vector *nodoY;
  
  /// Vector que contiene los centroides de la grilla
  Vector *CentroidesGrilla;
  
 /// Matriz que contiene informacion sobre la particion paralalela de un matriz
 Matriz_Entera *Particion_FC; 

 /// Matriz que continene informacion sobre indices de procesadores
 Matriz_Entera *Particion_Procesadores;
 
 /// Tiempo utilizado en comunicacion aproxiamcion FEM
 double tFEM;
 
 /// Tiempo utilizado en comunicacion calculo Grilla
 double tGrilla;
 
   /// Costructor de la clase
   Rutinas_Paralelas()
   {
   }
   
   /// Costructor sobre cargado de la clase
   Rutinas_Paralelas(int Procesadores)
   {
       nProcesadores=Procesadores;
	   int size,rank;
       MPI_Comm_rank(MPI_COMM_WORLD,&rank);	
	   MPI_Comm_size(MPI_COMM_WORLD, &size);	  
   }
   
   /// Destructor de la clase
    ~Rutinas_Paralelas()
   {}
   
   /// Retorna la matriz que contiene todo los datos de los elementos (id elemento, area, coeficientes de permutacion, aproximaciones nodales y lista de nodos)
   Matriz *Retorna_Datos_Elementos(void)
   {return Datos_Elementos;}
	
	/// Retorna la matriz que almacena las filas y columnas de las particiones 
	Matriz_Entera *Retorna_FC(void)
   {return Particion_FC;}
	
    /// Retorna la matriz con los indices de rank de la matriz de particion fila columna
	Matriz_Entera *Retorna_MP(void)
   {return Particion_Procesadores;}
	
    /// Retorna en un vector los centroides de las particiones de la grilla en paralelo
	Vector *Retorna_Centroides(void)
   {return CentroidesGrilla;}
	
   /// Retorna en un vector los valores X de los puntos de la grilla
	Vector *Retorna_PuntosX(void)
   {return puntosX;}
	
   /// Retorna en un vector los valores Y de los puntos de la grilla
    Vector *Retorna_PuntosY(void)
   {return puntosY;}
	
   /// Retorna en un vector los valores de X de los nodos de la geometria original evaluada en a FEM
	Vector *Retorna_NodosX(void)
   {return nodoX;}
	
   /// Retorna en un vector los valores de Y de los nodos de la geometria original evaluada en la FEM
	Vector *Retorna_NodosY(void)
   {return nodoY;}
	
   /// Retorna el vector de los datos que continenen toda la informacion del calculo FEM
	Vector *Retorna_Datos_Elementos_Vector(void)
   {return Datos_Elementos_Vector;}
	
	/// Almacena la matriz de particiones a partir de una matriz de entrada 
	void Almacena_Matriz_FC(Matriz_Entera *entrada)
	{
		Particion_FC = new Matriz_Entera(entrada->Filas(),entrada->Columnas());
		entrada->Copia(Particion_FC);
	}
	
    /// Almacena la matriz de indices de procesadores a partir de un matriz de entrada	
	void Almacena_Matriz_MP(Matriz_Entera *entrada)
	{
		Particion_Procesadores = new Matriz_Entera(entrada->Filas(),entrada->Columnas());
		entrada->Copia(Particion_Procesadores);
	}
	
	/// Reserva memoria para los centroides de la grilla particionada
	void Reserva_Memoria_Centroides(unsigned long long dim)
	{CentroidesGrilla = new Vector(dim);}
	
	/// Reserva memoria para los puntos de la grilla
	void Reserva_Memoria_Puntos(unsigned long long  nNodos)
	{
	   puntosX = new Vector(nNodos);
       puntosY = new Vector(nNodos);
	}
	
	/// Reserva memoria para la informacion de los en de x e y en los nodos de la FEM
	void Reserva_Memoria_Nodos(unsigned long long  nNodos)
	{		
	   nodoX = new Vector(nNodos);
       nodoY = new Vector(nNodos);
	}
	
	/// Reserva memoria para los datos de todo el calculo realizado por la aproximacion original de la FEM 
	void Reserva_Memoria_Datos_Elementos_Vector(unsigned long long  nElementos)
	{ Datos_Elementos_Vector = new Vector(nElementos*17);}
	
   /// Almacena en punteros los nodos de la geometria FEM para distribucion de informacion entre procesadores
   void Almacenar_Nodos_Geometria(Nodo_2D *GeometriaFEM,unsigned long long  nNodos);
	
   /// Almacena en punteros los nodos de la grilla para distribucion de informacion entre procesadores
   void Almacenar_Nodos_Grilla(Nodo_2D *GrillaFEM,unsigned long long  nNodos);
	
   /// Almacena en una fila especifica de la matriz Datos_Elementos (id elemento, area, coeficientes de permutacion, aproximaciones nodales y lista de nodos)
   void Almacenar_Datos_Elemento(unsigned long long  id_fila,unsigned long long  id_elemento,unsigned long long  nodo1,unsigned long long  nodo2,unsigned long long  nodo3,double area,double fi1,double fi2,double fi3, Matriz *Coeficientes_PermutacionEL);
   
   /// Almacena el numero de procesos	
   	void SetProcesadores(int Procesadores)
   {nProcesadores=Procesadores;}   
   
   /// Retorna el numero de procesos
    int GetProcesadores()
   {return nProcesadores;}  
   
	/// Multiplica una matriz dispersa por un vector, guardando en vector salida el resultado
	void Producto_Matriz_Dispersa_Vector(Vector *x,Vector *Salida,Vector_Entero *INDICES,Vector_Entero *COLUMNAS, Vector *DATOS);


    /// Multiplica una matriz traspuesta dispersa por un vector, guardando en vector salida el resultado
	void Producto_MatrizT_Dispersa_Vector(Vector *x,Vector *Salida,Vector_Entero *INDICES,Vector_Entero *COLUMNAS, Vector *DATOS,unsigned long long dim,unsigned long long inicio);


	/// Multiplica una matriz dispersa por un vector, guardando en una seccion el vector salida el resultado
	void Producto_Matriz_Dispersa_Vector_Seccion(Vector *x,Vector *Salida,Vector_Entero *INDICES,Vector_Entero *COLUMNAS, Vector *DATOS,unsigned long long dim, unsigned long long inicio);


   	/// Multiplica una matriz dispersa por un vector, guardando en una seccion el vector salida el resultado
	void Producto_Matriz_Dispersa_Vector_Seccion_LSQR(Vector *x,Vector *Salida,Vector_Entero *INDICES,Vector_Entero *COLUMNAS, Vector *DATOS,unsigned long long dim, unsigned long long inicio,double normaIn,double normaOut,Vector *u);
   
    /// Particiona un vector segun numero de procesadores, almacenando el largo de cada sub vector
	void Obtener_Vector_Particion(int size,unsigned long long orden,Vector_Entero *vParticion);
	
	/// Envia un mensaje que contiene un vector entero
	void Envio_Vector_Entero(int procesador_destino,int TAG,unsigned long long  posicion_indice,unsigned long long  largo,unsigned long long  *vector);
	
	/// Envia un mensaje que contiene un vector flotante	
	void Envio_Vector_Double(int procesador_destino,int TAG,unsigned long long  posicion_indice,unsigned long long  largo,double *vector);
	
	/// Envia un mensaje que contiene un dato flotante   
	void Envio_Dato_Double(int procesador_destino,int TAG,double Dato);
   
 	/// Envia un mensaje que contiene un dato entero     
   	void Envio_Dato_Entero(int procesador_destino,int TAG,unsigned long long  Dato);
   
    /// Recibe un mensaje que contiene un dato flotante
	void Recibe_Dato_Double(int procesador_origen,int TAG,double *con);
	
	/// Recibe un mensaje que contiene un dato entero	
	void Recibe_Dato_Entero(int procesador_origen,int TAG,unsigned long long  *con);
	
	/// Recibe un vector de tipo entero long long sin signo
	void Recibe_Vector_Entero(int procesador_origen,int TAG,unsigned long long  largo,unsigned long long  *vector,MPI_Status status);
	
	/// Recibe un vector de tipo double
	void Recibe_Vector_Double(int procesador_origen,int TAG,unsigned long long  largo,double *vector,MPI_Status status);
	
	/// Recibe un vector de tipo double indicando la posicion del indicie desde donde se recibira la informacion
	void Recibe_Vector_Double_Puntero(int procesador_origen,int TAG,unsigned long long  posicion_indice,unsigned long long  largo,double *vector,MPI_Status status);
	
};
#endif
