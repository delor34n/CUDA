#include <stdio.h>
#include "Matriz_Dispersa.hpp"


/// Multiplica una matriz dispersa por un vector, guardando en vector salida el resultado
void Matriz_Dispersa::Producto_MA_Vector(Vector *x,Vector *Salida)
	  {
	
      unsigned long long nDimensionFilasMatriz=vIndiceFila->Columnas()-1;
      unsigned long long LimiteInicial,LimiteFinal,nDatosRango;
	  double suma;
      if(nDimensionFilasMatriz!=x->Columnas())
      printf("...........Error de DImensiones en Producto Matriz - Vector!!!!!!!!!");  		  
	  
		for(unsigned long long k=0;k<nDimensionFilasMatriz;k++)
        {
		    suma=0.0;
			LimiteInicial=vIndiceFila->Retorna(k);
			LimiteFinal=vIndiceFila->Retorna(k+1);
			nDatosRango=LimiteFinal-LimiteInicial;
			for(unsigned long long i=0;i<nDatosRango;i++)
			{
				suma+=vDato->Retorna(LimiteInicial+i)*x->Retorna(vColumna->Retorna(LimiteInicial+i));
			}	
			
			Salida->Asigna(k,suma);
			suma=0.0;
					
		}			 
}

/// Cuenta elementos no ceros de la matriz
unsigned long long  Matriz_Dispersa::Conteo_Datos(Matriz *Entrada)
{
unsigned long long contadorDatos=0;
unsigned long long nFil,nCol;
nFil=Entrada->Filas();
nCol=Entrada->Columnas();

	
	for(unsigned long long  i=0;i<nFil;i++)
		for(unsigned long long j=0;j<nCol;j++)	
		{
			  if(Entrada->Retorna(i,j)!=0.0)
			  {contadorDatos++;}
		}
		
return contadorDatos;		
}

/// Transforma un matriz dispera a formato de matriz CSR
void Matriz_Dispersa::Almacenar_Formato_CSR(Matriz *Entrada)
{
	unsigned long long contadorTMP=0;
	unsigned long long contadorIndice=0;
	unsigned long long contadorDatos = Conteo_Datos(Entrada);
	int Bandera_Fila_Revisada=1;
	unsigned long long nFil,nCol;
    nFil=Entrada->Filas();
    nCol=Entrada->Columnas();
	vIndiceFila=new Vector_Entero(nFil+1);
	vColumna=new Vector_Entero(contadorDatos);
	vDato=new Vector(contadorDatos);

	vIndiceFila->Asigna(contadorTMP,0);
	contadorTMP++;
	for(unsigned long long  i=0;i<nFil;i++)
	{
			for(unsigned long long  j=0;j<nCol;j++)	
			{
				  if(Entrada->Retorna(i,j)!=0.0)
				  {
					  					  
					  if(Bandera_Fila_Revisada==0)
					  {
		                Bandera_Fila_Revisada=1;
						vIndiceFila->Asigna(contadorTMP,contadorIndice);
                        contadorTMP++;
					  }	  
					  
					  vDato->Asigna(contadorIndice,Entrada->Retorna(i,j));
					  vColumna->Asigna(contadorIndice,j);
				      contadorIndice++;
					  
				  }
				
			}
	Bandera_Fila_Revisada=0;
    
			
   }
   vIndiceFila->Asigna(contadorTMP,contadorIndice); 

}

/// Transforma una matriz dispersa con datos de entrada map a formato de matriz CSR
void Matriz_Dispersa::Almacenar_Formato_CSR_Hash(map<unsigned long long ,double> ENTRADA,unsigned long long nFilas,map<unsigned long long,unsigned long long> mFILA,map<unsigned long long,unsigned long long> mCOLUMNA)
{
	unsigned long long contadorIndice=0;
	unsigned long long contadorDatos = ENTRADA.size ();
	Vector_Entero *vTMP=new Vector_Entero(nFilas + 1);
	vColumna=new Vector_Entero(contadorDatos);
	vDato=new Vector(contadorDatos);
	vTMP->Asigna(0,0);

	for(map<unsigned long long,double>::iterator it=ENTRADA.begin(); it!=ENTRADA.end(); it++)
	{ 
		  	  vTMP->Asigna( mFILA[(*it).first] , vTMP->Retorna(mFILA[(*it).first]) + 1);
	   		  vDato->Asigna(contadorIndice,ENTRADA[(*it).first]);
			  vColumna->Asigna(contadorIndice, mCOLUMNA[(*it).first]);
			  contadorIndice++;
    
	}
    
	vIndiceFila=new Vector_Entero(nFilas + 1);	
	vIndiceFila->Asigna(0,0);	
	
	for(unsigned long long  i=1;i<nFilas+1;i++)
	{
		vIndiceFila->Asigna(i,vTMP->Retorna(i-1) + vIndiceFila->Retorna(i-1));	
	}
    
		
}
