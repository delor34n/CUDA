#include "Geometria2D_Triangulos.hpp"
/// Contante de velocidad de la luz, 1.0 en caso de no utilizar esta constante en el calculo de integrales
#define vLUZ 1.0  // SI velocidad del medio no se toma en consideracion en el modelo su valor es 1
//#define ESCALA 100.0 // Se define la escala de la dimension a utilizar 1 - Metro, 100 - CM, 1000 - mm, por defecto el calculo es en Metro
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
using namespace std;
 
/// Lamada a generar una grilla y almacenar las coordenadas en vectores	
void Geometria2D_Triangulos::Generar_Puntos_Grilla_FEM(Vector *dominio,Vector_Entero *Resolucion,Matriz_Entera *Dim_Fila_Columna,Matriz_Entera *Bandera_Fila_Columna,Vector *gX,Vector *gY,Vector *CentroGeometrico,int Proceso)
{
	Generar_Grilla_Vectores(dominio,Resolucion,Dim_Fila_Columna,Bandera_Fila_Columna,gX,gY,CentroGeometrico,Proceso);
}	
/// Calcula densidades a partir de una grilla de puntos - Esta funcion es la utilizada en los procesos paralelos, por eso se utiliza notacion vectorial y de punteros, ademas es mas facil de manipular en el envio y recepcion de mensajes
void Geometria2D_Triangulos::Calcula_Densidades_Puntos_Grilla(int procesador, Vector_Entero *pCentroides,unsigned long long nElementos,Vector *DENSIDADES,double *Datos_Elementos_Paralelo,double *px,double *py,double *nx,double *ny,unsigned long long *puntos_internos)  //NODO2D GRILLA ES PARA Q EVALUE SOLO LA PARTICION PARALELA
{
double areaTMP;
double aitmp,bitmp,citmp;
unsigned long long numero_puntos;
unsigned long long nNodal1,nNodal2,nNodal3;
double Fi;
double fNodalTMP;
double x1T, x2T, x3T, y1T, y2T, y3T;
int bandera_interior=0;	
int cont_calculo;
cont_calculo=0;	
numero_puntos=DENSIDADES->Columnas();
	
	for(unsigned long long i=0;i<numero_puntos;i++)
	{
        	fNodalTMP=0.0;
     		
                for(unsigned long long j=0;j<nElementos;j++)		
          		{
					
					nNodal1 =  (unsigned long) Datos_Elementos_Paralelo[j*17 +1] ;
					nNodal2 =  (unsigned long) Datos_Elementos_Paralelo[j*17 +2] ;
					nNodal3 =  (unsigned long) Datos_Elementos_Paralelo[j*17 +3] ;
					
					
					if( (int) pCentroides->Retorna(nNodal1)  ==   procesador  ||  (int) pCentroides->Retorna(nNodal2)  ==   procesador  || (int) pCentroides->Retorna(nNodal3)  ==    procesador  )
                   {
										
								x1T=nx[ nNodal1 ];  //   Cordenadas del elemento finito triangulare
								x2T=nx[ nNodal2 ];  //    (x1T,y1T) = Nodo 1 del elemento j
								x3T=nx[ nNodal3 ];  //    (x2T,y2T) = Nodo 2 del elemento j
								y1T=ny[ nNodal1 ];  //    (x3T,y3T) = Nodo 3 del elemento j
								y2T=ny[ nNodal2 ];  //
								y3T=ny[ nNodal3 ];  //     almacenado consecutivamente en vector --> [ - indice elemento j - , { nodo 1, nodo 2 , nodo 3 } , -Area elemento-, --- ]
								
								if(Punto_Interior_Triangulo_Coordenado( x1T, x2T, x3T, y1T, y2T, y3T,px[i],py[i]) == 1)
							   {
								 cont_calculo++;  
								 areaTMP=Datos_Elementos_Paralelo[j*17 +4];  // Area del elemento j, almacenado en vector --> [- nodo 3 -, { Area Elemento } , - F1 -, ---] 
								
								for(int k=0;k<3;k++)   
								{
									 Fi = Datos_Elementos_Paralelo[j*17 + 5 + k]; // Densidad Fi en el nodo K del elemento j, --> Almacenado en vector [-Area Elemento-, { F1, F2, F3 }, -a1-]
						
									 aitmp = Datos_Elementos_Paralelo[j*17 +8+3*k];         // I	a1 b1 c1	I  <--- Matriz de coeficientes original ** Almacenamiento en Vector --->  [- Area Elemento -, { a1, b1, c1, a2, b2, c2, a3, b3, c3},  I
									 bitmp = Datos_Elementos_Paralelo[j*17 +9+3*k];         // I	a2 b2 c2	I
									 citmp = Datos_Elementos_Paralelo[j*17 +10+3*k];       // I	a3 b3 c3	I
									 
									 fNodalTMP=fNodalTMP+Fi*Get_Funcion_Nodal(areaTMP,aitmp,bitmp,citmp,px[i],py[i]);    //  Sumatoria de  Fi * Funcion_Nodal i en (X(i),Y(i)) del elemento j
								}
								DENSIDADES->Asigna(i,fNodalTMP); // Se almacena la densidad obtenida en el punto i de la grilla  
								bandera_interior=1;
								break;
							   }
						 }	   
					else
                    {
					
					bandera_interior=1;	
					continue;

					}						
							   
					   
				}	  
				
				if(bandera_interior==0) // Si no estaba dentro del dominio su valor es 0.0
				   {
					   DENSIDADES->Asigna(i,0.0);
					}
				else
				{
					bandera_interior=0;
				}	
	}

	puntos_internos[0]=cont_calculo;

}

