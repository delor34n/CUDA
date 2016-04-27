#ifndef _ETRAD_
#define _ETRAD_
#include "Matriz.hpp"
#include "Matriz_Entera.hpp"
#include "Problema.hpp"
#define PI 3.14159265 

/// Clase que define las caracteristicas del problema de difusion de la luz en medios difusos
class ETRAD:public Problema
{
    protected:
	/// Vector que contiene los coeficientes de absorcion en cada subdominio del problema	
	Vector *Ua;
	
	/// Vector que contiene los coeficientes de Dispersion - Scattering en cada subdominio del problema	
	Vector *Us;
	
	/// Puntos fuente (REVISAR IMPLEMENTACION Y USO)
	Vector_Entero *Puntos_Fuente;
	
	/// Fuentes y densidades respectivas 
	Matriz *Fuentes_Fotones;
	
	/// Velocidad de la luz en el medio
    double cLuz;	
    
	/// Funcion de fase
	double fFase;	
	
	/// Tipo de aproximacion del problema, 1-Eliptico (sin calculo temporal asociado) 2-Hiperbolico (con calculo temporal asociado)
	int tipo_aproximacion;
	
	/// Valor 1 si se usa condicion de borde en los subdominios interiores, 0 en caso contrario (REVISAR IMPLEMENTACION)
    int borde_interno;
	
	/// Valor 1 si se usa condicion de borde distinta de 0 en el borde del dominio (REVISAR IMPLEMENTACION)
    int borde_externo;
	
	/// Numero de epocas en caso de utilizar funcion Hiperbolica (REVISAR IMPLEMENTACION)
	int tiempo;
	
	/// Nombre del archivo que contiene los datos de absorsion
    char *NombreArchivoAbsorcion;
	
	/// Nombre del archivo que contiene los datos de Scattering
	char *NombreArchivoScattering;
	
	/// Nombre del archivo que contiene los datos de las fuentes de fotones
	char *NombreArchivoFuentes;
	
    public:

    unsigned long long Suma_Fotones_Fuentes; 
    /// Construtor de la clase
    ETRAD(void)
    {

    }

    /// Destructor de la clase
    ~ETRAD()
    {
      delete Ua;
      delete Us;
    }
    
   /// Retorna el coeficiente de difusion de la ecuacion de difusion de la luz, dado absorcion, scattering y funcion de fase
   double Coeficiente_difusion(double UaValor,double UsValor,double fase);

   /// Retorna el coseno promedio dado la funcion de fase y el scattering, segun propuesta de la Tesis de Tarvainen 2006 (REVISAR IMPLEMENTACION, NO SE ESTA USANDO, POSIBLE ELIMINACION)
   double Coseno_Promedio_funcion_Fase2D(double g,double pScatter);
   
   /// Asigna el valos de la funcion de fase en la difusion
   void Asigna_Funcion_Fase(double fase)
   {fFase =fase;}	
	
   /// Carga del archivo que contiene los datos de absorcion en los distintos subdominios   
   void Carga_Absorcion(char *arch);
       
   /// Carga el archivo que contiene los datos de scattering en los distintos subdominios
   void Carga_Scattering(char *arch);
	
   void Carga_Fuentes(char *arch);
   
   /// Asigna el nombre del archivo aociado a la absorcion
   void SetNombreArchivoAbsorcion(char *nombre)
   {
	  NombreArchivoAbsorcion=nombre; 
   }
   
   /// Asigna el nombre del archivo asociado al scattering
   void SetNombreArchivoScattering(char *nombre)
   {
	  NombreArchivoScattering=nombre; 
   }
  
   ///  Asigna el nombre del archivo asociado a las fuentes de fotones
   void SetNombreArchivoFuentes(char *nombre)
   {
	  NombreArchivoFuentes=nombre;   
   }
   
   
   /// Retorna el nombre del archivo asociado a la absorcion   
   char* GetNombreArchivoAbsorcion()
   {
   return NombreArchivoAbsorcion; 
   }
   
   /// Retorna el nombre del archivo asociado al scattering   
   char* GetNombreArchivoScattering()
   {
   return NombreArchivoScattering; 
   }
      
   /// Retorna el nombre del archivo asociado a las fuentes   
   char* GetNombreArchivoFuentes()
   {
   return NombreArchivoFuentes; 
   }
   
   /// Reserva memoria para el vector que contendra los datos de absorcion  
   void Set_Longitud_Vector_Ua(int longitud)
   {  
	   Ua=new Vector(longitud);
   }
   
   /// Reserva memoria para el vector que contendra los datos de scattering  
   void Set_Longitud_Vector_Us(int longitud)
   {   Us=new Vector(longitud);
	   	
   }
    
   /// Asigna el valor de absorcion al vector Ua en la posicion "indice"
   void Set_Valor_Ua(int indice, double valor)
   {  
	   Ua->Asigna(indice,valor);
   }
   
   /// Asigna el valor de scattering al vector Us en la posicion "indice"
   void Set_Valor_Us(int indice, double valor)
   {  
	   Us->Asigna(indice,valor);
   }
   
   /// Retorna el vector que contiene los datos de absorcion
   Vector *Get_Vector_Ua()
   {
	return Ua;   
   }
  
   /// Retorna el vector que contiene los datos de scattering
   Vector *Get_Vector_Us()
   {
	return Us;   
   }
   
   /// Retorna el vector que contiene los datos de scattering
   Matriz *Get_Fuentes()
   {
	return Fuentes_Fotones;   
   }
   
   /// Retorna la suma del numero de fotones provenientes de las fuentes en todo el dominio
   unsigned long long Get_Suma_Fuentes()
   {
   return Suma_Fotones_Fuentes;   
   }
   
   /// Retorna el valor de absorcion del vector Ua en la posicion "id"
   double Get_Valor_Ua(int id)
   {
	return Get_Vector_Ua()->Retorna(id);   
   }
   
   /// Retorna el valor de absorcion del vector Us en la posicion "id"
   double Get_Valor_Us(int id)
   {return Get_Vector_Us()->Retorna(id);}
   
   /// Retorna el valor de la funcion de fase
   double Get_Funcion_Fase()
   {return fFase;}
   
      
};
#endif
