#include "Sistema_Lineal.hpp"
#define Condicion_Convergencia           	    500    // Solo en caso de para con condicion de error, si el error no tiene variacion de para el ciclio iterativo del LSQR
double Tiempo_Inicial_Comunicacion_LSQR,Tiempo_Parcial_Comunicacion_LSQR;

/// Redimensiona un sistema de ecuaciones representados matricialmente con Hashing , indicando que filas-columnas se desean eliminar	
void Sistema_Lineal::Redimensionar_Sistema_Hashing(map<unsigned long long,double> Matriz_A_FEM ,map<unsigned long long,unsigned long long>Mapeo_FEM_Fila, map<unsigned long long,unsigned long long > Mapeo_FEM_Columna,Vector *b,Vector_Entero *vBorde,unsigned long DimActual)
{
	unsigned long long indice,tmp,ubicacion;
	std::map<unsigned long long,unsigned long long>::iterator posicion;
	std::map<unsigned long long,double>::iterator indice_nuevo;
	for(indice=0; indice<vBorde->Columnas(); indice++)
	{	
		
		tmp=vBorde->Retorna(indice);
			
		/// Elimina la informacion de tipo map de columnas, filas y datos, a partir del vector de entrada donde se indica que filas - columnas se quieren eliminar de la matriz
		for (posicion = Mapeo_FEM_Fila.begin(); posicion != Mapeo_FEM_Fila.end();) // recorre el mapeo de las filas
		{
				if(posicion->second==tmp || Mapeo_FEM_Columna[posicion->first] == tmp) // si una fila o columna del mapeo pertenece al vector fronteras
				{
				ubicacion =  posicion->first;                                                         // se almacena la llave asociada al indice de la matriz dispersa en map
				Mapeo_FEM_Columna.erase(ubicacion);                                                        // se elimina el indice de columna asociada a la llave
				Matriz_A_FEM.erase(ubicacion);                                                              // se elimina el dato asociada a la llave
				Mapeo_FEM_Fila.erase(posicion++);                                                             // se elimina el indice de fila asociada a la llave
				}
				else
				++posicion;
	    }		
		
	}
	

}


/// Redimensiona un sistema de ecuaciones representados matricialmente, indicando que filas-columnas se desean eliminar	
void Sistema_Lineal::Redimensionar_Sistema(Matriz *a,Vector *b,Vector_Entero *vBorde,unsigned long long DimFrontera) 
{

	unsigned long long NuevaDim,DimActual;
	unsigned long long tmpi,tmpj,k,m;
	DimActual=a->Columnas();
	NuevaDim=DimActual-DimFrontera;
	printf("La Dimension del borde: %Ld, la nueva dimension del sisema es por lo tanto: %LdX%Ld\n ",DimFrontera,NuevaDim,NuevaDim);
	Matriz *aReducida=new Matriz(NuevaDim,NuevaDim);
	Vector *QReducido=new Vector(NuevaDim);
		k=0;
	for (unsigned long long i = 0; i < DimActual; i++)     
	{
		m=0; 	 
        tmpi=vBorde->Buscar_Valor_Limite(i,DimFrontera);
	
		if(tmpi!=1)
		{		
         QReducido->Asigna(k,b->Retorna(i)); //original

			for (unsigned long long j = 0; j < DimActual; j++)  
				   {
					    tmpj=vBorde->Buscar_Valor_Limite(j,DimFrontera);
								if(tmpj!=1)   
								{
								aReducida->Asigna(k,m,a->Retorna(i,j));	
								m++;	
								}  
					   
				   }
		   k++;
    	   }
}	


a->Libera_Memoria();
a->Solicita_Memoria(NuevaDim,NuevaDim);
aReducida->Copia(a);

b->Libera_Memoria();
b->Solicita_Memoria(NuevaDim,0.0);
QReducido->Copia(b);


}



