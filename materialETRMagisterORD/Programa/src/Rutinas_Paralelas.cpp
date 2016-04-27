#include "Rutinas_Paralelas.hpp"

   /// Almacena en punteros los nodos de la geometria FEM para distribucion de informacion entre procesadores
   void Rutinas_Paralelas::Almacenar_Nodos_Geometria(Nodo_2D *GeometriaFEM,unsigned long long nNodos)
   {
	   
	   nodoX = new Vector(nNodos);
       nodoY = new Vector(nNodos);
	   
	   for(unsigned long long i=0;i<nNodos;i++)
	   { 
		nodoX->Asigna(i,GeometriaFEM[i].Get_X());  
       	nodoY->Asigna(i,GeometriaFEM[i].Get_Y());  
	   }   

	}
   /// Almacena en punteros los nodos de la grilla para distribucion de informacion entre procesadores
   void Rutinas_Paralelas::Almacenar_Nodos_Grilla(Nodo_2D *GrillaFEM,unsigned long long  nNodos)
   {
	   
	   puntosX = new Vector(nNodos);
       puntosY = new Vector(nNodos);
	   
	   for(unsigned long long i=0;i<nNodos;i++)
	   { 
		puntosX->Asigna(i,GrillaFEM[i].Get_X());  
       	puntosY->Asigna(i,GrillaFEM[i].Get_Y());  
	   }   

	}
	
   /// Almacena en una fila especifica de la matriz Datos_Elementos (id elemento, area, coeficientes de permutacion, aproximaciones nodales y lista de nodos)
   void Rutinas_Paralelas::Almacenar_Datos_Elemento(unsigned long long  id_fila,unsigned long long  id_elemento,unsigned long long  nodo1,unsigned long long  nodo2,unsigned long long  nodo3,double area,double fi1,double fi2,double fi3, Matriz *Coeficientes_PermutacionEL)
   {
	   int contador=0;
	   Datos_Elementos->Asigna(id_fila,0,(double)id_elemento);
	   Datos_Elementos->Asigna(id_fila,1,(double)nodo1);
	   Datos_Elementos->Asigna(id_fila,2,(double)nodo2);
	   Datos_Elementos->Asigna(id_fila,3,(double)nodo3);
	   Datos_Elementos->Asigna(id_fila,4,(double)area);
	   Datos_Elementos->Asigna(id_fila,5,(double)fi1);
	   Datos_Elementos->Asigna(id_fila,6,(double)fi2);
	   Datos_Elementos->Asigna(id_fila,7,(double)fi3);
	   
	    Datos_Elementos_Vector->Asigna(id_fila*17 , (double)id_elemento);
	    Datos_Elementos_Vector->Asigna(id_fila*17 + 1 , (double)nodo1);
	    Datos_Elementos_Vector->Asigna(id_fila*17 + 2, (double)nodo2);
	    Datos_Elementos_Vector->Asigna(id_fila*17 + 3, (double)nodo3);
	    Datos_Elementos_Vector->Asigna(id_fila*17 + 4, (double)area);
	    Datos_Elementos_Vector->Asigna(id_fila*17 + 5, (double)fi1);
	    Datos_Elementos_Vector->Asigna(id_fila*17 + 6, (double)fi2);
	    Datos_Elementos_Vector->Asigna(id_fila*17 + 7, (double)fi3);
	    
	   
	   for(int i=0;i<(int) Coeficientes_PermutacionEL->Columnas();i++)
	   { 
		   for(int j=0;j< (int) Coeficientes_PermutacionEL->Columnas();j++)
			   {
				   Datos_Elementos->Asigna(id_fila,8 + contador,Coeficientes_PermutacionEL->Retorna(i,j)); 
				 
                   Datos_Elementos_Vector->Asigna(id_fila*17 + 8 + contador, Coeficientes_PermutacionEL->Retorna(i,j));

				   contador++;
			   }
        }   
	}
   
	
	/// Multiplica una matriz dispersa traspuesta por un vector guardando en vector salida el resultado
	void Rutinas_Paralelas::Producto_MatrizT_Dispersa_Vector(Vector *x,Vector *Salida,Vector_Entero *INDICES,Vector_Entero *COLUMNAS, Vector *DATOS,unsigned long long dim,unsigned long long inicio)//,int FilaInicio)//fila inicio no se utiliza
   {
			  
		  int size,rank;
		  MPI_Comm_rank(MPI_COMM_WORLD,&rank);	
		  MPI_Comm_size(MPI_COMM_WORLD, &size);
		  unsigned long long  nDimensionFilasMatriz=dim;
		  unsigned long long  LimiteInicial,LimiteFinal,nDatosRango;
		  double suma;
		  unsigned long long  cont=0;
		  double tmp; 
	   
			for(unsigned long long  k=0;k<nDimensionFilasMatriz;k++)
			{
				suma=0.0;
				LimiteInicial=INDICES->Retorna(k);
				LimiteFinal=INDICES->Retorna(k+1);
				nDatosRango=LimiteFinal-LimiteInicial;
				
				for(unsigned long long  i=0;i<nDatosRango;i++)
				{
				tmp = Salida->Retorna( COLUMNAS->Retorna( cont ) ); 	
				Salida->Asigna(COLUMNAS->Retorna( cont )  , tmp +  DATOS->Retorna( cont )*x->Retorna(k)); 
				cont++;
					
				}
				
			}
			
		
	} 
	

	/// Multiplica una matriz dispersa por un vector, guardando en vector salida el resultado
	void Rutinas_Paralelas::Producto_Matriz_Dispersa_Vector(Vector *x,Vector *Salida,Vector_Entero *INDICES,Vector_Entero *COLUMNAS, Vector *DATOS)//,int FilaInicio)//fila inicio no se utiliza
   {
			  
		  int size,rank;
		  MPI_Comm_rank(MPI_COMM_WORLD,&rank);	
		  MPI_Comm_size(MPI_COMM_WORLD, &size);
		  unsigned long long  nDimensionFilasMatriz=Salida->Columnas();
		  unsigned long long  contadorFila=0;
		  unsigned long long  LimiteInicial,LimiteFinal,nDatosRango;
		  double suma;
		  unsigned long long  cont=0;
		
			  
			for(unsigned long long  k=0;k<nDimensionFilasMatriz;k++)
			{
				suma=0.0;
				LimiteInicial=INDICES->Retorna(k);
				LimiteFinal=INDICES->Retorna(k+1);
				nDatosRango=LimiteFinal-LimiteInicial;
				for(unsigned long long  i=0;i<nDatosRango;i++)
				{
					suma=suma+DATOS->Retorna(i+cont)*x->Retorna(COLUMNAS->Retorna(i+cont));	
				}	
				cont=cont+nDatosRango;
				Salida->Asigna(contadorFila,suma);  
				
				
				contadorFila++;
			  
				
			}
			
			
	} 
 
		/// Multiplica una matriz dispersa por una seccion de un vector guardando en vector salida el resultado
	void Rutinas_Paralelas::Producto_Matriz_Dispersa_Vector_Seccion(Vector *x,Vector *Salida,Vector_Entero *INDICES,Vector_Entero *COLUMNAS, Vector *DATOS,unsigned long long dim,unsigned long long inicio)//,int FilaInicio)//fila inicio no se utiliza
   {
			  
		  int size,rank;
		  MPI_Comm_rank(MPI_COMM_WORLD,&rank);	
		  MPI_Comm_size(MPI_COMM_WORLD, &size);
		  unsigned long long  nDimensionFilasMatriz=dim;
		  unsigned long long  contadorFila=0;
		  unsigned long long  LimiteInicial,LimiteFinal,nDatosRango;
		  double suma;
		  unsigned long long  cont=0;
		//  Salida->Ver_Informacion();
		//	  printf("INICIO = %Ld",inicio);
			for(unsigned long long  k=0;k<nDimensionFilasMatriz;k++)
			{
				suma=0.0;
				LimiteInicial=INDICES->Retorna(k);
				LimiteFinal=INDICES->Retorna(k+1);
				nDatosRango=LimiteFinal-LimiteInicial;
				for(unsigned long long  i=0;i<nDatosRango;i++)
				{
					suma=suma+DATOS->Retorna(i+cont)*x->Retorna(COLUMNAS->Retorna(i+cont));	
					
					
					
				}	
				cont=cont+nDatosRango;
				Salida->Asigna( (unsigned long long) inicio +  contadorFila ,suma);
				contadorFila++;
			  
				
			}
			
			
	} 
	
		/// Multiplica una matriz dispersa por un vector, guardando en vector salida el resultado
	void Rutinas_Paralelas::Producto_Matriz_Dispersa_Vector_Seccion_LSQR(Vector *x,Vector *Salida,Vector_Entero *INDICES,Vector_Entero *COLUMNAS, Vector *DATOS,unsigned long long dim,unsigned long long inicio,double normaIn,double normaOut,Vector *u)//,int FilaInicio)//fila inicio no se utiliza
   {
			  
	   
	   
	   
	          //dentro del ciclo
		//for (unsigned long long j=0; j < nfilas; j++)
		//u->Asigna(j,X_STAR->Retorna(j) - alpha*u->Retorna(j));
	   
	   // u = normal
	   	// dentro del siguiente producto con un factor beta   
		//for (unsigned long long j=0; j < nfilas; j++)
		//u->Asigna(j,u->Retorna(j)/beta);
 	   // en el ciclo anterior
		//			 for (unsigned long long k=0; k < ncols; k++)
		//			 V->Asigna(k, X_NEW->Retorna(k) - beta*V->Retorna(k) );  
	   
		  int size,rank;
		  MPI_Comm_rank(MPI_COMM_WORLD,&rank);	
		  MPI_Comm_size(MPI_COMM_WORLD, &size);
		  unsigned long long  nDimensionFilasMatriz=dim;
		  unsigned long long  contadorFila=0;
		  unsigned long long  LimiteInicial,LimiteFinal,nDatosRango;
		  double suma;
	      double tmp,tmp2=0.0;
		  unsigned long long  cont=0;
		//  Salida->Ver_Informacion();
		//	  printf("INICIO = %Ld",inicio);
			for(unsigned long long  k=0;k<nDimensionFilasMatriz;k++)
			{
				suma=0.0;
				LimiteInicial=INDICES->Retorna(k);
				LimiteFinal=INDICES->Retorna(k+1);
				nDatosRango=LimiteFinal-LimiteInicial;
				for(unsigned long long  i=0;i<nDatosRango;i++)
				{
					suma=suma+DATOS->Retorna(i+cont)*x->Retorna(COLUMNAS->Retorna(i+cont));	
					
					
					
				}	
				
				tmp = suma - normaIn*u->Retorna( inicio +  contadorFila );
				u->Asigna( inicio +  contadorFila , tmp );
				tmp2+= tmp*tmp;
				
				cont= cont + nDatosRango;
				Salida->Asigna( (unsigned long long) inicio +  contadorFila ,suma);
				contadorFila++;
			  
				
			}
			normaOut = tmp2;
						printf("SECCION[%d] = %f   ",rank,normaOut); 

			
	} 
	
	
