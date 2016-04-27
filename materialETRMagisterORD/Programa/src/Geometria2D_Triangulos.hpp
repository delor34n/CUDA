#ifndef _Geometria2D_Triangulos_
#define _Geometria2D_Triangulos_
#include "Geometria2D.hpp"
#include "Elemento_Triangular.hpp"
#include "Elemento_Frontera.hpp"
#include "ETRAD.hpp" 
#include "Vector_Entero.hpp"

/// Clase para generar una geometria triangular 2D
class Geometria2D_Triangulos : public Geometria2D
{
   /// Arreglo de objetos Nodo_2D[] que contiene los nodos de la geometria	 
   Nodo_2D *Nodos_Geometria;
   /// Vector que contiene los identificadores de los nodos frontera
   Vector_Entero *Nodos_Frontera_Geometria;	
   /// Arreglo de objetos Elemento_Triangular[] que almacena todos los elementos de la geometria	
   Elemento_Triangular *Elementos_Geometria;	
   /// Arreglo de objetos Elemento_Frontera[] que almacena todos los elementos que pertenecen a la frontera (borde) de la geometria	
   Elemento_Frontera *Fronteras_Geometria;
   /// Arreglo de Objetos Nodo_2D[] que almacenan puntos fuente 	
   Nodo_2D *Fuentes;
   /// Contenedor puntos elementos (REVISAR IMPLEMENTACION -  FUENTES)	
   Vector_Entero *CP_Elemento; 
   /// Area del dominio global
   double Area_Dominio;
   /// Numero de puntos internos (puntos de la grilla que pertenecen a la geometria FEM)
   unsigned long long  nPuntosInterior;	
   /// Numero de puntos frontera	
   unsigned long long  nFronteraDominio;
	
   public:

   /// Fase en la fuente 
   double fEsfera; 
    
   
   /// Constructor de la clase
   Geometria2D_Triangulos(void)
   {}

   /// Destructor de la clase
   ~Geometria2D_Triangulos()
   {
      delete Nodos_Geometria;
      delete Elementos_Geometria;
       
   }
   
   /// Almacena el numero de puntos interiores
   void Set_nPuntosInterior(int nPuntos)
   {nPuntosInterior=nPuntos;}
   
   unsigned long long  Retorna_nPuntosInterior()
   {return nPuntosInterior;}
   
   /// Calculo de area global del dominio
   void Calculo_Area_Global();
   
   ///Amacena el area global del dominio
   void Almacenar_Area_Dominio(double aGlobal)
   {Area_Dominio=aGlobal;}
	
	///Retorna el area del dominio global
    double Area_Global()
	{return Area_Dominio;}
   
   /// Calcula los vectores de carga de todos los elementos del dominio
   void Generar_Vectores_de_Cargas();
   
   /// Calcula el vector de carga de un elemento "id"
   void Generar_Vector_Carga_Elemento(unsigned long long  id);
   
   /// Calcula la funcion de forma del Elemento "id", funcion "indice" y Nodo "A1"
   double Funcion_Nodal(unsigned long long  id,int indice,Nodo_2D A1);
   
   /// Calcula la funcion de forma con los parametros: Area del elemento, Ai, Bi y Ci (constantes geometricas del elemento) del elemento y un punto pX y pY cualquiera
   double Get_Funcion_Nodal(double areaE,double Ai,double Bi,double Ci,double pX,double pY);
   
   /// Retorna la constante geometrica del elemento (Ai, Bi o Ci) indicada por "coeficiente" de la funcion de forma "indice" del elemento, esta es una variante del calculo nodal
   double Permutacion_Nodal(Nodo_2D A1,Nodo_2D A2,Nodo_2D A3,int coeficiente,int indice);
   
   /// Busca el valor minimo de los nodos del elemento "id" en el eje X (0) o Y (1), util para obtener coordenadas locales del elemento 
   double Buscar_Mininos_Nodos(unsigned long long  id,int eje);
   
   /// Obtiene y muestra  las coordenadas locales del elemento "id"
   void Obtener_Coordenadas_Locales_Elemento(unsigned long long  id);
   
   /// Retorna la orientacion de un elemento 
   int Orientacion_Elemento(unsigned long long  id);
   
   /// Retorna la orientacion de un triangulo dado 3 Nodos
   int Orientacion_Triangulo(Nodo_2D A1,Nodo_2D A2,Nodo_2D A3);
	
   /// Verifica si un punto pertence al interior de un elemento finito triangular id	
   int Punto_Interior_Triangulo(unsigned long long  id,Nodo_2D Punto);
   
   /// Retorna la orientacion de un triangulo dadas las 3 coordenadas
   int Orientacion_Triangulo_Coordenado(double x1,double x2,double x3,double y1,double y2,double y3);
	
   /// Verifica si un punto pertence al interior de un triangulo dadas las tres coordenadas de un triangulo y el punto a evaluar
   int Punto_Interior_Triangulo_Coordenado(double x1T,double x2T,double x3T,double y1T,double y2T,double y3T,double px,double py);
   