void Sistema_Lineal::LSQR_Resolver_Paralelo(Vector *x, Vector *b,unsigned long long *iParticion,Vector_Entero *COLUMNAS,Vector_Entero *INDICES,Vector *DATOS,int Tipo_Parada,int traza)
{
     double tolerancia=0.00000001;//10e-8;
    int size,rank;
	int TAG_iFila=10;	
	
    unsigned long long  *Inicio_Fila;
	int *DESP;
	int *convergencia=new int[1];
	unsigned long long *iFila=new unsigned long long[1];
	unsigned long long *desplazamiento=new unsigned long long[1];
	double suma=0.0;   
	double inicioLSQR,finalLSQR; 
	//double Tiempo_Producto_inicio,Tiempo_Producto_Final;
    
	unsigned long long contador_global;
    double theta,phi, phibar, rho, rhobar;
    double c, s;
    double arnorm,pPunto,normaB,normaA;
   	unsigned long long int nfilas, ncols,ctmp;
    
    double suma_n1=10e9;
	double diferencia;
    int contador_verificacion=0;
	unsigned long long dim;
		
	
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);	
	MPI_Comm_size(MPI_COMM_WORLD, &size);
 
	nfilas=ncols=x->Columnas();
    convergencia[0]=0;
	dim = iParticion[0] - 1;	
	desplazamiento[0]=dim;
    contador_global=0;
	
	Vector *U=new Vector(dim,"U");	
	Vector *ERROR=new Vector(Get_Iteraciones());
	Vector *u=new Vector(nfilas,"u");
	Vector *w=new Vector(nfilas,"w");
	Vector *v=new Vector(nfilas,"v");		
	Vector *ax=new Vector(dim,"ax");
	Vector *V=new Vector(nfilas,"V");	
	Vector *AX=new Vector(nfilas,"AX");	
	Vector *Norma2=new Vector(nfilas,"Norma2");	
	Rutinas_Paralelas *RUTINA;
	Vector *x_star=new Vector(dim,"XSTAR");	
	Vector *x_new=new Vector(nfilas,"XNEW");
	Vector *X_NEW=new Vector(nfilas,"X_NEW");	
		
	if(rank==0)
	{ 
		inicioLSQR=MPI_Wtime();
	    Inicio_Fila = new 	unsigned long long  [INDICE_FILAS->Columnas()];
		DESP = new int [(int) INDICE_FILAS->Columnas()];
		Inicio_Fila[0]=0;
		iFila[0]=0;
		DESP[0]=dim;
        Tiempo_Inicial_Comunicacion_LSQR=MPI_Wtime();    
			for(int i=1;i<size;i++)
			{
				DESP[i] =(int) ( Inicio_Fila[ ( i-1 ) ] + INDICE_FILAS->Retorna(i));	//original
				Inicio_Fila[i] = (	unsigned long long ) dim*i; 
				RUTINA->Envio_Dato_Entero(i,TAG_iFila,Inicio_Fila[i]); // Envio la fila inicial
				
			}		
		
	 }
  
    Tiempo_Parcial_Comunicacion_LSQR=0.0;
	
	if(rank!=0)
	{	
		RUTINA->Recibe_Dato_Entero(0,TAG_iFila,iFila); // Recibo la fila inicial
	}	
   	Tiempo_Parcial_Comunicacion_LSQR+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_LSQR;
	
	
  
 

				 for (unsigned long long j=0; j < nfilas; j++)
				 u->Asigna(j,b->Retorna(j)); 	 
			  
				 normaB=b->Vector_Normal();
				 for (unsigned long long j=0; j < nfilas; j++) 
				 {
				  u->Asigna(j,u->Retorna(j)/normaB);
				 }
 
				 
				 for (unsigned long long j=0; j < dim; j++) 
				 {
				  ctmp=j + iFila[0];	 
				  U->Asigna(j,u->Retorna(ctmp)  ); 
				 }
				 

				 

  				 RUTINA->Producto_Matriz_Dispersa_Vector_Seccion(u,v,INDICES,COLUMNAS,DATOS,dim,iFila[0]);
				 
				 Tiempo_Inicial_Comunicacion_LSQR=MPI_Wtime();
				 MPI_Allreduce( v->Retorna_Puntero_Datos(), V->Retorna_Puntero_Datos(),nfilas, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
				 Tiempo_Parcial_Comunicacion_LSQR+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_LSQR;
				 normaA = V->Vector_Normal();
				 
				 for (unsigned long long i=0; i < ncols; i++)
				 {
					V->Asigna(i,V->Retorna(i)/normaA);
					w->Asigna(i,V->Retorna(i));
				 }
				
				 arnorm = normaA * normaB;
				 phibar = normaB; 
				 rhobar = normaA;
	if(rank==0) 
    printf("Comunicacion Inicial: %f",Tiempo_Parcial_Comunicacion_LSQR);

    while(  ( Tipo_Parada ==1  ) ||  ( Tipo_Parada == 0 && convergencia[0] == 0 ) ) // Para en base a numero de iteraciones o en base a que se alacanza convergencia
    {
	    //1� Multiplicacion

		//Tiempo_Producto_inicio = MPI_Wtime();
		RUTINA->Producto_Matriz_Dispersa_Vector(V,x_star,INDICES,COLUMNAS,DATOS);
	
		for (unsigned long long j=0; j < dim; j++)
		U->Asigna(j, (x_star->Retorna(j) - normaA*U->Retorna(j)));
		
        pPunto=U->Producto_Punto();
		
		// 1� comunicacion ( pequenio)
		Tiempo_Inicial_Comunicacion_LSQR=MPI_Wtime(); 
		MPI_Allreduce( &pPunto, &normaB, 1, MPI_DOUBLE, MPI_SUM,MPI_COMM_WORLD ); // Comunicacion minima
		Tiempo_Parcial_Comunicacion_LSQR+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_LSQR;
	
		normaB=(double)sqrt(normaB);
		
	   if (normaB == 0.0)
	   {
		printf("Vector normal en LSQR = 0.0 el programa no continuara\n");
		MPI_Finalize();
    	exit(1);
	   }
					
				 for (unsigned long long j=0; j < dim; j++) 
				 {
				   U->Asigna(j,U->Retorna(j)/normaB  ); 
				 }
				
				x_new->Inicializa(0.0);
				// 2� Multiplicacion 
				RUTINA->Producto_MatrizT_Dispersa_Vector(U,x_new,INDICES,COLUMNAS,DATOS,dim,iFila[0]);	 
		
				// 2� comunicacion ( grande )
				
     			Tiempo_Inicial_Comunicacion_LSQR=MPI_Wtime();   
				MPI_Allreduce( x_new->Retorna_Puntero_Datos(), X_NEW->Retorna_Puntero_Datos(),nfilas, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD ); // Comunicacion principal
        		Tiempo_Parcial_Comunicacion_LSQR+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_LSQR;
			 
		             // Asigancion serial es mas rapida que paralela + comunicacion
					 for (unsigned long long k=0; k < ncols; k++)
					 V->Asigna(k, X_NEW->Retorna(k) - normaB*V->Retorna(k) );  
					 
					 
					 // Calculo de la norma de V
					 normaA=V->Vector_Normal();
				
					 if (normaA == 0.0)
					   {
						 printf("Vector normal en LSQR = 0.0 el programa no continuara\n");
						 MPI_Finalize();
    				     exit(1);
					   }
			
					 // Normalizacion de V
					 for (unsigned long long k=0; k < ncols; k++)
					 V->Asigna(k, V->Retorna(k)/normaA); 	
				
					 
					 // Construccion y aplicacion de la siguiente transformacion ortogonal
					 
					 rho    =  sqrt( rhobar*rhobar + normaB*normaB );
					 c      =  rhobar / rho;
					 s      =  normaB   / rho;
					 theta  =  s * normaA;
					 rhobar = -c * normaA;
					 phi    =  c * phibar;
					 phibar =  s * phibar;
					 
					 		 
					 // Actualizacion de vectores
					 for (unsigned long long k=0; k < ncols; k++)
					 {
					   x->Asigna(k,x->Retorna(k) +   (phi/rho) * w->Retorna(k)   );
					   w->Asigna(k,V->Retorna(k) - (theta/rho) * w->Retorna(k));	   
					 }
				 

					   
		  
	 if(Tipo_Parada==0 || ( Tipo_Parada ==1 && traza == 1 )) // solo para efectos de seguimiento del algoritmo
     {	

		 RUTINA->Producto_Matriz_Dispersa_Vector(x,ax,INDICES,COLUMNAS,DATOS);			
        
         Tiempo_Inicial_Comunicacion_LSQR=MPI_Wtime();   
         MPI_Gatherv(ax->Retorna_Puntero_Datos(),dim,MPI_DOUBLE,AX->Retorna_Puntero_Datos(),DESP,(int*)Inicio_Fila,MPI_DOUBLE,0,MPI_COMM_WORLD);
    	 Tiempo_Parcial_Comunicacion_LSQR+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_LSQR;
	
			 if(rank==0) 
			 {			 
							if(contador_global!=0)
							suma_n1=suma;  
							suma=Norma2->Error_Norma2(AX,b);
							diferencia=suma_n1-suma;
	
									
							 if(contador_global%1==0.0 &&  traza ==1 || ( contador_global%100==0.0 &&traza==2 ))
							 {
							    printf("Iteracion %Ld \n",contador_global);
									
							 }
									 
										 if(Tipo_Parada==0)
										 {	 
										 if(diferencia<=tolerancia)
										 contador_verificacion++;
										 else
										 contador_verificacion=0;	 
										}
															
	
							if( diferencia <= tolerancia && contador_verificacion >= Condicion_Convergencia) 
							{
								 convergencia[0]=1;
							 }	 
	
				}
				
		 
		}
				     	 if(rank==0) 
						 {	
							     /// Muestra cada 100 iteraciones un contador en pantalla...
				                 if(  contador_global%100==0.0 &&traza==2 )
								 {
							        printf("Iteracion  %Ld  \n",contador_global);
	 
								}	 
						} 
						
						
                        Tiempo_Inicial_Comunicacion_LSQR=MPI_Wtime();   
						MPI_Bcast(&convergencia[0],1,MPI_INT,0,MPI_COMM_WORLD);
     					Tiempo_Parcial_Comunicacion_LSQR+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_LSQR;
	
						
						 if(traza==1)
						 ERROR->Asigna(contador_global,suma);

						 contador_global++;
						 if( contador_global >= Get_Iteraciones())
                         break;							 
	}
       
			if(rank==0)
			{
					if(traza==1)
					{
						char *NombreSolucion="ERROR_LSQR_Paralelo.dat";
						Archivo *ERROR_ARCHIVO=new Archivo();
						ERROR_ARCHIVO->SetNombre(NombreSolucion);
						ERROR_ARCHIVO->SetModo(GRABA_ARCHIVO);	
						ERROR_ARCHIVO->Abrir();
						ERROR_ARCHIVO->Escribe_Vector(ERROR,ERROR->Columnas());   
						ERROR_ARCHIVO->Cerrar();
					}

					if(suma > 1000)
					{
						printf("\n\n ************ ATENCION ES MUY PROBABLE QUE LA APROXIMACION SEA INCORRECTA POR UNA MALA TRIANGULACION DE MALLA FEM UTILIZADA  *****************     ");	
						printf("\n    ************ SE ACONSEJA REFINAR LA MALLA Y EVITAR TRIANGULOS CON ANGULOS MUY AGUDOS ************* \n ************* YA QUE ESTE TIPO DE TRIANGULOS ES CAUSANTE DE DIVERGENCIA EN ALGUNAS APROXIMACIONES NODALES  *****************     \n TAMBIEN PUEDE SER POR QUE DEBEN SER MAS ITERACIONES\n");	
				
					}	
		
			finalLSQR=MPI_Wtime();	
					
	              if(Tipo_Parada==0 || ( Tipo_Parada ==1 && traza == 1 ))
                  printf("Iteraciones: %Ld  en   %f segundos con norma 2=%f y diferencia = %f\n",contador_global,finalLSQR-inicioLSQR,suma,diferencia);
	              else
	              printf("Iteraciones: %Ld  en   %f segundos \n",contador_global,finalLSQR-inicioLSQR);			
			
	}		
     
	if(rank==0)
	{ 
		tSistema=Tiempo_Parcial_Comunicacion_LSQR;    
	}
    u->Libera_Memoria();
	w->Libera_Memoria();
	x_star->Libera_Memoria();
	x_new->Libera_Memoria();
	Norma2->Libera_Memoria();   
	
	
} 