/// Particiona un vector segun numero de procesadores, almacenando el largo de cada sub vector
 void Rutinas_Paralelas::Obtener_Vector_Particion(int size,unsigned long long  orden,Vector_Entero *vParticion)
{
unsigned long long particion= (unsigned long long )orden/size;	
unsigned long long  cont=0;
			for(unsigned long long i=0;i<(unsigned long long) size;i++)
			{
				if(i==(unsigned long long)(size-1))
				{	
					vParticion->Asigna(i,orden - cont);	
				}
				else
				vParticion->Asigna(i,particion);	
				cont = cont + vParticion->Retorna(i);
			}
} 

/// Envia un vector entero long long sin signo 
void Rutinas_Paralelas::Envio_Vector_Entero(int procesador_destino,int TAG,unsigned long long  posicion_indice,unsigned long long  largo,unsigned long long  *vector)
{
	MPI_Send(&vector[posicion_indice],largo,MPI_UNSIGNED_LONG_LONG,procesador_destino,TAG, MPI_COMM_WORLD);	//Envia el numero de filas de la particion
}
	
/// Recibe un vector entero long long sin signo 
void Rutinas_Paralelas::Envio_Vector_Double(int procesador_destino,int TAG,unsigned long long  posicion_indice,unsigned long long  largo,double *vector)
{
	MPI_Send(&vector[posicion_indice],largo,MPI_DOUBLE,procesador_destino,TAG, MPI_COMM_WORLD);	//Envia el numero de filas de la particion
}