/// Calcula densidades a partir de una grilla de puntos en notacion utilizando objetos Nodo_2D, solo para calculo secuencial
void Geometria2D_Triangulos::Calcula_Densidades_Puntos(Vector *GRILLA,unsigned long long *Puntos_Dominio)
{
int bandera_interior=0;	
double fNodalTMP;	
Nodo_2D nodoTMP;
unsigned long long tmp;
unsigned long long cont_calculo=0;	
for(unsigned long long i=0;i<GRILLA->Columnas();i++)
{
	fNodalTMP=0.0;

				for(unsigned long long j=0;j<Numero_Elementos;j++)		
          		{
								
			/*if(GeometriaTrabajo->Get_Grilla()->Retorna(I,J)==1)	  // 1.-  Esta condicion es utlizada para solo evaluar los puntos que pertenecen al dominio 
					    {                                                                            // formados por los elementos triangulares contenidos en la grilla 
					                                                                                  // 2.- Esta desactivada esta opcion 
							fprintf(traza, "0.0\n");                                         // 3.- El archivo de entrada es un archivo binario que tiene como valores 0 (color negro)
							bandera_interior=1;                                          // y valores 1 (color blanco), dond eel blanco significa que pertenece el punto al dominio FEM
							break;                                                                // en futuros trabajos se pueden utilizar distintos valores segun los colores de entrada
						}                                                                            // en el cual el proceso de discretizacion esta asociado a tecnicas de analisis de imagenes
				*/
	
					if(Punto_Interior_Triangulo(j,  Retornar_Punto_Grilla(i) )==1)
				   {
					 cont_calculo++;  
						for(int k=0;k<3;k++)   
						{
							tmp=Elementos_Geometria[j].Get_Valor_Nodo(k);
							nodoTMP=Nodos_Geometria[tmp];
							fNodalTMP=fNodalTMP+nodoTMP.Get_Densidad()*Funcion_Nodal(j,k,Retornar_Punto_Grilla(i));	
						}

					GRILLA->Asigna( i  ,  fNodalTMP );
					bandera_interior=1;
                    break;
				   }
				  
				   
				}	  
				
				if(bandera_interior==0)
			   {GRILLA->Asigna(i,0.0);}
				else
				{bandera_interior=0;}	
						
	}
	
	Puntos_Dominio[0]=cont_calculo;
	
}


/// Carga la informacion de de los nodos desde un archivo
void Geometria2D_Triangulos::Carga_Nodos(char *arch)
{
unsigned long long nNodos,contador_nodo;

char lineaTextoNodos[1024];
//char *lineaTextoNodos;
char *nSep1;
char strTempX[1024];
char strTempY[1024];

Archivo *Nodos_Archivo=new Archivo();
Nodos_Archivo->SetNombre(arch);
Nodos_Archivo->SetModo(LEE_ARCHIVO);	
Nodos_Archivo->Abrir();
	
// Cuenta el numero de nodos que estan en el archivo de entrada
nNodos=Nodos_Archivo->Numero_Lineas();
// Asigna el numero de nodos de la geometria
Asigna_Numero_de_Nodos(nNodos);
// Reserva memoria y crea el vector de objetos Nodos	
Set_Reserva_Memoria_Nodos(nNodos);	
	
contador_nodo=0;

	if(Nodos_Archivo->GetArchivo()!=NULL)
	{
		contador_nodo=0;
		while(fgets(lineaTextoNodos, 1024, Nodos_Archivo->GetArchivo())!=NULL) 	
		{
			
			lineaTextoNodos[strlen(lineaTextoNodos)-1] = '\0';
	        if ((nSep1 = strchr(lineaTextoNodos, ',')) != NULL) 
			{
	            strncpy(strTempX, lineaTextoNodos, nSep1-lineaTextoNodos);
				// Le asigna un identifcador al nodo
				Nodos_Geometria[contador_nodo].Set_Id_Nodo(contador_nodo);
                // Almacena posicion x				
				Nodos_Geometria[contador_nodo].Set_X(atof(strTempX));	
				strcpy(strTempY,nSep1+1);
				// Almacena posicion y
				Nodos_Geometria[contador_nodo].Set_Y(atof(strTempY));
                // Inicializa el nodo como no perteneciente al borde				
				Nodos_Geometria[contador_nodo].Set_pBorde(0);	
	   		}
	     contador_nodo++;
		}
	}
	

}

/// Carga la informacion de los elementos desde un archivo
void Geometria2D_Triangulos::Carga_Elementos(char *arch)
{
char strTempP1[1024];
char strTempP2[1024];
char strTempP3[1024];
char strTempP4[1024];
unsigned long long contador_elemento=0;
char cadena[150];
char* cpToken;
unsigned long long nElementos;	
Archivo *Elementos_Archivo=new Archivo();
Elementos_Archivo->SetNombre(arch);
Elementos_Archivo->SetModo(LEE_ARCHIVO);	
Elementos_Archivo->Abrir();	
nElementos=Elementos_Archivo->Numero_Lineas();
Asigna_Numero_de_Nodos_en_Elementos(3);	
Asigna_Numero_de_Elementos(nElementos);	
Set_Reserva_Memoria_Elementos(nElementos);

		while (fscanf (Elementos_Archivo->GetArchivo(), "%s", cadena) != EOF)
		{
			// Se fija el numero de nodos por elemento
			Elementos_Geometria[contador_elemento].Set_Numero_Nodos_Elemento(3);
			
			// Se reserva memoria para almacenar un arreglo de 3 objetos nodos en el elemento 
			Elementos_Geometria[contador_elemento].Set_Reserva_Memoria_Nodos_Triangulo(Elementos_Geometria->Get_Numero_Nodos_Elemento());
            
			// Extraccion del string para el primer nodo
			cpToken = strtok (cadena, ",");
			strcpy (strTempP1, cpToken);
			
			// Extraccion del string para el segundo nodo
			cpToken = strtok (NULL, ",");
			strcpy (strTempP2, cpToken);
			
			// Extraccion del string para el tercer nodo
			cpToken = strtok (NULL, ",");
			strcpy (strTempP3, cpToken);
			
			// Extraccion del string para obtener el dominio al cual pertenece el elemento
			cpToken = strtok (NULL, ",");
			strcpy (strTempP4, cpToken);
		    
			// Se almacenan los nodos y el dominio 
			Elementos_Geometria[contador_elemento].Set_Nodo_Elemento(0,atoi(strTempP1)-1);
			Elementos_Geometria[contador_elemento].Set_Nodo_Elemento(1,atoi(strTempP2)-1);
			Elementos_Geometria[contador_elemento].Set_Nodo_Elemento(2,atoi(strTempP3)-1);
			Elementos_Geometria[contador_elemento].Set_Dominio_Elemento(atoi(strTempP4)-1);
			
			// Se define una matriz de 3X3 local de pertencia a nodos globales la cual es usada en el proceso de aporte nodal a matrices globales
			Generar_Matriz_Pertenencia_Elemento(contador_elemento);
		    
			//Se cargan inicialmente como elementos sin fuente en su interior, mas adelante se asigna la pertenencia
			Elementos_Geometria[contador_elemento].Set_Elemento_Fuente(0);
			
		contador_elemento++;
		}
	Elementos_Archivo->Cerrar();
  
}

