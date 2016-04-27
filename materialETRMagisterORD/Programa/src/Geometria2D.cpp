#include "Geometria2D.hpp"


/// Calcula particiones de la grilla y almacena en memoria el numero de filas y columnas que se utilizara en el proceso paralelo de particion de la grilla
void Geometria2D::Calcula_Particiones_Grilla(int particiones)
{
	double pTmp;
	nParticiones=particiones;
	int marca,p1,p2;
	int *P1=new int[particiones];
   	int *P2=new int[particiones];
	int min1,min2;
	pTmp = sqrt((double)particiones);
	int divisor1=particiones-1;
	int contador2=1,contador=0;

			if(particiones!=1)
			{	
				if(pTmp-floor(pTmp)==0.0)
				{
					 p1=p2=(int)pTmp;
					 marca=0;
					 P1[marca]=p1;
					 P2[marca]=p2;
					 contador++;
			        
				}
				 else
				 {
					 while(contador2<particiones)
					 {
			
						 if(particiones%divisor1==0.0)
						{							
							 p1=particiones/divisor1;
							 p2=divisor1;
							 P1[contador]=p1;
							 P2[contador]=p2;
							 contador++; 
							divisor1--;
						}
							else
							{
								divisor1--;	
							}			
					 contador2++;
					}
			
					min1=(int) sqrt((double)pow( P1[0] - P2[0],2));
					marca=0; 		 
			
					for(int i=1;i<contador;i++)
					{
						min2 =(int) sqrt( (double) pow(P1[i] - P2[i],2));
						if(min1>min2)  
						{
							min1=min2;
							marca = i;
						}  
						  
					}	  
			
					  
			
				 }
			 }
			 else	 
	         {
				 marca=0;
				 P1[marca]=1;
				 P2[marca]=1;
			 }

 Set_FilaGrilla(P2[marca]);
 Set_ColumnaGrilla(P1[marca]);	
	
}   
   
  
/// Carga la informacion de la imagen binaria del dominio a calcular densidad 
void Geometria2D::Carga_Imagen_Binaria(char *arch)
{
unsigned long Puntos,contador_Puntos;
char linea[10000];
char* cpToken;
char strTemp[1024];
Archivo *Archivo_Binario=new Archivo();
Archivo_Binario->SetNombre(arch);
Archivo_Binario->SetModo(LEE_ARCHIVO);	
Archivo_Binario->Abrir();
// Cuenta el numero de puntos que estan en el archivo de entrada
Puntos=Archivo_Binario->Numero_Lineas();
GrillaDominio=new Matriz_Entera(Puntos,Puntos);
contador_Puntos=0;
unsigned long i;
while (fscanf (Archivo_Binario->GetArchivo(), "%s", linea) != EOF)
		{ i=0;

			cpToken = strtok (linea, ",");
			strcpy (strTemp, cpToken);
			GrillaDominio->Asigna(contador_Puntos,i,atoi(strTemp));
            for( i=1;i<Puntos;i++)
				{
			cpToken = strtok (NULL, ",");
			strcpy (strTemp, cpToken);
			GrillaDominio->Asigna(contador_Puntos,i,atoi(strTemp));		
					}	
		    contador_Puntos++;
		}
Archivo_Binario->Cerrar();	

}
 
/// Carga la informacion de puntos donde se desea obtener densidad 
int Geometria2D::Carga_Puntos_Archivo(char *arch)
{
unsigned long Puntos,contador_Puntos;
char lineaTextoNodos[1024];
char *nSep1;
char strTempX[1024];
char strTempY[1024];
Archivo *Puntos_Archivo=new Archivo();
Puntos_Archivo->SetNombre(arch);
Puntos_Archivo->SetModo(LEE_ARCHIVO);	
Puntos_Archivo->Abrir();
// Cuenta el numero de nodos que estan en el archivo de entrada
Puntos=Puntos_Archivo->Numero_Lineas();
PuntosGrilla=new Nodo_2D[Puntos];
	
	if(Puntos_Archivo->GetArchivo()!=NULL)
	{
		contador_Puntos=0;
		while(fgets(lineaTextoNodos, 1024, Puntos_Archivo->GetArchivo())!=NULL) 	
		{
			
			lineaTextoNodos[strlen(lineaTextoNodos)-1] = '\0';
	        if ((nSep1 = strchr(lineaTextoNodos, ',')) != NULL) 
			{
	            strncpy(strTempX, lineaTextoNodos, nSep1-lineaTextoNodos);
				// Le asigna un identifcador al nodo
				PuntosGrilla[contador_Puntos].Set_Id_Nodo(contador_Puntos);
                // Almacena posicion x				
				PuntosGrilla[contador_Puntos].Set_X(atof(strTempX));	
				strcpy(strTempY,nSep1+1);
				// Almacena posicion y
				PuntosGrilla[contador_Puntos].Set_Y(atof(strTempY));
	   		}
	     contador_Puntos++;
		}
	}
Puntos_Archivo->Cerrar();	
return contador_Puntos;
}

