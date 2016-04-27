#ifndef _FEM_
#define _FEM_
#include "Geometria2D_Triangulos.hpp"
#include "ETRAD.hpp"
#include "Problema.hpp"
#include "Sistema_Lineal.hpp"
#include <map>
 using namespace std; 
/// Indica que se utilizara geometria basada en elementos triangulares
#define GEOMETRIA_TRIANGULOS      	 1
/// Indica que se utilizara geometria basada en elementos cuadrilateros
#define GEOMETRIA_CUADRILATEROS     2
/// Indica que se utilizara geometria basada en elementos tetrahedros
#define GEOMETRIA_TETRAHEDROS   	 3 
/// Indica que se utilizara una programacion secuencial
#define CALCULO_SECUENCIAL         	10
/// Indica que se utilizara una programacion paralela
#define CALCULO_PARALELO           	    20
/// Metodo Jacobi
#define JACOBI 									    1  //Metodo Iterativo secuencial
/// Metodo Gauss - Seidel 
#define GAUSS_SEIDEL							2   //Metodo Iterativo secuencial
/// Metodo LSQR  
#define LSQR											3   //Metodo Iterativo secuencial
/// Metodo Factorizacion LU 
#define LU 												4   //Metodo Directo secuencial
/// Metodo Factorizacion Cholesky LLT
#define LLT 												5   //Metodo Directo secuencial
/// Metodo LSQR para matrices Dispersas 
#define LSQR_DISPERSO						6   //Metodo Iterativo secuencial y paralelo


///Clase que define un procedimiento general del metodo del elemento finito - FEM (Siglas en ingles)
class FEM
{
  
  protected:

  /// Matriz de riguidez completa del sistema ( Matriz stiffness (Fisico-geometria) + Matriz Masa (fisico-masa)  ) 
   Matriz *A;
  
   /// Vector que contendra los valores conocidos del vector de carga global (Vector Fuente)
   Vector *B;
  
   /// Vector incognita del problema, son los valores desconocidos del problema
   Vector *X;
   
   /// Vector Fronteras 
   Vector_Entero *FronteraDominioGlobal;
   
   /// Declaracion del sistema lineal a resolver
   Sistema_Lineal *Sistema;
  
   /// Numero de procesadores a utilizar
   int nProcesadores;
  
   /// Puntero de las rutinas paralelas
   Rutinas_Paralelas *Proceso_Paralelo;
  
     /// Puntero a Geometria de trabajo a utilizar
   Geometria2D_Triangulos *GeometriaTrabajo;
  
  public:

  /// Nombre archivo nodos
  char *NombreArchivoGrilla;
  /// Nombre archivo nodos
  char *NombreArchivoNodos;
  /// Nombre archivo elementos
  char *NombreArchivoElementos;
  /// Nombre archivo fronteras
  char *NombreArchivoFronteras;
  /// Nombre archivo solucion 
  char *NombreArchivoX;
  /// Nombre archivo tiempos 
  char *NombreArchivoTiempos;
  /// Tipo de geometria a utilizar
  int TIPO_GEOMETRIA; 
  /// Tipo calculo secuencial o paralelo
  int TIPO_CALCULO;
  /// Numero de procesadores en caso de utilizar programacion paralela (FALTA EVALUAR IMPLEMENTACION)
  int NUMERO_PROCESADORES;
  /// Tipo de solucion matricial a utilizar 1-Matriz completa, 2-Matriz reducida
  int tipo_solucion_matricial;
  /// Orden del sistema de ecuaciones
  unsigned long orden_nodal;
  /// Orden del sistema reducido - Condicion borde 0
  unsigned long orden_dirichlet;
  /// Tipo vector de cargas, 1-distribucion uniforme 2- Distribucion de Dirac  
  int tipo_vector_cargas;
  /// Indica cual es el dominio que contiene a la frontera global	 
  int FronteraExterior;
  /// Tiempo total utilizado en comunicacion
  double tTotalComunicacion;
  /// Tiempo total de ejecucion global
  double tTotalGlobal;
  /// Tiempos parciales comunicacion grilla
  double Tiempo_Inicial_Comunicacion_Grilla,Tiempo_Parcial_Comunicacion_Grilla,tTotalComunicacionGrilla;
  /// Tiempos parciales comunicacion FEM
  double Tiempo_Inicial_Comunicacion_FEM,Tiempo_Parcial_Comunicacion_FEM,tTotalComunicacionFEM;
  