/// Carga la informacion de las fronteras de los dominios desde un archivo
void Geometria2D_Triangulos::Carga_Fronteras(char *arch)
{
char nodo1Tmp[1024];
char nodo2Tmp[1024];
char strTempP3[1024];
char strTempP4[1024];
char cuadranteTmp[1024];
char dominioTmp[1024];
char dominioInterno[1024];	
unsigned long long contador_fronteras=0;

char cadena[150];
char* cpToken;
unsigned long long n1,n2;
unsigned long long nCaras;	
int peso;
Archivo *Fronteras_Archivo=new Archivo();
Fronteras_Archivo->SetNombre(arch);
Fronteras_Archivo->SetModo(LEE_ARCHIVO);	
Fronteras_Archivo->Abrir();	
nCaras=Fronteras_Archivo->Numero_Lineas();
Elemento_Frontera *Fronteras_GeometriaTMP;
Vector_Entero *Nodos_Frontera_GeometriaTMP;	
Fronteras_GeometriaTMP=new Elemento_Frontera[nCaras];
Nodos_Frontera_GeometriaTMP=new Vector_Entero(nCaras);

		while (fscanf (Fronteras_Archivo->GetArchivo(), "%s", cadena) != EOF)
		{
				
			cpToken = strtok (cadena, ","); 
			strcpy (nodo1Tmp, cpToken); //Nodo 1
			n1=atoi(nodo1Tmp)-1;
			
			cpToken = strtok (NULL, ",");
			strcpy (nodo2Tmp, cpToken); //Nodo 2
			n2=atoi(nodo2Tmp)-1;
			
			cpToken = strtok (NULL, ",");
			strcpy (strTempP3, cpToken);//No  se usa, pero queda preparado en caso de necesitarlo
			
			cpToken = strtok (NULL, ",");
			strcpy (strTempP4, cpToken);//No  se usa, pero queda preparado en caso de necesitarlo
			
			cpToken = strtok (NULL, ",");//Cuadrante
			strcpy (cuadranteTmp, cpToken);
			
			cpToken = strtok (NULL, ",");//Dominio 
			strcpy (dominioInterno, cpToken);
			
			cpToken = strtok (NULL, ",");//Dominio Nuevo de prueba , al parecer este es el correcto
			strcpy (dominioTmp, cpToken);
			
			if((atoi(dominioTmp)) == 0)  //Si pertence a la frontera global se almacena la informacion
			{	
			Fronteras_GeometriaTMP[contador_fronteras].Set_Reserva_Memoria_Nodos_Frontera(2);
		   	Nodos_Frontera_GeometriaTMP->Asigna(contador_fronteras,n1);
			Fronteras_GeometriaTMP[contador_fronteras].Set_Nodo_Frontera(0,n1);
			Fronteras_GeometriaTMP[contador_fronteras].Set_Nodo_Frontera(1,n2);
			Nodos_Geometria[n1].Set_pBorde(1);	
			Nodos_Geometria[n2].Set_pBorde(1);	
			Fronteras_GeometriaTMP[contador_fronteras].Set_Dominio_Frontera(atoi(dominioInterno)-1);
	        contador_fronteras++;

			}	
		}
		
Set_Reserva_Memoria_Fronteras(contador_fronteras);

	// Proceso de calculo necesario para la integral de los elementos fronteras	 
   for(unsigned long long i=0;i<contador_fronteras;i++)
   { 
		  Fronteras_Geometria[i].Set_Reserva_Memoria_Nodos_Frontera(2);
		  Nodos_Frontera_Geometria->Asigna(i,Nodos_Frontera_GeometriaTMP->Retorna(i));
		  Fronteras_Geometria[i].Set_Nodo_Frontera(0,Fronteras_GeometriaTMP[i].Get_Valor_Nodo_Frontera(0));
		  Fronteras_Geometria[i].Set_Nodo_Frontera(1,Fronteras_GeometriaTMP[i].Get_Valor_Nodo_Frontera(1));
		  Fronteras_Geometria[i].Set_Dominio_Frontera(Fronteras_GeometriaTMP[i].Get_Dominio_Frontera());
		  Fronteras_Geometria[i].Set_Norma(Norma_Dos_Nodos(Fronteras_GeometriaTMP[i].Get_Valor_Nodo_Frontera(0),Fronteras_GeometriaTMP[i].Get_Valor_Nodo_Frontera(1)));
		  Fronteras_Geometria[i].Set_Reserva_Memoria_MIB();
		  Fronteras_Geometria[i].Set_Vector_Pertenencia_Borde(0,Fronteras_GeometriaTMP[i].Get_Valor_Nodo_Frontera(0));
		  Fronteras_Geometria[i].Set_Vector_Pertenencia_Borde(1,Fronteras_GeometriaTMP[i].Get_Valor_Nodo_Frontera(1));

	   // Calculo de matriz frontera del elemento	   
			  for(int j=0;j<2;j++)
			  {	  
			 		 for(int k=0;k<2;k++)
					  {
						  if(j==k)
                           peso=1;
							  else
							 peso=0;	  
									 Fronteras_Geometria[i].Set_Matriz_Borde(j,k, Fronteras_Geometria[i].Calculo_MBorde_ML(2.0/PI,Fronteras_Geometria[i].Get_Norma(),peso) );	   
					  }
		     }			  
  }
   

    Asigna_Numero_de_Fronteras(contador_fronteras);		
	Set_Numero_Nodos_Frontera_Dominio_Global(contador_fronteras);
	Fronteras_Archivo->Cerrar();	 // cierra el archivo
  

}