   /// Busca los elementos vecinos a un nodo 
   void Conjunto_Elementos_Vecinos_a_Nodo(unsigned long long  idNodo);
   
   /// Calcula la distancia de un Nodo "idNodo" a un punto cualquiera (x,y) = (xCord,yCord)
   double Distancia_Nodo_Punto(unsigned long long  idNodo,double xCord,double yCord);
   
   /// Retorna la distancia de dos puntos
   double Distancia_Dos_Puntos(double xCord1,double xCord2,double yCord1,double yCord2);
   
   /// Calcula la distancia del centro del elemento "id" a un punto (x,y) = (xCord,yCord)
   double Distancia_Centro_Elemento_Punto(double xCord,double yCord,unsigned long long  id);
 
   /// Busca el Triangulo que tiene menor distancia desde su centro geometrico a un punto (x,y) = (xCord,yCord)
   void Buscar_Minino_Centroide_Triangulo_Fuente(double xCord,double yCord);
   
   /// Reserva memoria para las fuentes
   void Set_Reserva_Memoria_Fuentes(int nFuentes)
   {Fuentes=new Nodo_2D[nFuentes];}
   
    /// Reserva memoria vector contenedor de puntos fuente (VER LA FORMA DE CREAR SUBVECTORES DE LARGO VARIABLE SEGUN PERTENENCIAS )
    void Set_Reserva_Memoria_Contenedor_Punto_Elemento(unsigned long long  nElementosP) 
   {CP_Elemento=new Vector_Entero(nElementosP);
	} //Revisar y probar si funciona asignacion variable de memoria para cada Vector[i]
   
    /// Reserva memoria para el arreglo de objetos Nodo_2D[], que contendran informacion de nodos de la geometria	
    void Set_Reserva_Memoria_Nodos(unsigned long long  nNodos)
   {Nodos_Geometria=new Nodo_2D[nNodos];}
   
    /// Reserva memoria para el arreglo de objetos Elemento_Triangular[], que contendran informacion de los elementos de la geometria	
    void Set_Reserva_Memoria_Elementos(unsigned long long  nElementos)
   {Elementos_Geometria=new Elemento_Triangular[nElementos];}
   
	/// Reserva memoria para el arreglo de objetos Elemento_Frontera[], que contendran informacion de los elementos de la geometria que tienen nodos en el borde del dominio
    void Set_Reserva_Memoria_Fronteras(unsigned long long  nCaras)
   {
	   Fronteras_Geometria=new Elemento_Frontera[nCaras];
	   Nodos_Frontera_Geometria=new Vector_Entero(nCaras);
   }

	/// Carga las densidades nodales desde un archivo, cuyos valores fueron obtenidos por el metodo de elementos finitos
	void Carga_Densidad_Nodal_Archivo(char *arch);

	/// Funcion que carga las densidades nodales obtenidas por el metodo de elementos finitos 
    void Carga_Densidad_Nodal_Vector(Vector *X);
    
	/// Calcula las densidades de una serie de puntos definidos en una grilla - Este proceso utiliza notacion de objetos Nodo 2D y esta implementada para version secuencial
	void Calcula_Densidades_Puntos(Vector *dGrilla,unsigned long long *Puntos_Dominio);
	
	/// Calcula densidades a partir de una grilla de puntos - Esta funcion es la utilizada en los procesos paralelos, por eso se utiliza notacion vectorial y de punteros, ademas es mas facil de manipular en el envio y recepcion de mensajes
    void Calcula_Densidades_Puntos_Grilla(int procesador,Vector_Entero *pCentroides,unsigned long long  nElementos,Vector *DENSIDADES,double *Entrada,double *px,double *py,double *nx,double *ny,unsigned long long  *puntos_internos);

	/// Carga la informacion de los Nodos de la geometria desde el archivo en formato Matlab-PDETool
    void Carga_Nodos(char *arch);
   
	/// Carga la informacion de los Elementos de la geometria desde el archivo en formato Matlab-PDETool	
    void Carga_Elementos(char *arch);
	
	/// Carga la informacion de las Fronteras de la geometria desde el archivo en formato Matlab-PDETool	
    void Carga_Fronteras(char *arch);
   
    /// Muestra el nodo "id" de la geometria
    void Ver_Nodo(unsigned long long  id);
   
    /// Muestra todos los nodos de la geometria
    void Ver_Nodos(void);
	
	/// Muestra informacion de elemento "id" de la geometria
    void Ver_Elemento(unsigned long long  id);
	
	/// Muestra informacion de todos los elementos de la geometria
    void Ver_Elementos(void);
   
    /// Muestra las coordenadas (x,y) de un elemento "id" de la geometria
    void Ver_Coordenadas_Elemento(unsigned long long  id);
   
    /// Muestra las coordenadas (x,y) de todos los elementos de la geometria
    void Ver_Coordenadas_Todos_los_Elementos();
	