/// Genera una grilla de puntos a partir de informacion particionada de la grilla y almacena los centros geometricos de la grilla  
void Geometria2D::Generar_Grilla_Vectores(Vector *dominio,Vector_Entero *Resolucion,Matriz_Entera *Dim_Fila_Columna,Matriz_Entera *Bandera_Fila_Columna,Vector *gX,Vector *gY,Vector *CentroGeometrico,int Proceso)
{
double dx,dy,X,Y,minx,miny,centroideX,centroideY,minCX,minCY,maxCX,maxCY;	
int contador_Puntos=0;	
dx = (dominio->Retorna(2) - dominio->Retorna(0) ) / (double)(Resolucion->Retorna(0) -1);
dy = (dominio->Retorna(3) - dominio->Retorna(1) ) / (double)(Resolucion->Retorna(1) -1) ;	
miny = dominio->Retorna(1);	
minx = dominio->Retorna(0);	
 
unsigned long long dCol =Dim_Fila_Columna->Retorna(1,0);
unsigned long long dFil =Dim_Fila_Columna->Retorna(1,0);

			for (int i=0; i< (int) Dim_Fila_Columna->Retorna(0,Proceso); i++)
			{  
				for (int j=0; j<(int) Dim_Fila_Columna->Retorna(1,Proceso); j++)
				{
				
				X = minx + dx*(  (double) ( j + dCol* Bandera_Fila_Columna->Retorna(1,Proceso)));
				Y = miny + dy*(  (double) ( i + dFil  * Bandera_Fila_Columna->Retorna(0,Proceso)));
				
							// Almacena posicion x	
                            gX->Asigna(contador_Puntos,X);					
							// Almacena posicion y
						    gY->Asigna(contador_Puntos,Y); 
					
					if(contador_Puntos==0)
					{
					minCX = X;	
					minCY = Y;	
					}
					
				contador_Puntos++;
									
				}
			}
			
			maxCX = X;
			maxCY = Y;

centroideX =  ( maxCX + minCX )  / 2.0;
centroideY =  ( maxCY + minCY )  / 2.0;
	
CentroGeometrico->Asigna(0,centroideX);
CentroGeometrico->Asigna(1,centroideY);
			
}   
  
/// Genera una grilla para un procesador
void Geometria2D::Generar_Grilla_Memoria(Vector *dominio,Vector_Entero *Resolucion)
{
double dx,dy,X,Y,minx,maxy;	
Set_MaxXY(dominio->Retorna(2),dominio->Retorna(3));	  
Set_MinXY(dominio->Retorna(0),dominio->Retorna(1));	  
Set_ResolucionX((int)Resolucion->Retorna(0));
Set_ResolucionY((int)Resolucion->Retorna(1));
Set_DistanciaX(Get_MaxX() - Get_MinX());
Set_DistanciaY(Get_MaxY() - Get_MinY());
maxy=Get_MaxY();
minx=Get_MinX();	
dx  = Get_DistanciaX()/(double)(Get_ResolucionX()-1);  /* cm */
dy  = Get_DistanciaY()/(double)(Get_ResolucionY()-1);  /* cm */
Set_NumeroPuntosGrilla(Get_ResolucionY()*Get_ResolucionX());
PuntosGrilla=new Nodo_2D[Get_NumeroPuntosGrilla()];
int contador_Puntos=0;

for (int i=0; i<Get_ResolucionY(); i++)
{  
	//printf("\n ");
	for (int j=0; j<Get_ResolucionX(); j++)
	{
  	X = minx+j*dx;
    Y = maxy-i*dy;
				PuntosGrilla[contador_Puntos].Set_Id_Nodo(contador_Puntos);
                // Almacena posicion x				
				PuntosGrilla[contador_Puntos].Set_X(X);	
				// Almacena posicion y
				PuntosGrilla[contador_Puntos].Set_Y(Y);
	
		contador_Puntos++;
	}
}
printf("Numero de Puntos =%d",contador_Puntos);


}