/// LSQR para un procesador con matrices dispersas

void Sistema_Lineal::LSQR_Resolver(Matriz_Dispersa *MDA, Vector *x, Vector *b,int Tipo_Parada,int traza)
{
	
	double tolerancia=0.00001;//10e-15;
    double theta, alpha, beta, phi, phibar, rho, rhobar;
    double c, s;
    double arnorm;
   	unsigned long long nfilas, ncols,iter;
	nfilas=ncols=x->Columnas();
	Vector *ERROR=new Vector(Get_Iteraciones());
	Vector *u=new Vector(nfilas); 
	Vector *v=new Vector(ncols);
	Vector *w=new Vector(ncols);
	Vector *x_star=new Vector(nfilas);
	Vector *x_new=new Vector(ncols);
	Vector *x_aux=new Vector(nfilas);
	Vector *Norma2=new Vector(nfilas);
	Vector *AX=new Vector(nfilas);
	double suma=0.0;  
	double suma_n1=10e9;
	double diferencia=10e9;
	int contador_verificacion=0;
	double inicioLSQR,finalLSQR;
    inicioLSQR=MPI_Wtime();
	
	for (unsigned long long j=0; j < nfilas; j++)
	u->Asigna(j,b->Retorna(j)); 	 
 
	beta=b->Vector_Normal();
	
	for (unsigned long long j=0; j < nfilas; j++) 
	{
    	 u->Asigna(j,u->Retorna(j)/beta); //Muchos ceros pero no es relevante para la complejidad del algorimto
	}

	 MDA->Producto_MA_Vector(u,v); // Solo multiplica muy pocos valores 3*n_fuentes como maximo - original
	 alpha = v->Vector_Normal();
     for (unsigned long long i=0; i < ncols; i++)
     {
		v->Asigna(i,v->Retorna(i)/alpha);
    	w->Asigna(i,v->Retorna(i));
	 }
	
     arnorm = alpha * beta;
     phibar = beta;
     rhobar = alpha;

     printf("Iterando...\n");
	 unsigned long long i=0;
	 while( ( Tipo_Parada==1) ||  ( Tipo_Parada == 0 && ( diferencia > tolerancia || contador_verificacion != Condicion_Convergencia)  ) )
    {
	
     MDA->Producto_MA_Vector(v,x_star); // Solo multiplica pocos valores, pero es innecesaria una inversion de multiplicaion dispersa en este punto 

    
     
		   
    for (unsigned long j=0; j < nfilas; j++)
    u->Asigna(j,x_star->Retorna(j) - alpha*u->Retorna(j));

    beta=u->Vector_Normal();
	if (beta == 0.0)
	{
	    printf("El vector normal no pude ser cero, el programa finalizara el proceso de resolucion LSQR \n");
        MPI_Finalize();
		exit(1);
	}

	 for (unsigned long long j=0; j < nfilas; j++)
     u->Asigna(j,u->Retorna(j)/beta);
	
	 
     MDA->Producto_MA_Vector(u,x_new);


     for (unsigned long long k=0; k < ncols; k++)
	 v->Asigna(k, x_new->Retorna(k) - beta*v->Retorna(k) );  
	 alpha=v->Vector_Normal();

	 if (alpha == 0.0)
    {
	    printf("El vector normal no pude ser cero, el programa finalizara el proceso de resolucion LSQR \n");
        MPI_Finalize();
		exit(1);
    }

	 for (unsigned long long k=0; k < ncols; k++)
     v->Asigna(k, v->Retorna(k)/alpha); 	

	 // Construccion y aplicacion de la siguiente transformacion ortogonal
	 
	 rho    =  sqrt( rhobar*rhobar + beta*beta );
	 c      =  rhobar / rho;
	 s      =  beta   / rho;
	 theta  =  s * alpha;
	 rhobar = -c * alpha;
	 phi    =  c * phibar;
	 phibar =  s * phibar;


	 for (unsigned long long k=0; k < ncols; k++)
    {
	   x->Asigna(k,x->Retorna(k) +   (phi/rho) * w->Retorna(k)   );
       w->Asigna(k,v->Retorna(k) - (theta/rho) * w->Retorna(k));	   
    }
            // Solo para uso de estudio de convergencia o detencion de iteracion en base a una tolerancia de error dada
			if(Tipo_Parada==0 || ( Tipo_Parada ==1 && traza == 1 ))
			{	
			 MDA->Producto_MA_Vector(x,AX); 
			   
			 if(i!=0)
			 suma_n1=suma;  
			 
			 suma=Norma2->Error_Norma2(AX,b);
			 diferencia=suma_n1-suma;
			 }
             
            // Solo para uso de estudio de convergencia y traza del metodo
			if((i%1==0.0 &&  traza==1 )|| ( i%100==0.0 &&traza==2 )) // muestra los datos cada 100 iteraciones
			 {
				 if(Tipo_Parada==0)
				 printf("La convergencia en  %Ld es: (t = %f, t-1 = %f)  DIf = (%f) \n", i,suma,suma_n1,diferencia);
				 else
				 printf("Iteracion  %Ld \n", i);
				 	 
			 }
			
             // Solo en caso de querer utilizar el metodo basado en la detencion del algortimmo en base a una tolenacia de error dada			 
			 if(Tipo_Parada==0)
			 {	 
			 if(diferencia<=tolerancia)
			 contador_verificacion++;
			 else
			 contador_verificacion=0;	 
			}
			   
	
     if(Tipo_Parada==0 || ( Tipo_Parada ==1 && traza == 1 ) ) // solo si se desea el funcioanmiento en base a una tolerancia o solo si se quiere estudiar la convergencia del metodo
	 ERROR->Asigna(i,suma);
	
	 i++;
     iter=i;
	 
     if(i == Get_Iteraciones()) // se finaliza el proceso iterativo cuando se alacanza el numero de iteraciones indicado en el inicio del programa
	 break;	 
 }
  finalLSQR=MPI_Wtime();	
 
     if(traza==1)
	{
		char *NombreSolucion="ERROR_LSQR_Dispersa.dat";
		Archivo *ERROR_ARCHIVO=new Archivo();
		ERROR_ARCHIVO->SetNombre(NombreSolucion);
		ERROR_ARCHIVO->SetModo(GRABA_ARCHIVO);	
		ERROR_ARCHIVO->Abrir();
		ERROR_ARCHIVO->Escribe_Vector(ERROR,ERROR->Columnas());   
		ERROR_ARCHIVO->Cerrar();
    }
	
	// si no se produce convergencia es posible que exista un error en la geometria definida en el proceso de la FEM
    if(suma > 1000)
	{
	printf("\n\n  ************ ATENCION ES MUY PROBABLE QUE LA APROXIMACION SEA INCORRECTA POR UNA MALA TRIANGULACION DE MALLA FEM UTILIZADA  *****************     \n\n");	
	}	
 
	if(Tipo_Parada==0 || ( Tipo_Parada ==1 && traza == 1 ))
    printf("Iteraciones: %Ld  en   %f segundos con norma 2=%f y diferencia = %f\n",iter,finalLSQR-inicioLSQR,suma,diferencia);
	else
	printf("Iteraciones: %Ld  en   %f segundos \n",iter,finalLSQR-inicioLSQR);
	  	
	u->Libera_Memoria();
	v->Libera_Memoria();
	w->Libera_Memoria();
	x_star->Libera_Memoria();
	x_new->Libera_Memoria();
	x_aux->Libera_Memoria();
	
	   
}    