/// Carga solucion Nodal - Desde Vector 
void Geometria2D_Triangulos::Carga_Densidad_Nodal_Vector(Vector *X)
{
unsigned long long nNodos,contador_nodo;
nNodos=X->Columnas();

		contador_nodo=0;
		while(contador_nodo<nNodos) 	
		{
		Nodos_Geometria[contador_nodo].Set_vNodal(X->Retorna(contador_nodo));	
		contador_nodo++;
		}	
		
}

/// Carga solucion Nodal desde archivo
void Geometria2D_Triangulos::Carga_Densidad_Nodal_Archivo(char *arch)
{
unsigned long long nNodos,contador_nodo;
char lineaTextoNodos[1024];
char strTempX[1024];
Archivo *Densidad_Nodal=new Archivo();
Densidad_Nodal->SetNombre(arch);
Densidad_Nodal->SetModo(LEE_ARCHIVO);	
Densidad_Nodal->Abrir();
nNodos=Densidad_Nodal->Numero_Lineas();

	if(Densidad_Nodal->GetArchivo()!=NULL)
	{
		contador_nodo=0;
		while(fgets(lineaTextoNodos, 1024, Densidad_Nodal->GetArchivo())!=NULL) 	
		{
			
			lineaTextoNodos[strlen(lineaTextoNodos)-1] = '\0';
	            strcpy(strTempX, lineaTextoNodos);
				Nodos_Geometria[contador_nodo].Set_vNodal(atof(strTempX));	
	     contador_nodo++;
		}
	}
Densidad_Nodal->Cerrar();	

	
}

/// Busca el valor minimo nodal - puede ser usado para normalizar graficas
double Geometria2D_Triangulos::Buscar_Mininos_Nodos(unsigned long long id,int eje) //eje =0 es X / eje =1 es Y 
{
double minimo;
	if(eje==0)
	{
	minimo=Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(0)].Get_X();
	for(int i=1;i<3;i++)	
	  if(Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(i)].Get_X()<minimo)	
	  minimo=Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(i)].Get_X();
	}
		else
		{
		minimo=Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(0)].Get_Y();
		for(int i=1;i<3;i++)	
		  if(Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(i)].Get_Y()<minimo)	
		  minimo=Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(i)].Get_Y();
		}	
	
return minimo;
}

/// Obtiene y muestra las coordenadas locales del elemento id
void Geometria2D_Triangulos::Obtener_Coordenadas_Locales_Elemento(unsigned long long id)
{
double minimoX,minimoY,xCordTMP,yCordTMP;
minimoX=Buscar_Mininos_Nodos(id,0);
minimoY=Buscar_Mininos_Nodos(id,1);
printf("Las coordenadas minimas del elemento %Ld son: (%f,%f) \n",id,minimoX,minimoY);
Elementos_Geometria[id].Set_Reserva_Memoria_Coordenadas_locales(3);

	printf("Las coordenadas locales del elemento %Ld son: ",id);
	for(int i=0;i<3;i++)
	{
		printf("  X:    %f     ",Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(i)].Get_X());
		printf("  Y:    %f     ",Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(i)].Get_Y());
		xCordTMP=Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(i)].Get_X() - minimoX;
		yCordTMP=Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(i)].Get_Y() - minimoY;
		Elementos_Geometria[id]. Set_Coordenada_Local_Elemento(i,xCordTMP,yCordTMP);
		printf("(%f,%f)",Elementos_Geometria[id].Get_Coordenada_Local(i).Get_X(),Elementos_Geometria[id].Get_Coordenada_Local(i).Get_Y());
	}	

}

/// Retorna la funcion nodal de un elemento finito triangular de e nodos dado: Area, coeficientes ai,bi y ci, ademas de un punto x,y
double Geometria2D_Triangulos::Get_Funcion_Nodal(double areaE,double Ai,double Bi,double Ci,double pX,double pY)
{
	double funcion_Nodal;
	funcion_Nodal=(1.0/(2.0*areaE))*( Ai + Bi*pX + Ci*pY);
	return funcion_Nodal;
}

/// Retorna la funciona nodal de un elemento finito triangular id, indicando el nodo y un punto cualquiera
double Geometria2D_Triangulos::Funcion_Nodal(unsigned long long id,int indice,Nodo_2D Punto) 
{
double areaE,Ai,Bi,Ci,pX,pY,fNodal;
areaE=Elementos_Geometria[id].Get_Area();	
Ai=Elementos_Geometria[id].Get_Coeficientes_Permutacion()->Retorna(indice,ai);
Bi=Elementos_Geometria[id].Get_Coeficientes_Permutacion()->Retorna(indice,bi);	
Ci=Elementos_Geometria[id].Get_Coeficientes_Permutacion()->Retorna(indice,ci);
pX=Punto.Get_X();
pY=Punto.Get_Y();
fNodal=Get_Funcion_Nodal(areaE,Ai,Bi,Ci,pX,pY);	
return fNodal;
}

/// Obtiene el conjunto de elementos que contienen a un nodo en comun
void Geometria2D_Triangulos::Conjunto_Elementos_Vecinos_a_Nodo(unsigned long long idNodo)
{
	Vector_Entero *VecindadTmp=new Vector_Entero(100);
	unsigned long long cont=0;
	    for(unsigned long long i=0;i<Numero_Elementos;i++)
		{
			if(Elementos_Geometria[i].Get_Valor_Nodo(0)==idNodo||Elementos_Geometria[i].Get_Valor_Nodo(1)==idNodo||Elementos_Geometria[i].Get_Valor_Nodo(2)==idNodo)  
			{
					VecindadTmp->Asigna(cont,i);
					cont++; 			
			}
		}
		
	Nodos_Geometria[idNodo].Set_Orden_Vecindad(cont);
		
	for(unsigned long long i=0;i<cont;i++)
    {Nodos_Geometria[idNodo].Set_Vecindad(i,VecindadTmp->Retorna(i));}
	
	delete VecindadTmp;
}