  /// Mapeo de la Matriz de riguidez del sistema ( Matriz stiffness (Fisico-geometria) + Matriz Masa (fisico-masa)  )
  /// Se utiliza este mapeo para reservar memoria solo para los valoress no nulos de la matriz dispersa
   map<unsigned long long,double> Matriz_A_FEM;
  
   /// Mapeo de la informacion de las filas con elementos no nulos
   map<unsigned long long,unsigned long long> Mapeo_FEM_Fila;
   
   /// Mapeo de la informacion de las columnas con elementos no nulos
   map<unsigned long long,unsigned long long > Mapeo_FEM_Columna;
   
   /// Mapeo de los indice para aproximacion de borde tipo Dirichlet
   map<unsigned long long,unsigned long long > Mapeo_FEM_Dirichlet;
   
   /// Suma vector de cargas en magnitud de entrada
   double sCarga; 
	
  /// Constructor de la clase
   FEM()
   {
	   Sistema = new Sistema_Lineal();
   }

   /// Destructor de la clase
   ~FEM()
   {
      delete A;
      delete B;
      delete X;   
   }
   
   /// Almacena y suma los aportea a una Map que utiliza el Hashing como referencia para cada aporte nodal a la matriz A - se almacenan solo los datos no nulos
    void Hashing_FEM(unsigned long long indice,double aporte,unsigned long long  i,unsigned long long j)
   {

	   double tmp;
	   tmp =   Matriz_A_FEM[indice];
	   Matriz_A_FEM[indice] = aporte + tmp;
       Mapeo_FEM_Fila[indice] = i;
       Mapeo_FEM_Columna[indice]=j;
	   
	}
		
    /// Muetra la matriz de tipo Map donde se utiliza un hash como estructura de almacenamiento de la matriz A
	void Mostrar_Hashing_FEM()
	{
	unsigned long  long contador=0;	
	printf("\n");

		for(map<unsigned long long,double>::iterator it=Matriz_A_FEM.begin(); it!=Matriz_A_FEM.end(); it++)
		{ 
		   printf("[%Ld][%f] - A[%Ld][%Ld] \n",(*it).first,Matriz_A_FEM[(*it).first],Mapeo_FEM_Fila[(*it).first],Mapeo_FEM_Columna[(*it).first]);
		   contador++;
		}

		printf("\n Dimension Hash FEM= %Ld \n",contador);

	}
    /// Muestra el tiempo total utilizado en comunicacion   
    void Mostrar_Tiempo_Comunicacion()
	{
		printf("\nEl tiempo utlizado en comunicacion del programs fue de: %f segundos \n",tTotalComunicacion);
	}
	/// Muestra el tiempo global utilizado en el programa
	 void Mostrar_Tiempo_Global()
	{
		printf("\nEl tiempo total utilizado por el programa fue de: %f segundos \n",tTotalGlobal);
	}
    /// Retorna el tiempo usado en comunicacion	
	double Retornan_Tiempo_Comunicacion()
	{
		return tTotalComunicacion;
	}

	/// Retorna el tiempo total del programa
	double Retornar_Tiempo_Global()
	{
		return tTotalGlobal;
	}
	
  	/// Alamcena el numero de procesadores
	void SetProcesadores(int Procesadores)
   {
	nProcesadores=Procesadores;   
   }   
   
    /// Retorna el numero de procesos
    int GetProcesadores()
   {
	return nProcesadores;   
   }  
	
   /// Retorna la geometria de trabajo
   Geometria2D_Triangulos* Get_Geometria2D_Triangulos()
   {
    return GeometriaTrabajo;
	  
   }
   
   /// Asigna el nombre al archivo solucion
   void SetNombreArchivoX(char *nombre)
   {
	  NombreArchivoX=nombre; 
   }
   
   /// Asigna el nombre al archivo tiempos
   void SetNombreArchivoTiempos(char *nombre)
   {
	  NombreArchivoTiempos=nombre; 
   }
   
   /// Retorna el nombre del archivo tiempos
   char* GetNombreArchivoTiempos()
   {return NombreArchivoTiempos;}
   
   /// Retorna el nombre del archivo solucion
   char* GetNombreArchivoX()
   {return NombreArchivoX;}
   
      /// Retorna el nombre del archivo solucion
   char* GetNombreArchivoGrilla()
   {return NombreArchivoGrilla;}
   
   /// Asigna el nombre del archivo grilla
   void SetNombreArchivoNodos(char *nombre)
   {NombreArchivoNodos=nombre;}
   