/// LSQR con matriz tradicional 
void Sistema_Lineal::LSQR_Resolver_Normal(Matriz *A, Vector *x, Vector *b,int Tipo_Parada,int traza)
{
	 double tolerancia=0.00000001;
     double theta, alpha, beta, phi, phibar, rho, rhobar;
     double c, s;
     double arnorm;
   	 unsigned long long nfilas, ncols,iter;
     nfilas=ncols=x->Columnas();
	 Vector *ERROR=new Vector(Get_Iteraciones());
	 Vector *u=new Vector(nfilas);
	Vector *v=new Vector(ncols);
	Vector *w=new Vector(ncols);
	Vector *x_star=new Vector(nfilas);
	Vector *x_new=new Vector(ncols);
	Vector *x_aux=new Vector(nfilas);
	Vector *Norma2=new Vector(nfilas);
	Vector *AX=new Vector(nfilas);
	 double suma=0.0;  
	 double suma_n1=10e9;
	 double diferencia=10e9;
	 int contador_verificacion=0;
	 double inicioLSQR,finalLSQR;
     inicioLSQR=MPI_Wtime();
	 for (unsigned long long j=0; j < nfilas; j++)
	 u->Asigna(j,b->Retorna(j)); 	 
  
	 beta=b->Vector_Normal();
	
	 for (unsigned long long j=0; j < nfilas; j++) 
	 {
      u->Asigna(j,u->Retorna(j)/beta); //Muchos ceros pero no es relevante para la complejidad del algorimto
	 }
	 
	 A->Multiplica(u,v); // Solo multiplica muy pocos valores 3*n_fuentes como maximo 

	 alpha = v->Vector_Normal();
     for (unsigned long long i=0; i < ncols; i++)
     {
		v->Asigna(i,v->Retorna(i)/alpha);
    	w->Asigna(i,v->Retorna(i));
	 }
	
     arnorm = alpha * beta;
     phibar = beta;
     rhobar = alpha;

     printf("Iterando...\n");
	 unsigned long long i=0;
	 while( ( Tipo_Parada==1  ) ||  ( Tipo_Parada == 0 && ( diferencia > tolerancia || contador_verificacion != Condicion_Convergencia) ) )
    {
  

     A->Multiplica(v,x_star);

		   
   finalLSQR=MPI_Wtime();	
     
		   
   for (unsigned long long j=0; j < nfilas; j++)
    u->Asigna(j,x_star->Retorna(j) - alpha*u->Retorna(j));

    beta=u->Vector_Normal();
	 if (beta == 0.0)
	   {
	    printf("El vector normal no pude ser cero, el programa finalizara el proceso de resolucion LSQR \n");
        MPI_Finalize();
		exit(1);
	   }

	 for (unsigned long long j=0; j < nfilas; j++)
      u->Asigna(j,u->Retorna(j)/beta);
	
	 

		//MDA->Producto_MA_Vector(u,x_new);
        A->Multiplica(u,x_new);



     for (unsigned long long k=0; k < ncols; k++)
	 v->Asigna(k, x_new->Retorna(k) - beta*v->Retorna(k) );  
	 alpha=v->Vector_Normal();
	 if (alpha == 0.0)
	 {
        printf("El vector normal no pude ser cero, el programa finalizara el proceso de resolucion LSQR \n");
        MPI_Finalize();
		exit(1);
	 }

	 for (unsigned long long k=0; k < ncols; k++)
     v->Asigna(k, v->Retorna(k)/alpha); 	

	 // Construccion y aplicacion de la siguiente transformacion ortogonal
	 
	 rho    =  sqrt( rhobar*rhobar + beta*beta );
	 c      =  rhobar / rho;
	 s      =  beta   / rho;
	 theta  =  s * alpha;
	 rhobar = -c * alpha;
	 phi    =  c * phibar;
	 phibar =  s * phibar;


	 for (unsigned long long k=0; k < ncols; k++)
	   {
	   x->Asigna(k,x->Retorna(k) +   (phi/rho) * w->Retorna(k)   );
       w->Asigna(k,v->Retorna(k) - (theta/rho) * w->Retorna(k));	   
	   }

     //MDA->Producto_MA_Vector(x,AX); 
	 if(Tipo_Parada==0 || ( Tipo_Parada ==1 && traza == 1 ))
     {	
	 A->Multiplica(x,AX);
	   
     if(i!=0)
	 suma_n1=suma;  
     
	 suma=Norma2->Error_Norma2(AX,b);
	 diferencia=suma_n1-suma;
     }
	 
	 if(i%1==0.0 &&  traza==1 || ( i%100==0.0 &&traza==2 ))
	 {
     printf("La suma en [ %Ld] es: (t = %f, t-1 = %f)  DIf = (%f) \n", i,suma,suma_n1,diferencia);
	 }
	 
	 if(Tipo_Parada==0)
	 {	 
     if(diferencia<=tolerancia)
     contador_verificacion++;
	 else
	 contador_verificacion=0;	 
    }
	
	 if(Tipo_Parada==0 || ( Tipo_Parada ==1 && traza == 1 ) )
	 ERROR->Asigna(i,suma);
	 i++;
     if(i == Get_Iteraciones())
	 break;	
	 iter=i;

 }
 
    if(traza==1)
	{
 	char *NombreSolucion="ERROR_LSQR.dat";
	Archivo *ERROR_ARCHIVO=new Archivo();
	ERROR_ARCHIVO->SetNombre(NombreSolucion);
	ERROR_ARCHIVO->SetModo(GRABA_ARCHIVO);	
	ERROR_ARCHIVO->Abrir();
    ERROR_ARCHIVO->Escribe_Vector(ERROR,ERROR->Columnas());   
    ERROR_ARCHIVO->Cerrar();
    }
    if(suma > 1000)
	{
	printf("\n\n  ************ ATENCION ES MUY PROBABLE QUE LA APROXIMACION SEA INCORRECTA POR UNA MALA TRIANGULACION DE MALLA FEM UTILIZADA  *****************     \n\n");	
	}	
 
    printf("Iteraciones: %Ld  en   %f segundos con norma 2=%f y diferencia = %f\n",iter,finalLSQR-inicioLSQR,suma,diferencia);
	
	u->Libera_Memoria();
	v->Libera_Memoria();
	w->Libera_Memoria();
	x_star->Libera_Memoria();
	x_new->Libera_Memoria();
	x_aux->Libera_Memoria();
  
}    