/// Retorna orientacion de un elemento finito triangular
 int  Geometria2D_Triangulos::Orientacion_Elemento(unsigned long long id)
{
int orientacion;
orientacion=Orientacion_Triangulo(Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(0)],Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(1)],Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(2)]);
return orientacion;
}

/// Llamada a funcion que retorna la orientacion de un triangulo utilizando nodos
int Geometria2D_Triangulos::Orientacion_Triangulo(Nodo_2D A1,Nodo_2D A2,Nodo_2D A3)
{
int salida;
salida=Orientacion_Triangulo_Coordenado(A1.Get_X(),A2.Get_X(),A3.Get_X(),A1.Get_Y(),A2.Get_Y(),A3.Get_Y());
return salida;	
}

/// Retorna la orientacion de un triangulo, 1 - orientacion positiva (sentido horario) y 0 - orientacion negativa (sentido anti-horario) teniendo como entrada las coordenadas de los tres puntos de los vertices
int Geometria2D_Triangulos::Orientacion_Triangulo_Coordenado(double x1,double x2,double x3,double y1,double y2,double y3)
{
double R;

	R=( x1 - x3 )*( y2 - y3 ) - ( y1 - y3 )*( x2 - x3 );	
	if(R>=0)
		return 1;//Orientacion Positiva
	else
		return 0;	//Orientacion Negativa
} 
  
/// Verifica si un punto pertence al interior de un triangulo dadas las tres coordenadas de un triangulo y el punto a evaluar
int Geometria2D_Triangulos::Punto_Interior_Triangulo_Coordenado(double x1T,double x2T,double x3T,double y1T,double y2T,double y3T,double px,double py)
{
	int salida;
	int suma_verificadora=0;
	salida=Orientacion_Triangulo_Coordenado( x1T, x2T , px , y1T , y2T , py );
	suma_verificadora=suma_verificadora+salida;
	salida=Orientacion_Triangulo_Coordenado( x2T, x3T , px , y2T , y3T , py );
	suma_verificadora=suma_verificadora+salida;
	salida=Orientacion_Triangulo_Coordenado( x3T, x1T , px , y3T , y1T , py );
    suma_verificadora=suma_verificadora+salida;
   // Si el punto esta dentro del triangulo	
   if(suma_verificadora==3||suma_verificadora==0)
   {salida=1;}
   // sino el punto esta afuera del triangulo
   else
   {salida=0;}
   
   return salida;
}

/// Verifica si un punto del tipo Nodo_2D se encuentra en el interior de un elemento finito triangular "id"	
int Geometria2D_Triangulos::Punto_Interior_Triangulo(unsigned long long id,Nodo_2D Punto)
{
	int salida;
	int suma_verificadora=0;
	//Nodo_2D punto;
	double xCord,yCord;
	//punto.Set_X(1.0);
	xCord=Punto.Get_X();
	yCord=Punto.Get_Y();

	salida=Orientacion_Triangulo(Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(0)],Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(1)],Punto);
	suma_verificadora=suma_verificadora+salida;
	salida=Orientacion_Triangulo(Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(1)],Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(2)],Punto);
	suma_verificadora=suma_verificadora+salida;
	salida=Orientacion_Triangulo(Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(2)],Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(0)],Punto);
	suma_verificadora=suma_verificadora+salida;
   // Si el punto esta dentro del triangulo
   if(suma_verificadora==3||suma_verificadora==0)
   {salida=1;}
   // sino el punto esta afuera del triangulo
   else
   {salida=0;}
   return salida;
}

/// Genera los vectores de carga de todos los elementos
void Geometria2D_Triangulos::Generar_Vectores_de_Cargas()
{
	for(unsigned long long i=0;i<Numero_Elementos;i++)
	{
	Elementos_Geometria[i].Set_Reserva_Memoria_Vector_Carga();
	Generar_Vector_Carga_Elemento(i);
	}
}

/// Genera la matriz de rigidez de un elemento finito trinagular de 3 nodos  
void Geometria2D_Triangulos::Generar_Matriz_Rigidez_Elemento(unsigned long long id)
{  double Kij,Bi,Bj,Ci,Cj,AreaE;
	double cConstante; //Constante de scattering
	Elementos_Geometria[id].Set_Reserva_Memoria_Matriz_Rigidez_Elemento();
	AreaE=Elementos_Geometria[id].Get_Area();
    cConstante=Elementos_Geometria[id].Get_kVariable();
  
	for(int i=0;i<3;i++)
	{	
		for(int j=0;j<3;j++)	
		{	
		Bi=Elementos_Geometria[id].Get_Coeficientes_Permutacion()->Retorna(i,bi);
		Bj=Elementos_Geometria[id].Get_Coeficientes_Permutacion()->Retorna(j,bi);	
		Ci=Elementos_Geometria[id].Get_Coeficientes_Permutacion()->Retorna(i,ci);
		Cj=Elementos_Geometria[id].Get_Coeficientes_Permutacion()->Retorna(j,ci);
        Kij=vLUZ*cConstante*(Bi*Bj + Ci*Cj)/(4.0*AreaE);		
		Elementos_Geometria[id].Set_Matriz_Rigidez_Elemento(i,j,Kij);	
		}
	}
	

}

