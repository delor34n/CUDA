#ifndef _Geometria2D_
#define _Geometria2D_
#include "Geometria.hpp"
#include "Matriz_Entera.hpp"


/// Clase base para generar una geometria en 2D
class Geometria2D : public Geometria
{
  protected:
  
  /// Tipo de geometria, 2 - Triangulo, 3 Cuadrilatero
  int tipoGeometria2D; 
  
  ///Numero de particiones - Uitlizado en calculo paralelo
  int nParticiones;
  
  /// Numero Filas particion paralela
  int FilaGrilla;
  
  /// Numero de columnas particion paralela
  int ColumnaGrilla;
  
  /// Resolucion X,Y
  int ResolucionX,ResolucionY;
  
  /// Fronteras de la Grilla
  double minX,minY,maxX,maxY;
  
  /// Area diferencial
  double Area_DIferencial;
  
  /// Tipo Grilla aproximacion en vertice = 1, en centro geometrico de 4 vertices que forman un cuadrilatero = 2
  int tipoGrilla; 
  
  /// Distancias de la grilla
  double distanciaX,distanciaY;
  
  public:

  /// Grilla que almacena datos del dominio de la geometria 1=pertenece al dominio, 0 EOC
  Matriz_Entera *GrillaDominio;
  
  /// Puntos a evaluar de la grilla
  Nodo_2D *PuntosGrilla;
  
  /// Centroides de particiones de la grilla, utilizado en la seleccion de elementos que estan contenidos en las subgrillas particionadas para el proceso paralelo
  Nodo_2D *CentroidesGrilla;
  
  /// Numero de puntos a calcular de la grilla  
  unsigned long NumeroPuntosGrilla;
  
   /// Constructor de la clase
   Geometria2D(void)
   {}

   /// Destructor de la clase
   ~Geometria2D()
   {
       
   }
 
  
  /// Asigna el tipo de geometria 2D
  void Asigna_Tipo_Geometria(int tipoGeometria)
  { 
  		tipoGeometria2D = tipoGeometria;
  }
  
  /// Retorna la pertenencia del dominio de la grilla
  Matriz_Entera* Get_Grilla()
  {
	return  GrillaDominio; 
  }
  
  /// Retorna puntos de la grilla
  Nodo_2D Retornar_Punto_Grilla(unsigned long id)
   {return PuntosGrilla[id];}
  
   /// Retorna los centroides de la grilla
   Nodo_2D Retornar_Centroide_Grilla(unsigned long id)
   {return CentroidesGrilla[id];}
  
   /// Retorna puntero de los puntos de la grilla
    Nodo_2D *Retornar_Puntos_Grilla()
   {return PuntosGrilla;}
   
   /// Retorna puntero de centroides de la grilla
  Nodo_2D *Retornar_Centroides_Grilla()
   {return CentroidesGrilla;}
   
  /// Retorna el tipo de geometria 2D
  int Retorna_Tipo_Geometria()
  { 
  return tipoGeometria2D;
  }
  
   /// Calcula el area diferencial por cada nodo a partir de una grilla de entrada
  double Calculo_Area_Diferencial()
  {
	return  (double)( ( Get_DistanciaX() * Get_DistanciaY() )/Get_NumeroPuntosGrilla()); 
  }
  
   /// Almacena los minimo del dominio global 
   void Set_MinXY(double mX,double mY)
  {
	minX=mX;
	minY=mY; 
  }
  
  /// Almacena los maximos del dominio global
   void Set_MaxXY(double mX,double mY)
  {
	maxX=mX;
	maxY=mY; 
  }
  
  /// Almacena el numero total de puntos de la grilla
   void Set_NumeroPuntosGrilla(unsigned long long valor)
  {NumeroPuntosGrilla=valor;}

  /// Retorna el numeo de puntos de la grilla
   unsigned long Get_NumeroPuntosGrilla()
  {return NumeroPuntosGrilla;}
  
  /// Almacena la distancia entre el minimo y el maximo del eje X de la grilla 
   void Set_DistanciaX(double valor)
  {distanciaX=valor;}
  
  /// Almacena la distancia entre el maximo y el minimo del eje Y de la grilla
   void Set_DistanciaY(double valor)
  {distanciaY=valor;}
  
  /// Asigna el numero de filas a particionar la grilla en proceso paralelo
  void Set_FilaGrilla(int valor)
  {FilaGrilla=valor;}
  
  /// Asigna el numero de Columnas a particionar la grilla en el proceso paralelo
  void Set_ColumnaGrilla(int valor)
  {ColumnaGrilla=valor;}

  /// Asigna la resolucion X de la grilla
   void Set_ResolucionX(unsigned long long valor)
  {ResolucionX=valor;}

  /// Asigna la resolucion Y de la grilla
   void Set_ResolucionY(unsigned long long valor)
  {ResolucionY=valor;}
  
  /// Retorna la distancia de X
   double Get_DistanciaX()
  {return distanciaX;}
  
  /// Retorna la distancia de Y
   double Get_DistanciaY()
  {return distanciaY;}
  
  /// Retorna el numero de filas de la grilla
   int Get_FilaGrilla()
  {return FilaGrilla;}
  
  /// Retorna el numero de columnas de la grilla
   int Get_ColumnaGrilla()
  {return ColumnaGrilla;}
  
  /// Retorna el maximo valor del dominio en X
  double Get_MaxX()
  {return maxX;}

  /// Retorna el maximo valor del dominio en Y
  double Get_MaxY()
  {return maxY;}

  /// Retorna el minimo valor del dominio en X
  double Get_MinX()
  {return minX;}

  /// Retorna el minimo valor del dominio en Y
  double Get_MinY()
  {return minY;}

  	/// Retorna la resolucion en X
  int Get_ResolucionX()
  {return ResolucionX;}
  	
   /// Retorna la solucion en Y
   int Get_ResolucionY()
   {return ResolucionY;}
  
    /// Calcula particiones Grilla
    void Calcula_Particiones_Grilla(int particiones);
	   
	/// Carga la informacion de la imagen binaria del dominio a calcular densidad 
	void Carga_Imagen_Binaria(char *arch);
	 
	/// Carga la informacion de puntos donde se desea obtener densidad 
	int Carga_Puntos_Archivo(char *arch);
	   
	/// Genera una grilla de puntos a partir de informacion particionada de la grilla y almacena los centros geometricos de la grilla
	void Generar_Grilla_Vectores(Vector *dominio,Vector_Entero *Resolucion,Matriz_Entera *Dim_Fila_Columna,Matriz_Entera *Bandera_Fila_Columna,Vector *gX,Vector *gY,Vector *CentroGeometrico,int Proceso);
	
	/// Genera una grilla para un procesador
	void Generar_Grilla_Memoria(Vector *dominio,Vector_Entero *Resolucion);
	  
  
};
#endif