// Resuelve el sistema LY=B, dados L y B
void Sistema_Lineal::Solve_L(Matriz *a, Vector *y, Vector *b)
{
   unsigned long long i, j, n;
   double t;
   n = a->Columnas();

   y->Asigna(0,  b->Retorna(0) / a->Retorna(0,0) );
   for (i = 0; i < n; i++)
   {
      t = b->Retorna(i);

      for (j = 0; j < i ; j++)
	  t -= a->Retorna(i,j) * y->Retorna(j);
      y->Asigna(i, t);
   }
}



/// Resuelve el sistema UX=Y, dados U  y Y
void Sistema_Lineal::Solve_U(Matriz *a, Vector *x, Vector *y)
{
   unsigned long long i, j, n = a->Columnas();
   double t;
   
   x->Asigna(n - 1, y->Retorna(n - 1) / a->Retorna(n - 1, n-1) );
   for (i = (n - 2); i >= 0; i--)
   {
      t = y->Retorna(i);
      for (j = (n - 1); j > i; j--) t -= a->Retorna(i,j) * x->Retorna(j);
      x->Asigna(i, t / a->Retorna(i,j) );
   }
}

/// Calcula la factorizacion LLT
 void Sistema_Lineal::Factoriza_LLT(Matriz *a)
 {
	 double suma;
	 double tmp;
	// double traza;
	 long long int n,i,j,k;
	 suma=0.0;
	 n= (long long int) a->Columnas();
	  printf("Factorizando LLT\n");
	
	for ( i = 0; i <n; i++) 
    {
        suma=a->Retorna(i,i); 
	      for(k=0; k<i;k++)
		   {
			suma = suma - pow( a->Retorna(k,i),2.0);
		   }
		   
		   a->Asigna(i,i,  (double) sqrt((double)suma));
			  for( j = i+1; j <  n ; j++) 
			  {				  
			   suma=a->Retorna(i,j); 
				  for( k=0; k<i;k++)
				   {
					suma = suma - a->Retorna(k,i)*a->Retorna(k,j);
				   }
				tmp=suma/a->Retorna(i,i);
			    a->Asigna(i,j, tmp);
				a->Asigna(j,i,tmp);   
	        	// printf("Asignando LLT[%Ld][%Ld]\n",i,j); 
			   }
	  
   	  }
	  
	   
}


 /// Resuelve un sistema factorizado utilizando Cholesky LLT