/// Genera la matriz de masa de un elemento finito triangular de tres nodos
void Geometria2D_Triangulos::Generar_Matriz_Masa_Elemento(unsigned long long id)
{  double Cij,AreaE;
	double mConstante; // Constante de absorcion
	double Peso_Nodal;
	Elementos_Geometria[id].Set_Reserva_Memoria_Matriz_Masa_Elemento();
	AreaE=Elementos_Geometria[id].Get_Area();
    mConstante=Elementos_Geometria[id].Get_mVariable();
	for(int i=0;i<3;i++)
	{	
		for(int j=0;j<3;j++)
		{	
		if(i==j)
        Peso_Nodal=1.0;
        else
        Peso_Nodal=0.0;
        Cij=vLUZ*mConstante*AreaE*(1.0+Peso_Nodal)/12.0;	
		Elementos_Geometria[id].Set_Matriz_Masa_Elemento(i,j,Cij);	
		}

	}


}

/// Calculo del vector gradiente de un elemento id en un nodo 
void Geometria2D_Triangulos::Vector_Gradiente_Elemento(unsigned long long id,unsigned long long nodo)
{
double AreaTMP,Bi,Ci;
AreaTMP=Elementos_Geometria[id].Get_Area();
Bi=Elementos_Geometria[id].Get_Coeficientes_Permutacion()->Retorna(nodo,bi);	
Ci=Elementos_Geometria[id].Get_Coeficientes_Permutacion()->Retorna(nodo,ci);
Elementos_Geometria[id].Set_Vector_Gradiente(Bi/(2*AreaTMP),Ci/(2*AreaTMP));	
}

/// Distancia de un nodo a un punto
double Geometria2D_Triangulos::Distancia_Nodo_Punto(unsigned long long idNodo,double xCord,double yCord)
{
double Distancia;
Distancia=Distancia_Dos_Puntos(Nodos_Geometria[idNodo].Get_X(),xCord,Nodos_Geometria[idNodo].Get_Y(),yCord);
return Distancia;
}

/// Calcula la distancia de dos puntos, dada las coordenadas de los dos puntos
double Geometria2D_Triangulos::Distancia_Dos_Puntos(double xCord1,double xCord2,double yCord1,double yCord2)
{
	double Distancia,dX,dY;
	dX=xCord1-xCord2;
	dY=yCord1-yCord2;
    Distancia=pow( pow(dX,2) + pow(dY,2) ,0.5);   
	return Distancia;
}

/// Distancia del centro de un elemento a un punto
double Geometria2D_Triangulos::Distancia_Centro_Elemento_Punto(double xCord,double yCord,unsigned long long id)
{
	double Distancia,xCentro,yCentro;
	xCentro=Elementos_Geometria[id].Get_Centroide()->Retorna(0);
	yCentro=Elementos_Geometria[id].Get_Centroide()->Retorna(1);
    Distancia=Distancia_Dos_Puntos(xCord,xCentro,yCord,yCentro);
	return Distancia;
}

/// Genera la matriz de pertenencia de un lemento finito triangular de tres nodos
void Geometria2D_Triangulos::Generar_Matriz_Pertenencia_Elemento(unsigned long long id)
{   
	unsigned long long vNodoGlobal;
	Elementos_Geometria[id].Set_Reserva_Memoria_Matriz_Pertenencia_Elemento();
	for(int i=0;i<3;i++)
	{	
		for(int j=0;j<3;j++)	
		{	
	    vNodoGlobal=Elementos_Geometria[id].Get_Valor_Nodo(j);			
		Elementos_Geometria[id].Set_Matriz_Pertenencia_Elemento(i,j,vNodoGlobal);	
		}
		
	}
	
}

/// Norma (distancia euclidea) entre dos nodos de la geometria 
double Geometria2D_Triangulos::Norma_Dos_Nodos(unsigned long long id_nodo1,unsigned long long id_nodo2)
{
	double nNorma,dX,dY;
	dX=Nodos_Geometria[id_nodo1].Get_X()-Nodos_Geometria[id_nodo2].Get_X();
	dY=Nodos_Geometria[id_nodo1].Get_Y()-Nodos_Geometria[id_nodo2].Get_Y();
    nNorma=pow( pow(dX,2) + pow(dY,2) ,0.5);   
	return nNorma;
}

/// Muestra los nodos de la geometria
void Geometria2D_Triangulos::Ver_Nodos(void)
{
   printf("\n");
   for (unsigned long long i = 0; i < Numero_Nodos; i++) 
   Ver_Nodo(i);
}

/// Muestra la informacion de un nodo
void Geometria2D_Triangulos::Ver_Nodo(unsigned long long id)
{
   printf("Nodo(%Ld):(%4.15f,%4.15f)",id,Nodos_Geometria[id].Get_X(),Nodos_Geometria[id].Get_Y());
   printf("\n");
}

/// Muestra asignacion nodal de los elementos finitos triangulares de tres nodos
void Geometria2D_Triangulos::Ver_Elementos(void)
{
   printf("\n");
   for (unsigned long long i = 0; i < Numero_Elementos; i++) {
   Ver_Elemento(i);   }printf("\n");
}

/// Muestra informacion nodal de un elemento finito triangular de tres nodos
void Geometria2D_Triangulos::Ver_Elemento(unsigned long long id)
{
   printf("\n Elemento(%Ld):[",id);
   for (int i = 0; i < Numero_Nodos_Elemento; i++) {
   printf("%Ld",Elementos_Geometria[id].Get_Valor_Nodo(i));
   if(i!=Numero_Nodos_Elemento-1)
   printf(",");	   
   }  printf("]\n");
}

/// Muestra las cordenadas de todos los elementos finitos triangulares de tres nodos 
void Geometria2D_Triangulos::Ver_Coordenadas_Todos_los_Elementos()
{   printf("\n Lista de Elementos \n");
	for(unsigned long long i=0;i<Numero_Elementos;i++)
	Ver_Coordenadas_Elemento(i);
}

/// Muestra las coordenadas de un elemento finito triangular de tres nodos
void Geometria2D_Triangulos::Ver_Coordenadas_Elemento(unsigned long long id)
{ 
   printf("Elemento E(%Ld):[",id);
   for (int i = 0; i < Numero_Nodos_Elemento; i++) 
   printf("(%4.8f,%4.8f)",Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(i)].Get_X() ,Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(i)].Get_Y());
   printf("]\n");
   
}

