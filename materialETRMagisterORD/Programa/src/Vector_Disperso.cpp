#include "Vector_Disperso.hpp"

/// Cuenta el numero de elementos no ceros de un vector
unsigned long Vector_Disperso::Conteo_Datos_VD(Vector *Entrada)
{
unsigned long contadorDatos=0;
unsigned long nCol;
nCol=Entrada->Columnas();

		for(unsigned long j=0;j<nCol;j++)	
		{
			  if(Entrada->Retorna(j)!=0.0)
			  {contadorDatos++;}
		}
nzDatos=contadorDatos;		
return contadorDatos;		
}



/// Transforma un vector disperso a un formato Columna - Dato
void Vector_Disperso::Almacenar_Formato_VDC(Vector *Entrada)
{
	
	unsigned long contadorDatos = Conteo_Datos_VD(Entrada);
	unsigned long nCol=dimOriginal=Entrada->Columnas();
	unsigned long contadorIndice=0;
    vdColumnas = new Vector_Entero(contadorDatos);
    vdDato = new Vector(contadorDatos);	


			for(unsigned long j=0;j<nCol;j++)	
			{
				  if(Entrada->Retorna(j)!=0.0)
				  {
					  					  
					  vdColumnas->Asigna(contadorIndice,j);
                      vdDato->Asigna(contadorIndice,Entrada->Retorna(j));
				      contadorIndice++;
					  
				  }
				
			}

	vdColumnas->Visualiza(); 		
	vdDato->Visualiza(); 

}