void Sistema_Lineal::Solve_LLT(Matriz *a, Vector *x, Vector *b)
{
   long long int n,i,j;
   
   n =   (long long int)  a->Columnas();
   printf("Resolviendo LLT\n");
	
	for( i=0;i<n;i++)
	{
		for( j=1;j<i;j++)
		{	
		b->Asigna(i, b->Retorna(i) - a->Retorna(j,i)*b->Retorna(j) );
		}
  
		b->Asigna(i,b->Retorna(i)/a->Retorna(i,i));	
	
	}

	b->Asigna(n-1, b->Retorna(n-1)/a->Retorna(n-1,n-1));
	 x->Asigna(n-1,b->Retorna(n-1));
	for( i=n-2; i >= 0; i--)
	{
		
		for(  j=i+1;j<n;j++)
		{	
		b->Asigna(i, b->Retorna(i) - a->Retorna(i,j)*b->Retorna(j) );
		}
  
		b->Asigna(i,b->Retorna(i)/a->Retorna(i,i));	
	    x->Asigna(i,b->Retorna(i));
		
	}

}

// Resuelve el sistema Ax=b usando factorizacion LU
void Sistema_Lineal::Resuelve_Factorizacion_LU(Matriz *a, Vector *x, Vector *b)
{
   Factoriza_LU(a);
   Solve_LU(a,x,b);
}