/// Envia un vector de tipo double 
void Rutinas_Paralelas::Envio_Dato_Double(int procesador_destino,int TAG,double Dato) 
{
	MPI_Send(&Dato,1,MPI_DOUBLE,procesador_destino,TAG, MPI_COMM_WORLD);	//Envia el numero de filas de la particion
}	

/// Recibe un dato de tipo double
void Rutinas_Paralelas::Recibe_Dato_Double(int procesador_origen,int TAG,double *con)
{
MPI_Status status;
	MPI_Recv(con,1,MPI_DOUBLE,procesador_origen,TAG,MPI_COMM_WORLD,&status);  //Recibe el dato entero
}

/// Envia un dato de tipo entero long long sin signo
void Rutinas_Paralelas::Envio_Dato_Entero(int procesador_destino,int TAG,unsigned long long  Dato) 
{
	MPI_Send(&Dato,1,MPI_UNSIGNED_LONG_LONG,procesador_destino,TAG, MPI_COMM_WORLD);	
}	

/// Recibe un dato de tipo entero long long sin signo
void Rutinas_Paralelas::Recibe_Dato_Entero(int procesador_origen,int TAG,unsigned long long  *con)
{
MPI_Status status;
	MPI_Recv(con,1,MPI_UNSIGNED_LONG_LONG,procesador_origen,TAG,MPI_COMM_WORLD,&status);  
}

/// Recibe un vector de tipo entero long long sin signo
void Rutinas_Paralelas::Recibe_Vector_Entero(int procesador_origen,int TAG,unsigned long long  largo,unsigned long long  *vector,MPI_Status status)
{
	MPI_Recv(&vector[0],largo,MPI_UNSIGNED_LONG_LONG, procesador_origen, TAG, MPI_COMM_WORLD,&status);
}

/// Recibe un vector de tipo double
void Rutinas_Paralelas::Recibe_Vector_Double(int procesador_origen,int TAG,unsigned long long  largo,double *vector,MPI_Status status)
{
	MPI_Recv(&vector[0],largo,MPI_DOUBLE, procesador_origen, TAG, MPI_COMM_WORLD,&status);
}

/// Recibe un vector de tipo double indicando la posicion del indicie desde donde se recibira la informacion
void Rutinas_Paralelas::Recibe_Vector_Double_Puntero(int procesador_origen,int TAG,unsigned long long  posicion_indice,unsigned long long  largo,double *vector,MPI_Status status)
{
	MPI_Recv(&vector[posicion_indice],largo,MPI_DOUBLE, procesador_origen, TAG, MPI_COMM_WORLD,&status);
}