   /// Asigna el nombre del archivo nodos
   void SetNombreArchivoGrilla(char *nombre)
   {NombreArchivoGrilla=nombre;}
   
   /// Asigna el nombre del archivo elementos
   void SetNombreArchivoElementos(char *nombre)
   {NombreArchivoElementos=nombre;}
   
   /// Asigna el nombre del archivo fronteras
   void SetNombreArchivoFronteras(char *nombre)
   {NombreArchivoFronteras=nombre;}
   
   /// Retorna el nombre del archivo nodos
   char* GetNombreArchivoNodos()
   {return NombreArchivoNodos;}
   
   /// Retorna el nombre del archivo elementos 
   char* GetNombreArchivoElementos()
   {
	  return NombreArchivoElementos; 
   }
   
   /// Retorna el nombre del archivo fronteras
   char* GetNombreArchivoFronteras()
   {
	  return NombreArchivoFronteras; 
   }
   
   /// Define el tipo de geometria a utilizar
   void SetGeometria(int TipoGeometria)
   {
	  TIPO_GEOMETRIA=TipoGeometria; 
   }
   
   /// Define el tipo de calculo a utilizar
   void SetTipoCalculo(int TipoCalculo)
   {
	  TIPO_CALCULO=TipoCalculo; 
   }
   
   /// Retorna el tipo de geometria utilizado
   int GetGeometria()
   {
	  return TIPO_GEOMETRIA; 
   }
   
   /// Retorna el tipo de de solucion matricial 1 - Matriz Completa, 2 - Matriz reducida
   int Get_tipo_solucion_matricial()
   {
	  return tipo_solucion_matricial; 
   }
   
   /// Retorna la sumas de magnitudes original del vector de cargas
   double Get_sCarga()
   {
	return sCarga;
   }   
   /// Asigna el tipo de solucion matricial
   void Set_tipo_solucion_matricial(int tipoM)
   {
	  tipo_solucion_matricial=tipoM; 
   }
   
   /// Retorna el orden nodal global
   unsigned long Get_orden_nodal()
   {
	  return orden_nodal; 
   }
   
  /// Asigna el orden nodal global
   void Set_orden_nodal(int oGlobal)
   {
	  orden_nodal=oGlobal; 
   }
   
    /// Asigna el suma de magnitudes original en vector de cargas
   void Set_sCarga(double sC)
   {
	  sCarga=sC; 
   }
   
   /// Define el numero de procesadores a utilizar en el calculo paralelo
   void SetNumeroProcesadores(int NumeroProcesadores)
   {
	   NUMERO_PROCESADORES=NumeroProcesadores;
   }
   
   /// Calcula la dendidad global de la solucion FEM, es una suma de densidades nodales por sus funciones de forma nodal (Se utiliza solo como funcion de traza del programa)
   void Densidad_FEM(void);
   
   /// Muestra el tipo de geometria que se esta utilizando
   void VerTipoGeometria(void);
   
   /// Carga la geometria  y calcula las areas de los elementos y matrices de permutacion elemental, que luego son utilizados al momento de generar las matrices de rigidez y de masa
   void CargarGeometriaFEM();
   
   /// Inicializa problema ETRAD (rECORDAR CAMBIAR IMPLEMNETACION Y DEJARLA EN UNA CLASE SEPARADA, SOLO SE DEJO ACA POR COMODIDAD DEPROGRMACION Y TRAZA)
   void Inicializa_Problema_ETRAD(ETRAD *FisicaProblema,int tipoFase);
   
   /// Calcula las matrices de rigidez, de masa y vector de carga de los elementos, 
   void Contruir_Matrices_Problema();
  
   /// Resolucion del sistema de ecuaciones
   void Resolver_Sistema_FEM(int metodo,int traza,unsigned long long iteraciones,int tipo_parada);

   /// Establece el tipo de tratamiento numerico con el cula se trabaja en la resolucion del sistema lineal (Matriz completa o reducida)
   void Establecer_Condicion_Matricial_FEM(int condicion,int metodo);

   /// Adquisicion de datos de la grilla a calcular
   void Ingreso_Datos_Grilla_FEM(Vector *dominio,Vector_Entero *Resolucion,int nProcesos);

   ///  Aproximacion nodal a partir de una grilla de puntos 
   void Calculo_Grilla_FEM();
 
   /// Carga datos de calculos elementales a una matriz global para futuro calculo paralelo 
   void Carga_Datos_Elementos_Grilla(void);
 
 
};
#endif