// Factoriza la matriz A en L y U dejandolas en la misma matriz
void Sistema_Lineal::Factoriza_LU(Matriz *a)
{

   
   unsigned long long i, j, k;
   double m, n;
   for (i = 0; i < a->Columnas(); i++) 
   {

      for (j = i+1; j < a->Columnas(); j++) 
      {
         n = a->Retorna(j,i);
     //  if (n == 0.0) continue;  // ojo si se usa, es menor el tiempo de computo que cholesky porque se salta el for, se dejo para la estimacion de tiempos
 
         m = n / a->Retorna(i,i);
         a->Asigna(j, i, m);

         for (k = i+1; k < a->Columnas(); k++) 
         {
            a->Asigna(j, k, a->Retorna(j,k) - m * a->Retorna(i,k) );
         }
      }
   }

}

void Sistema_Lineal::Solve_LU(Matriz *a, Vector *x, Vector *b)
{
   long long int i, j, n = (long long int) a->Columnas();
   double t;

   Vector *y = new Vector(n,"Vector Y");
	
   // Resuelve el sistema LY=B, dados L y B
   y->Asigna(0,  b->Retorna(0) / a->Retorna(0,0) );
   for (i = 0; i < n; i++)
   {
      t = b->Retorna(i);
      for (j = 0; j < i ; j++) t -= a->Retorna(i,j) * y->Retorna(j);
      y->Asigna(i, t);
   }

   // Resuelve el sistema UX=Y, dados U  y Y
   x->Asigna(n - 1, y->Retorna(n - 1) / a->Retorna(n - 1, n-1) );
   for (i = (n - 2); i >= 0; i--)
   {
	
      t = y->Retorna(i);
      for (j = (n - 1); j > i; j--) t -= a->Retorna(i,j) * x->Retorna(j);
      x->Asigna(i, t / a->Retorna(i,j) );
   }

   delete y;
}

// muestra el sistema, matriz A y vector b en archivos separado
 void Sistema_Lineal::mostrar_sistema(Matriz *a, Vector *x, Vector *b)
{
   unsigned long long n,i,j; // n = cant filas = cant columnas
   n = a->Columnas();
   cout<<"tama�o"<<n<<"\n\n";
   //cout<<".....................................................................::::::::::::::::::::::::::: tama�o"<<n<<"\n\n";
   ofstream fs1("/home/usuario/Escritorio/carpeta/matriz.dat"); //crear archivo de salida con la matriz
   ofstream fs2("/home/usuario/Escritorio/carpeta/vector.dat"); //crear archivo de salida con el vector
   for(i=0; i<n; i++)	
    { 
        for(j=0; j<n; j++) 
        { 
              fs1 << a->Retorna(i,j) <<" ";//enviar la matriz al archivo de salida
              //cout << a->Retorna(i,j) <<" \t";
        } 
        fs1 << "\n"; 
    } 
   for(i=0; i<n; i++)	
    { 
             fs2 << b->Retorna(i)<<" \n"; //enviar el vector al archivo de salida
    } 
    fs1.close(); //cerrar archivo de salida
    fs2.close();
}
 
 /// Resuelve un sistema de ecuaciones utilizando el metodo Jacobi
 void Sistema_Lineal::Resuelve_Jacobi(Matriz *a, Vector *x, Vector *b, double ep,int Tipo_Parada,int traza)
{
 unsigned long long dimLocal;
 dimLocal=x->Columnas();
   if (!a->Matriz_Cuadrada())
   {
      printf("\nError resolver por Jacobi, matriz no cuadrada\n\n");
      MPI_Finalize();
	  exit(1);
   }

   printf("\nResolviendo por Jacobi DIM %Ld %Ld Iteraciones %Ld condicion %d - -\n\n",dimLocal,b->Columnas(),Get_Iteraciones(),Tipo_Parada);
   unsigned long long m;
   int sw;
   double sum,tol;
   double tol_global;
   Vector *xt = new Vector(a->Columnas());
   Vector *xtem = new Vector(a->Columnas());
   Vector *ax_tmp= new Vector(a->Columnas()); // solo para traza
   Vector *ERROR=new Vector(Get_Iteraciones());
   m = 0;
   do
   {
       m++;
       sw = 0;
	   tol_global = 0.0;
       for (unsigned long long i = 0; i < a->Filas() ;i++)
       {
          sum = 0.0;
          for (unsigned long long j = 0; j < a->Columnas(); j ++)
          {
             if ( i == j) continue;
             sum += a->Retorna(i,j) * x->Retorna(j);
          }
          if (a->Retorna(i,i) == 0.0) {
             delete xt;
          }
          xt->Asigna(i,(1.0 / a->Retorna(i,i)) * (b->Retorna(i) - sum));
          xtem->Asigna(i,x->Retorna(i));
		
		  if(Tipo_Parada==0)
		 {	
	       tol=fabs(x->Retorna(i) - xt->Retorna(i));
		   tol_global = tol_global + tol;
		 }

    	  x->Asigna(i,xt->Retorna(i));  
	
		  }
	   
	             if (tol_global >= ep && Tipo_Parada==0)
				 {sw = 1;}
				 if( Tipo_Parada==1)
				 {sw =1;}
	   
	if(traza==1) // Traza de convergencia
	{
	   // Solo para traza de convergencia
	  a->Multiplica(x,ax_tmp);
      ERROR->Asigna(m-1,ax_tmp->Error_Norma2(ax_tmp,b));
	  printf(" [%Ld] ",m-1);
	}	
   	
	 if(m%1 == 0.0 && traza==1 || ( m%100==0.0 &&traza==2 ))
	 {	 
		  if(Tipo_Parada==0)
		   printf(" Iteracion %Ld con error (tolerancia) global = %f \n",m,tol_global);
		  else
		  {
			 printf(" Iteracion %Ld \n",m);  
		  }	  
	 }	   

	   
     if (m >= Get_Iteraciones() ) break;
   } while(sw);
   tol_global =0.0;
   
   for(unsigned long long i=0;i<a->Columnas();i++)
   {   
   	       tol=fabs(xtem->Retorna(i) - xt->Retorna(i));
		   tol_global = tol_global + tol;
   } 
	 printf(" Finalizo en la Iteracion %Ld \n",m);   //Efectos de traza
    if(traza==1)
	{
 	char *NombreSolucion="ERROR_Jacobi.dat";
	Archivo *ERROR_ARCHIVO=new Archivo();
	ERROR_ARCHIVO->SetNombre(NombreSolucion);
	ERROR_ARCHIVO->SetModo(GRABA_ARCHIVO);	
	ERROR_ARCHIVO->Abrir();
    ERROR_ARCHIVO->Escribe_Vector(ERROR,ERROR->Columnas());   
    ERROR_ARCHIVO->Cerrar();
    }
   
   delete xtem;
   delete ERROR;
   delete ax_tmp;
   delete xt;
}


