#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "Archivo.hpp"
#include <iostream> 
#define ESCRITURA_OK      06
#define LECTURA_OK      04



/// Cierra archivo
void Archivo::Cerrar(void)
{
   if (ARCHIVO_TRABAJO) {
      fflush(ARCHIVO_TRABAJO);
      fclose(ARCHIVO_TRABAJO);
   }
   ARCHIVO_TRABAJO = NULL;
}
///Abre archivo
void Archivo::Abrir()
{
 if (MODO == LEE_ARCHIVO)      ACCESO = "r";
 if (MODO == GRABA_ARCHIVO)    ACCESO = "wb";
 if (MODO == GRABA_LINEA_ARCHIVO)    ACCESO = "a";	 
   ARCHIVO_TRABAJO = fopen(ARCHIVO,ACCESO);
    if (ARCHIVO_TRABAJO == NULL) 
	{
        printf ("No se pudo abrir el archivo: %s\n", ARCHIVO);
     }

}

///Retorna el numero de lineas del archivo
unsigned long long Archivo::Numero_Lineas()
{
unsigned long	long nlineas;
char cadena[10000];
	if(ARCHIVO_TRABAJO!=NULL)
	{
	nlineas=0;
		while(fscanf (ARCHIVO_TRABAJO, "%s", cadena) != EOF)
		{
		//printf("El numero lineas del archivo es: %d\n",nlineas);	
		nlineas++;
		}
	}
rewind(ARCHIVO_TRABAJO);
return nlineas;
}
/// Escribe un Vector a un archivo
void Archivo::Escribe_Vector(Vector *a,unsigned long long nodos)
{
	
	 for(unsigned long long  i=0;i<nodos;i++) 
	 { 
		 
		 fprintf (ARCHIVO_TRABAJO, "%4.16f\n", a->Retorna(i));

	 }
}
/// Escribe una matriz cuadrada a un archivo
void Archivo::Escribe_MatrizCuadrada(Matriz *a,unsigned long long nodos)
{
	

	 for(unsigned long long i=0;i<nodos;i++) 
	 for(unsigned long long j=0;j<nodos;j++) 
	 {
	
		
	 fprintf (ARCHIVO_TRABAJO, "%4.8f,", a->Retorna(i,j));
	 if(j==nodos-1)
	 fprintf (ARCHIVO_TRABAJO,"\n");
	 }
}

/// Escribe una matriz cuadrada a un archivo
void Archivo::Escribe_Matriz(Matriz *a)
{
	

	 for(unsigned long long i=0;i<a->Filas();i++) 
	 for(unsigned long long j=0;j<a->Columnas();j++) 
	 {
	
		
	 fprintf (ARCHIVO_TRABAJO, "%4.8f,", a->Retorna(i,j));
	 if(j==a->Columnas()-1)
	 fprintf (ARCHIVO_TRABAJO,"\n");
	 }
}


/// Muestra el nombre del archivo
void Archivo::VerNombre()
{ 
	printf("El nombre del archivo es: %s",GetNombre());
}