/// Calcula el centro geometrico de un elemento finito triangular de tres nodos
void Geometria2D_Triangulos::Centroide_Elemento(unsigned long long id)
{
double centroX;
double centroY;
	for(int i=0;i<Numero_Nodos_Elemento;i++)
	{
	centroX= centroX + Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(i)].Get_X();
	centroY= centroY + Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo(i)].Get_Y(); 
	}	
	
Elementos_Geometria[id].Set_Centroide(centroX/3.0,centroY/3.0);
	
}

/// Calcula todos los centros geometricos de todos los elementos finitos triangulares de tres nodos
void Geometria2D_Triangulos::Centroides_Elementos()
{
   printf("\n Lista de Centroides de los Elementos \n");
	for(unsigned long long i=0;i<Numero_Elementos;i++)
	Centroide_Elemento(i);	
}

/// Funcion alternativa para el calculo de funciones nodales de un elemento finito triangular de tres nodos
double Geometria2D_Triangulos::Permutacion_Nodal(Nodo_2D A1,Nodo_2D A2,Nodo_2D A3,int coeficiente,int indice)
{

double *aTmp=new double(3);
double *bTmp=new double(3);
double *cTmp=new double(3);
	

aTmp[0]=A2.Get_X()*A3.Get_Y() - A3.Get_X()*A2.Get_Y();
aTmp[1]=A3.Get_X()*A1.Get_Y() - A1.Get_X()*A3.Get_Y();
aTmp[2]=A1.Get_X()*A2.Get_Y() - A2.Get_X()*A1.Get_Y();
	
bTmp[0]=A2.Get_Y() - A3.Get_Y();
bTmp[1]=A3.Get_Y() - A1.Get_Y();
bTmp[2]=A1.Get_Y() - A2.Get_Y();
	
cTmp[0]=A3.Get_X() - A2.Get_X();	
cTmp[1]=A1.Get_X() - A3.Get_X();
cTmp[2]=A2.Get_X() - A1.Get_X();	
	
	if(coeficiente == ai)
	{
		return aTmp[indice];
	}	
	if(coeficiente == bi)
	{
		return bTmp[indice];
	}	
	if(coeficiente == ci)
	{
		return cTmp[indice];
	}	

	return 0;
}

/// Calculo de permutacion nodal de un elemento finito triangular de tres nodos, donde se obtienen los coeficientes geometricos de las funciones de forma  
void Geometria2D_Triangulos::Permutacion_Elemento(unsigned long long id)
{
int modulo=3;
double tmp;
Elementos_Geometria[id].Set_Reserva_Memoria_Permutacion();	

	for(int i=0;i<3;i++)
	{
	tmp=Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo((i+1)%modulo)].Get_X()*Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo((i+2)%modulo)].Get_Y()	- Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo((i+2)%modulo)].Get_X()*Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo((i+1)%modulo)].Get_Y();
	Elementos_Geometria[id].Set_Permutacion(i,ai,tmp); //a(i)
		
	
	tmp=Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo((i+1)%modulo)].Get_Y()-Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo((i+2)%modulo)].Get_Y();
	Elementos_Geometria[id].Set_Permutacion(i,bi,tmp); //b(i)			
	
		
	tmp=Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo((i+2)%modulo)].Get_X()-Nodos_Geometria[Elementos_Geometria[id].Get_Valor_Nodo((i+1)%modulo)].Get_X();
	Elementos_Geometria[id].Set_Permutacion(i,ci,tmp); //c(i)		
	
	}
	
}

/// Calculo de permutacion de todos los elementos finitos triangulares de tres nodos
void Geometria2D_Triangulos::Permutaciones_Elementos()
{
 	for(unsigned long long i=0;i<Numero_Elementos;i++)
	Permutacion_Elemento(i);
}


/// Genera los vectores de carga del elemento id y con Distribucion de Dirac

void Geometria2D_Triangulos::Generar_Vector_Carga_Elemento(unsigned long long id)
{
	double *fi=new double[3];
	int n;
    int nFuentesElemento=0;
	n=Retorna_Numero_de_Fuentes();
	int *fuentesTMP=new int[n];
    double sumafuente=0.0;
	double da;
	//double r; //En caso de usar fuentes contenidas en esferas
	
	// Almacena valor 1 si el elemento contiene a una o varias fuentes
	for(int i=0;i<n;i++)
	{
	
		if(Punto_Interior_Triangulo(id,Retorna_Nodo_Fuente(i))==1 && Retorna_Nodo_Fuente(i).Get_bandera()==0)
		{
			Nodo_Fuente_Revisado(i);
			fuentesTMP[nFuentesElemento]=i;
			nFuentesElemento++;
			printf(" \n El elemento %Ld tiene contiene la fuente %d con area elemental =%f\n",id,i,Elementos_Geometria[id].Get_Area());
		}
        		
		
	}
	
	for(int i=0;i<nFuentesElemento;i++)
	{
		da=1.0; // Defecto, valores experimentales en distribucion 2d
		
		//r=1.0/((1.0   -  fEsfera )*Elementos_Geometria[id].Get_sVariable());
	    //	da=3.0/(4.0*PI*(pow(r,3.0)) ); // Distribucion de enrgia en todo el volumen de la esfera que contiene la fuente
		//  da=2.0/(PI*( pow(r,2.0 )) ); //Test 1 - solo para pruebas Fluencia
	    //	da=1.0/(4.0*PI*( pow( r,2.0 )) ); //Test 2 - distribucion en la superficie Shell Esfera
		//	da=1.0/(( pow( r,2.0 )) ); //Test 3 - distribucion en la superficie Shell Esfera
				
			for(int j=0;j<3;j++)
			{	
	    		fi[j]=Funcion_Nodal(id,j,Retorna_Nodo_Fuente(  fuentesTMP[i]   ))*Retorna_Nodo_Fuente(  fuentesTMP[i]   ).Get_Densidad()*da + Elementos_Geometria[id].Get_CargasV()->Retorna(j);	
				sumafuente+=fi[j];
			}
	
			Elementos_Geometria[id].Set_Vector_Carga(fi[0],fi[1],fi[2]);
	}	
}