	/// Calcula el centroide del elemento "id" de la geometria
    void Centroide_Elemento(unsigned long long  id);
	
	/// Calcula los centroides de todos los elementos de la geometria 
    void Centroides_Elementos();
   
    /// Calcula la permutacion en orden natural, asociada a las funciones de forma del elemento "id" de la geometria
    void Permutacion_Elemento(unsigned long long  id);
   
    /// Calcula las permutaciones de todos los elementos de la geometria
    void Permutaciones_Elementos();

    /// Calcula el area del elemento "id" de la geometria
    void Area_Elemento(unsigned long long  id);
   
    /// Calcula todas las areas de los elementos de la geometria
    void Areas_Elementos();
   
    /// Muestra el area del elemento "id"
    void Ver_Area_Elemento(unsigned long long  id);
   
    /// Muestra las areas de todos los elementos
    void Ver_Areas_Elementos();
   
    /// Genera la matriz de rigidez de un elemento "id" de la geometria 
    void Generar_Matriz_Rigidez_Elemento(unsigned long long  id);
   
    /// Genera la matriz de masa de un elemento "id" de la geometria 
    void Generar_Matriz_Masa_Elemento(unsigned long long  id);
   
    /// Calcula el vector gradiente del elemento "id" en el nodo "nodo"
	void Vector_Gradiente_Elemento(unsigned long long  id,unsigned long long  nodo);
    
	/// Calcula la matriz gradiente del elemento "id" en el nodo "nodo"
    void Matriz_Gradiente_Elemento(unsigned long long  id,unsigned long long  nodo);
   
    /// Retorna el vector que contiene los identificadores de los nodos que estan en la frontera (borde) del dominio 
    Vector_Entero* Get_Nodos_Frontera_Geometria()
   {return Nodos_Frontera_Geometria;}
   
	/// Retorna la norma entre dos nodos
    double Norma_Dos_Nodos(unsigned long long  id_nodo1,unsigned long long  id_nodo2);
    
	/// Genera la matriz de pertencia del elemento "id"
    void Generar_Matriz_Pertenencia_Elemento(unsigned long long  id);
   
	/// Carga la variable fisica asociada a la fisico-masa del elemento "id" 
    void Carga_Variable_Fisica_M_Elemento(unsigned long long  id,double Valor);
   
	/// Carga la variable fisica asociada a la fisico-geometria del elemento "id"
    void Carga_Variable_Fisica_K_Elemento(unsigned long long  id,double Valor);
   
    /// Retorna el arreglo de objetos Nodo_2D[] de la Geometria	
    Nodo_2D* Retornar_Nodos_Geometria()
   {return Nodos_Geometria;}
   
    /// Retorna el arreglo de objetos Nodo_2D[] que tienen informacion de las fuentes
    Nodo_2D Retorna_Nodo_Fuente(int id)
   {return Fuentes[id];}
   
    ///Asigna como nodo revisado
    void Nodo_Fuente_Revisado(int id)
   {Fuentes[id].Set_bandera(1);}
	
    /// Retorna un Nodo_2D[] de la geometria 
    Nodo_2D Retornar_Nodos_Especifico(unsigned long long  id)
   {return Nodos_Geometria[id];}
   
    /// Retorna el arreglo de objetos Elemento_Triangular[] de la geometria 
    Elemento_Triangular* Retornar_Elementos_Geometria()
   {return Elementos_Geometria;}

   /// Retorna el arreglo de objetos Elemento_Frontera[] de la geometria 
   Elemento_Frontera* Retornar_Fronteras_Geometria()
   {return Fronteras_Geometria;}
   
   /// Carga variables fisicas del problema - Dependiendo de los tipos de problemas se deben agregar los distintos metodos de calculo de constantes
   void Carga_Variables_Fisicas_Elementos_ETRAD(ETRAD *TMP);
   
   /// Asigna el numero de nodos frontera en el dominio global
   void Set_Numero_Nodos_Frontera_Dominio_Global(unsigned long long  num)
   {nFronteraDominio=num;}
   
   /// Retorna el numero global de nodos en la frontera 
   unsigned long long  Get_Numero_Nodos_Frontera_Dominio_Global()
   {return nFronteraDominio;}
	
   /// Genera las matrices de masa de todos los elementos de la geometria
   void Generar_Matrices_de_Masa_Elementos();
   
   /// Genera las matrices de rigides de todos los elementos de la geometria
   void Generar_Matrices_de_Rigidez_Elementos();
   
   /// Lamada a generar una grilla y almacenar las coordenadas en vectores	
   void Generar_Puntos_Grilla_FEM(Vector *dominio,Vector_Entero *Resolucion,Matriz_Entera *Dim_Fila_Columna,Matriz_Entera *Bandera_Fila_Columna,Vector *gX,Vector *gY,Vector *CentroGeometrico,int Proceso);

	
};
#endif