// Resuelve Ax=b usando el metodo Gauus-Seidel
void Sistema_Lineal::Resuelve_Gauss_Seidel(Matriz *a, Vector *x, Vector *b, double ep,int Tipo_Parada,int traza)
{

	mostrar_sistema(a,x,b);
   Vector *ax_tmp= new Vector(a->Columnas()); // solo para traza

	
   if (!a->Matriz_Cuadrada())
   {
      printf("\nError resolver por Gauss Siedel, matriz no cuadrada\n\n");
      MPI_Finalize();
	  exit(1);
   }


   unsigned long long m;
   int sw;
   double sum,sum1,sum2;
   double tol;
   double tol_global=0.0;
   Vector *xt = new Vector(a->Columnas());
   Vector *ERROR=new Vector(Get_Iteraciones());
   m = 0;
   do
   {
       m++;
       sw = 0;
	   	   tol_global = 0.0;
       for (unsigned long long i = 0; i < a->Filas(); i++)
       {
          sum = 0.0;
		  sum1 = 0.0;
		  sum2 = 0.0;		   
    	   
		  for (unsigned long long j = 0; j < i; j ++)
          {
             sum1 += a->Retorna(i,j) * x->Retorna(j);
	      }
		  
		  for (unsigned long long j = i+1; j < a->Columnas(); j ++)
          {
             sum2 += a->Retorna(i,j) * x->Retorna(j);
	      }
		   sum = sum1 + sum2;
          
		   if (a->Retorna(i,i) == 0.0) 
		  { 
			  printf("\nError al resolver por Gauss Siedel, la diagonal po debe tener valores nulos\n\n");
              MPI_Finalize();
			  exit(1);
		  }
          xt->Asigna(i,(1.0 / a->Retorna(i,i)) * (b->Retorna(i) - sum));


         
		if(Tipo_Parada==0)
		 {	
	       tol=fabs(x->Retorna(i) - xt->Retorna(i));
		   tol_global = tol_global + tol;
		 }
		 
		 
		 
          x->Asigna(i,xt->Retorna(i));
       }
	   
	   
	             if (tol_global >= ep && Tipo_Parada==0)
				 {
					 sw = 1;
				 }
				 if( Tipo_Parada==1)
				 sw =1;	 
	   
	   
	   	if(traza==1)
	{
	   // Solo para traza de convergencia
	  a->Multiplica(x,ax_tmp);
      ERROR->Asigna(m-1,ax_tmp->Error_Norma2(ax_tmp,b));
	   printf(" [%Ld] ",m-1);
	}	

	   	        if(m%1 == 0.0 && traza==1 || ( m%100==0.0 &&traza==2 ))
				{	 
				  if(Tipo_Parada==0)	
		    	  printf(" Iteracion %Ld con tolerancia global = %f \n",m,tol_global);
	              else
				  printf(" Iteracion %Ld\n",m);  
				}	
	   
       if (m >= Get_Iteraciones()) break;
   } while (sw);
   printf(" Finalizo en la Iteracion %Ld \n",m);   //Efectos de traza
     if(traza==1)
	{
 	char *NombreSolucion="ERROR_Gauss_Seidel.dat";
	Archivo *ERROR_ARCHIVO=new Archivo();
	ERROR_ARCHIVO->SetNombre(NombreSolucion);
	ERROR_ARCHIVO->SetModo(GRABA_ARCHIVO);	
	ERROR_ARCHIVO->Abrir();
    ERROR_ARCHIVO->Escribe_Vector(ERROR,ERROR->Columnas());   
    ERROR_ARCHIVO->Cerrar();
    }
   
    delete ERROR;
}