/// Metodo que carga la informacion fisica de un problema de difusion del tipo ETR-AD
void Geometria2D_Triangulos::Carga_Variables_Fisicas_Elementos_ETRAD(ETRAD *TMP)
{       
	double kTmp,cTmp;
	int dDom,nFuentes;
	double faseT; // Funcion de fase
    nFuentes=TMP->Get_Fuentes()->Filas();
    Set_Reserva_Memoria_Fuentes(nFuentes);
	Asigna_Numero_de_Fuentes(nFuentes);
	
	// Carga informacon de las fuentes 
	for(int i=0;i<nFuentes;i++)
	{
		Fuentes[i].Set_X(TMP->Get_Fuentes()->Retorna(i,0));
    	Fuentes[i].Set_Y(TMP->Get_Fuentes()->Retorna(i,1));
		Fuentes[i].Set_vNodal(TMP->Get_Fuentes()->Retorna(i,2));	
	//	printf(" Fuente (%d) en (%f,%f) con Fluencia de %f fotones por unidad de medida   \n  ",i,Fuentes[i].Get_X(),Fuentes[i].Get_Y(),Fuentes[i].Get_Densidad());	
	
	}
	
	// Carga la informacion en los elementos finitos de los coeficientes de absorcion, scattering asociados a un dominio y la funcion de fase en un medio homogeneo  
	for(unsigned long long i=0;i<Numero_Elementos;i++)
	{
	    dDom=Elementos_Geometria[i].Get_Dominio();
  		cTmp=TMP->Get_Vector_Ua()->Retorna(dDom);
		kTmp=TMP->Get_Vector_Us()->Retorna(dDom);
		Elementos_Geometria[i].Set_sVariable_Elemento(kTmp);
		fEsfera=faseT =TMP->Get_Funcion_Fase();
		kTmp=TMP->Coeficiente_difusion(cTmp,kTmp,faseT);
	    Carga_Variable_Fisica_K_Elemento(i,kTmp);
        Carga_Variable_Fisica_M_Elemento(i,cTmp);

	}	  
}


/// Carga el factor fisico asociado a la fisica-geometria , en el caso de la ETR-AD es el scattering
void Geometria2D_Triangulos::Carga_Variable_Fisica_K_Elemento(unsigned long long id,double Valor)
{   
	Elementos_Geometria[id].Set_kVariable_Elemento(Valor);
}

/// Carga el factor fisico asociado a la fisica-masa , en el caso de la ETR-AD es absorcion
void Geometria2D_Triangulos::Carga_Variable_Fisica_M_Elemento(unsigned long long id,double Valor)
{   
	Elementos_Geometria[id].Set_mVariable_Elemento(Valor);
}

/// Genera las matrices de rigidez de todos los elementos
void Geometria2D_Triangulos::Generar_Matrices_de_Rigidez_Elementos()
{
	for(unsigned long long i=0;i<Numero_Elementos;i++)
	Generar_Matriz_Rigidez_Elemento(i);
}

/// Genera las matrices de masas de todos los elementos
void Geometria2D_Triangulos::Generar_Matrices_de_Masa_Elementos()
{
	for(unsigned long long i=0;i<Numero_Elementos;i++)
	Generar_Matriz_Masa_Elemento(i);
}

/// Calcula la matriz gradiente de un elemento finito triangular de tres nodos
void Geometria2D_Triangulos::Matriz_Gradiente_Elemento(unsigned long long id,unsigned long long nodo)
{
double AreaTMP,Bi,Ci;
AreaTMP=Elementos_Geometria[id].Get_Area();
Bi=Elementos_Geometria[id].Get_Coeficientes_Permutacion()->Retorna(nodo,bi);	
Ci=Elementos_Geometria[id].Get_Coeficientes_Permutacion()->Retorna(nodo,ci);
Elementos_Geometria[id].Set_Matriz_Gradiente(Bi/(2.0*AreaTMP),Ci/(2.0*AreaTMP));	
}

/// Calcula el area de un elemento utilizando la matriz de permutacion del elemento finito trinagular de tres nodos
void Geometria2D_Triangulos::Area_Elemento(unsigned long long id)
{
double AreaTMP;
AreaTMP=(Elementos_Geometria[id].Get_Coeficientes_Permutacion()->Retorna(2,2)*Elementos_Geometria[id].Get_Coeficientes_Permutacion()->Retorna(1,1) - Elementos_Geometria[id].Get_Coeficientes_Permutacion()->Retorna(1,2)*Elementos_Geometria[id].Get_Coeficientes_Permutacion()->Retorna(2,1))/2.0;
Elementos_Geometria[id].Set_Area(AreaTMP);
}

/// Calcula todas las areas de los elementos finitos triangulares de tres nodos
void Geometria2D_Triangulos::Areas_Elementos()
{
	for(unsigned long long i=0;i<Numero_Elementos;i++)
	Area_Elemento(i);
	Calculo_Area_Global();
}

/// Calcula el area global a partir de las sumas de las areas de los elementos
void Geometria2D_Triangulos::Calculo_Area_Global()
{
	
	double Suma_Areas=0.0;
	for(unsigned long long i=0;i<Numero_Elementos;i++)
	{
		Suma_Areas=Suma_Areas+Elementos_Geometria[i].Get_Area();
	}

	Almacenar_Area_Dominio(Suma_Areas);

}

/// Muestra el area de un elmento triangular de tres nodos
void Geometria2D_Triangulos::Ver_Area_Elemento(unsigned long long id)
{
printf("\nEl Area del Elemento %Ld es:%f \n",id,Elementos_Geometria[id].Get_Area());
}

/// Muestra todas las areas de los elementos triangulares de tres nodos
void Geometria2D_Triangulos::Ver_Areas_Elementos()
{ 
	printf("\n Lista de Areas de los Elementos \n");
	for(unsigned long long i=0;i<Numero_Elementos;i++)
	Ver_Area_Elemento(i);
}
