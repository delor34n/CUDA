#include <string.h>
#include "Archivo.hpp"
#include "ETRAD.hpp"

/// Retorna el coeficiente de difusion de la ecuacion de difusion de la luz, dado absorcion, scattering y funcion de fase
double ETRAD::Coeficiente_difusion(double UaValor,double UsValor,double fase)
{ 
	   double cDifusion;
	   cDifusion=1.0/(3.0*(UaValor + (1.0-fase)*UsValor));
	   return cDifusion;
}
   
/// Retorna el coseno promedio dado la funcion de fase y el scattering, segun propuesta de la Tesis de Tarvainen 2006 
double ETRAD::Coseno_Promedio_funcion_Fase2D(double g,double pScatter)
{
	   double fTmp;
	   fTmp=(1.0/(2.0*PI))*(1.0-pow(g,2))/(1.0+pow(g,2)-2.0*g*pScatter);
       return fTmp; 
}


/// Carga del archivo que contiene los datos de absorcion en los distintos subdominios
void ETRAD::Carga_Absorcion(char *arch)
{

char UaTMP[1024];
char DomTMP[1024];
int contador_Dominios=0;
char cadena[150];
char* cpToken;
int DominioTMP,nDom;	
double Ua_TMP;

Archivo *Absorcion_Archivo=new Archivo();
Absorcion_Archivo->SetNombre(arch);
Absorcion_Archivo->SetModo(LEE_ARCHIVO);	
Absorcion_Archivo->Abrir();	
nDom=Absorcion_Archivo->Numero_Lineas();
Set_Longitud_Vector_Ua(nDom);
	
		while (fscanf (Absorcion_Archivo->GetArchivo(), "%s", cadena) != EOF)
		{
		    cpToken = strtok (cadena, ","); 
			strcpy (UaTMP, cpToken); 
			Ua_TMP=atof(UaTMP);
			cpToken = strtok (NULL, ",");
			strcpy (DomTMP, cpToken); 
			DominioTMP=atoi(DomTMP)-1;
			Set_Valor_Ua(DominioTMP,Ua_TMP);
            printf("La Absorcion V %d es:%f \n",DominioTMP,Ua->Retorna(DominioTMP));	
	        contador_Dominios++;
		}
	Absorcion_Archivo->Cerrar();	 
}

/// Carga el archivo que contiene los datos de scattering en los distintos subdominios
void ETRAD::Carga_Scattering(char *arch)
{
	//LECTURA Y CARGA DE FRONTERAS.DAT 
char UsTMP[1024];
char DomTMP[1024];
int contador_Dominios=0;
char cadena[150];
char* cpToken;
int DominioTMP,nDom;	
double Us_TMP;

Archivo *Scattering=new Archivo();
Scattering->SetNombre(arch);
Scattering->SetModo(LEE_ARCHIVO);	
Scattering->Abrir();	
nDom=Scattering->Numero_Lineas();
Set_Longitud_Vector_Us(nDom);

		while (fscanf (Scattering->GetArchivo(), "%s", cadena) != EOF)
		{
		    cpToken = strtok (cadena, ","); 
			strcpy (UsTMP, cpToken); 
			Us_TMP=atof(UsTMP);
			cpToken = strtok (NULL, ",");
			strcpy (DomTMP, cpToken); 
			DominioTMP=atoi(DomTMP)-1;
			Set_Valor_Us(DominioTMP,Us_TMP);
	        printf("La Scattering V %d es:%f \n",DominioTMP,Us->Retorna(DominioTMP));	
	 contador_Dominios++;
		}
	Scattering->Cerrar();	 
 
}

/// Carga el archivo que contiene los datos de las fuentes
void ETRAD::Carga_Fuentes(char *arch)
{

char xTMP[1024];
char yTMP[1024];
char nFotFuente[1024];	
char cadena[150];
char* cpToken;
int nFuentes,contador=0;
double xF,yF,nF;
Suma_Fotones_Fuentes=0;
Archivo *Fuentes=new Archivo();
Fuentes->SetNombre(arch);
Fuentes->SetModo(LEE_ARCHIVO);	
Fuentes->Abrir();	
nFuentes=Fuentes->Numero_Lineas();
Fuentes_Fotones=new Matriz(nFuentes,3);
	
		while (fscanf (Fuentes->GetArchivo(), "%s", cadena) != EOF)
		{
		    cpToken = strtok (cadena, ","); 
			strcpy (xTMP, cpToken); 
			xF=atof(xTMP);
			cpToken = strtok (NULL, ",");
			strcpy (yTMP, cpToken); 
			yF=atof(yTMP);
			cpToken = strtok (NULL, ",");
			strcpy (nFotFuente, cpToken); 
			nF=atof(nFotFuente);
			Suma_Fotones_Fuentes+=(unsigned long long)nF;
			Fuentes_Fotones->Asigna(contador,0,xF);
			Fuentes_Fotones->Asigna(contador,1,yF);
			Fuentes_Fotones->Asigna(contador,2,nF);
			printf("Fuente %d ubicada en (%f,%f) con %f fotones (1/cm2)\n",contador, Fuentes_Fotones->Retorna(contador,0), Fuentes_Fotones->Retorna(contador,1), Fuentes_Fotones->Retorna(contador,2));	
	 contador++;
		}
		
	Fuentes->Cerrar();	
  
}
