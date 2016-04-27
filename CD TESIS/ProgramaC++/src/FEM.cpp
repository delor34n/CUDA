#include "FEM.hpp"


/// Carga datos de calculos elementales a una matriz global para futuro calculo paralelo
void FEM::Carga_Datos_Elementos_Grilla(void)
{
	unsigned long long n1,n2,n3;
	double area,fi1,fi2,fi3;
	unsigned long long nElementos;
	int rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);	
    	
	if(rank==0)
	nElementos =  GeometriaTrabajo->Retorna_Numero_Elementos();

	Tiempo_Inicial_Comunicacion_Grilla=MPI_Wtime();
    MPI_Bcast(&nElementos,1,MPI_INT,0,MPI_COMM_WORLD);
	Tiempo_Parcial_Comunicacion_Grilla=MPI_Wtime() - Tiempo_Inicial_Comunicacion_Grilla;
  
	Proceso_Paralelo->Reserva_Memoria_Datos_Elementos_Vector(nElementos);
	
	if(rank==0)
	{	
	Proceso_Paralelo->Datos_Elementos = new Matriz(nElementos,17);
	
			for(unsigned long long  i=0;i<nElementos;i++)
			{
			
				n1 =GeometriaTrabajo->Retornar_Elementos_Geometria()[i].Get_Valor_Nodo(0);
				n2 =GeometriaTrabajo->Retornar_Elementos_Geometria()[i].Get_Valor_Nodo(1);
				n3 =GeometriaTrabajo->Retornar_Elementos_Geometria()[i].Get_Valor_Nodo(2);
				
				area = GeometriaTrabajo->Retornar_Elementos_Geometria()[i].Get_Area();
				
				fi1 = X->Retorna(n1);
				fi2 = X->Retorna(n2);
				fi3 = X->Retorna(n3);
				
				Proceso_Paralelo->Almacenar_Datos_Elemento(i,i, n1, n2, n3, area, fi1, fi2, fi3,  GeometriaTrabajo->Retornar_Elementos_Geometria()[i].Get_Coeficientes_Permutacion());
			
			}
			
	
    
	}

}

/// Carga de la geometria FEM, en esta instancia se carga en memoria los nodos y elementos internos y fronteras con los respectivos subdominios a los cuales perteneces cada uno, ademas 
/// se realizan calculos preliminares como las areas de los elementos y permutaciones elementales 
void FEM::CargarGeometriaFEM()
{ 
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);	

   if (TIPO_GEOMETRIA == GEOMETRIA_TRIANGULOS)
   {
   printf("Cargando Geometria2DTriangulos FEM con elementos triangulares\n");	  
	   
   GeometriaTrabajo=new Geometria2D_Triangulos();
   GeometriaTrabajo->Carga_Nodos(GetNombreArchivoNodos()); 
   GeometriaTrabajo->Carga_Elementos(GetNombreArchivoElementos()); 
   // Estos calculos se realizan en esta instancia para no repetir los ciclos for al momento de obtener las matrices de rigidez y masa locales, donde es necesario 
   // tener como dato, el area y la matriz de permutacion	   
 
   GeometriaTrabajo->Centroides_Elementos();
   GeometriaTrabajo->Permutaciones_Elementos();
   GeometriaTrabajo->Areas_Elementos();
   GeometriaTrabajo->Carga_Fronteras(GetNombreArchivoFronteras());
	   
   }

   
   if (TIPO_GEOMETRIA == GEOMETRIA_CUADRILATEROS)
   {
   printf("Geometria no disponible...");
   MPI_Finalize();
   exit(1); 	   
   }
   if (TIPO_GEOMETRIA == GEOMETRIA_TETRAHEDROS)
   {
   printf("Geometria no disponible...");
   MPI_Finalize();
   exit(1);
   }

}

/// Muestra el tipo de geometria utilizada
void FEM::VerTipoGeometria(void)
{ 
   printf("La geometria es...\n");	  
	
   if (TIPO_GEOMETRIA == GEOMETRIA_TRIANGULOS)
   {
   printf("Viendo geometria FEM con elementos triangulares\n");	  
      	   
		   
   }
   if (TIPO_GEOMETRIA == GEOMETRIA_CUADRILATEROS)
   {
   printf("Viendo Cuadrilateros no disponible...\n");
	   
   }
   if (TIPO_GEOMETRIA == GEOMETRIA_TETRAHEDROS)
   {
   printf("Viendo Tetrahedros no disponible...\n");
   }
	
}

/// Inicializa el problema de difusion de la luz. se cargan en memoria la informacion de la absorcion, scatter y la posicion y densidad de las fuentes de fotones
void FEM::Inicializa_Problema_ETRAD(ETRAD *FisicaProblema,int tipoFase)
{  tTotalComunicacion=0.0;	
	FisicaProblema->Carga_Absorcion(FisicaProblema->GetNombreArchivoAbsorcion());
	FisicaProblema->Carga_Scattering(FisicaProblema->GetNombreArchivoScattering());
	FisicaProblema->Carga_Fuentes(FisicaProblema->GetNombreArchivoFuentes());
	Set_sCarga((double)FisicaProblema->Get_Suma_Fuentes());
	GeometriaTrabajo->Carga_Variables_Fisicas_Elementos_ETRAD(FisicaProblema);
}


///Contruccion de matrices por elemento de Rigidez, masa y vectores de carga
void FEM::Contruir_Matrices_Problema()
{
	GeometriaTrabajo->Generar_Matrices_de_Rigidez_Elementos();
	GeometriaTrabajo->Generar_Matrices_de_Masa_Elementos();
    GeometriaTrabajo->Generar_Vectores_de_Cargas();
}

/// Carga la densidad obtenida por la FEM en los objetos Nodos de la geometria
void FEM::Densidad_FEM(void)
{
GeometriaTrabajo->Carga_Densidad_Nodal_Vector(X);	
}

/// Calculo de densidades a partir de una grilla de puntos una vez ya realizada una aproximacion nodal por el metodo FEM 
void FEM::Ingreso_Datos_Grilla_FEM(Vector *dominio,Vector_Entero *Resolucion,int nProcesos)
{
 int rank,size;
 int gFila,gColumna;
 MPI_Comm_rank(MPI_COMM_WORLD,&rank);	
 MPI_Comm_size(MPI_COMM_WORLD, &size);

	if(rank==0)
	{  
		if(size>1)
		{	
		Proceso_Paralelo->Almacenar_Nodos_Geometria(GeometriaTrabajo->Retornar_Nodos_Geometria(),GeometriaTrabajo->Retorna_Numero_Nodos());
		GeometriaTrabajo->Calcula_Particiones_Grilla(nProcesos);	
		printf("FILA = %d \n",GeometriaTrabajo->Get_FilaGrilla());	
		printf("COLUMNA = %d \n",GeometriaTrabajo->Get_ColumnaGrilla());
		gFila=GeometriaTrabajo->Get_FilaGrilla();
		gColumna=GeometriaTrabajo->Get_ColumnaGrilla();
		}	
		GeometriaTrabajo->Set_ResolucionX((int)Resolucion->Retorna(0));	
		GeometriaTrabajo->Set_ResolucionY((int)Resolucion->Retorna(1));		
		GeometriaTrabajo->Set_NumeroPuntosGrilla(Resolucion->Retorna(0)*Resolucion->Retorna(1));
	}
	
	if(size>1)
	{	
						Tiempo_Inicial_Comunicacion_Grilla=MPI_Wtime();
						MPI_Bcast(&gFila,1,MPI_INT,0,MPI_COMM_WORLD);
						MPI_Bcast(&gColumna,1,MPI_INT,0,MPI_COMM_WORLD);
						Tiempo_Parcial_Comunicacion_Grilla+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_Grilla;
						
						Vector_Entero *Particion_Geometria_Fila = new Vector_Entero(gFila,"Particion_Geometria_Fila");
						Vector_Entero *Particion_Geometria_Columna = new Vector_Entero(gColumna,"Particion_Geometria_Columna");
						Matriz_Entera *Dim_Fila_Columna =  new Matriz_Entera(2,size);
						Matriz_Entera *Bandera_Fila_Columna =  new Matriz_Entera(2,size);
						
					
					Proceso_Paralelo->Obtener_Vector_Particion(gFila,Resolucion->Retorna(1),Particion_Geometria_Fila);	
					Proceso_Paralelo->Obtener_Vector_Particion(gColumna,Resolucion->Retorna(0),Particion_Geometria_Columna);	
					
					unsigned long long sumaC;
					int contador=0;
					for(unsigned long long i=0;i< Particion_Geometria_Fila->Columnas();i++)
					{
						sumaC=0;
						for(unsigned long long j=0;j< Particion_Geometria_Columna->Columnas();j++)
						{
						
							Dim_Fila_Columna->Asigna(0,contador,Particion_Geometria_Fila->Retorna(i));
							Dim_Fila_Columna->Asigna(1,contador,Particion_Geometria_Columna->Retorna(j));
							Bandera_Fila_Columna->Asigna(0,contador,i);
							Bandera_Fila_Columna->Asigna(1,contador,j);
							contador++;
						}		
						
						
					}	
					Proceso_Paralelo->Almacena_Matriz_FC(Dim_Fila_Columna);
					Proceso_Paralelo->Almacena_Matriz_MP(Bandera_Fila_Columna);
					Proceso_Paralelo->Reserva_Memoria_Puntos(Dim_Fila_Columna->Retorna(0,rank)*Dim_Fila_Columna->Retorna(1,rank));
					Proceso_Paralelo->Reserva_Memoria_Centroides(2);
					GeometriaTrabajo->Generar_Puntos_Grilla_FEM(dominio,Resolucion,Dim_Fila_Columna,Bandera_Fila_Columna,Proceso_Paralelo->Retorna_PuntosX(),Proceso_Paralelo->Retorna_PuntosY(),Proceso_Paralelo->Retorna_Centroides(),rank);
		
		}
		else
		{
			
			GeometriaTrabajo->Generar_Grilla_Memoria(dominio,Resolucion);
		
		}
}

/// Aproximacion nodal a partir de una grilla de puntos 
void FEM::Calculo_Grilla_FEM()
{
	 int size,rank;
     MPI_Comm_rank(MPI_COMM_WORLD,&rank);	
	 MPI_Comm_size(MPI_COMM_WORLD, &size);
	 MPI_Status status;
	 double minimo,Diferencial_Area;
	 double distancia_centroide,x1,x2,y1,y2;
     unsigned long long  total,fLocal,cLocal,fGlobal,cGlobal;
	 unsigned long long  dimtmp; 
	 unsigned long long  nElementos,nNodosTMP,contador,bFila,bColumna,pMinimo;
     unsigned long long  desplazamiento=0; 
	 unsigned long long  *Puntos_Dominio=new unsigned long long [1];
	 unsigned long long *dimensiones = new unsigned long long[1];
	 unsigned long long *dimensiones_llegada= new unsigned long long[1];
     Vector *DistanciaTmp = new Vector(2);
	 Vector_Entero *Numero_Nodos_Dominio=new Vector_Entero(size);
     Vector *DistanciasGenerales = new Vector(size*2);    
	 Vector *Densidad_FEM_TMP;
	 Vector *DENSIDADES_GRILLA;
	 Vector *dGrilla;
     Vector_Entero *PerteneciaCentroides;

	if(rank==0)
	{
		Diferencial_Area =   GeometriaTrabajo->Calculo_Area_Diferencial();
		total= GeometriaTrabajo->Get_NumeroPuntosGrilla();
	}
	
	if(size>1)
	{	
	Tiempo_Inicial_Comunicacion_Grilla=MPI_Wtime();
	MPI_Bcast(&total,1,MPI_UNSIGNED_LONG_LONG,0,MPI_COMM_WORLD);	
    MPI_Bcast(&Diferencial_Area,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	Tiempo_Parcial_Comunicacion_Grilla+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_Grilla;
    }
	
	dimtmp = total/ (unsigned long long) size;
	DENSIDADES_GRILLA= new Vector(total,"Densidades");
	
	if(size>1)
	dGrilla=new Vector(Proceso_Paralelo->Retorna_FC()->Retorna(0,rank)*Proceso_Paralelo->Retorna_FC()->Retorna(1,rank),"GRILLA");	
    else
    dGrilla=new Vector(total,"GRILLA");	
    		

	
    if(rank==0)
	{
     nElementos =  GeometriaTrabajo->Retorna_Numero_Elementos();
	 nNodosTMP =  GeometriaTrabajo->Retorna_Numero_Nodos();
	}
	
	if(size>1)
	{	
					Tiempo_Inicial_Comunicacion_Grilla=MPI_Wtime();
					MPI_Bcast(&nElementos,1,MPI_UNSIGNED_LONG_LONG,0,MPI_COMM_WORLD);	
					MPI_Bcast(&nNodosTMP,1,MPI_UNSIGNED_LONG_LONG,0,MPI_COMM_WORLD);
					Tiempo_Parcial_Comunicacion_Grilla+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_Grilla;
				
				if(rank!=0)
				{
					Proceso_Paralelo->Reserva_Memoria_Nodos(nNodosTMP);
				}
			
				
				if(rank==0)
				{
						Tiempo_Inicial_Comunicacion_Grilla=MPI_Wtime();
						for(int i=0;i<size;i++)		
						{
						Proceso_Paralelo->Envio_Vector_Double(i,14,0,nNodosTMP,Proceso_Paralelo->Retorna_NodosX()->Retorna_Puntero_Datos());
						Proceso_Paralelo->Envio_Vector_Double(i,13,0,nNodosTMP,Proceso_Paralelo->Retorna_NodosY()->Retorna_Puntero_Datos());
						Proceso_Paralelo->Envio_Vector_Double(i,12,0,nElementos*17,Proceso_Paralelo->Retorna_Datos_Elementos_Vector()->Retorna_Puntero_Datos());
						}
				
				}
				
				Proceso_Paralelo->Recibe_Vector_Double(0,14,nNodosTMP,Proceso_Paralelo->Retorna_NodosX()->Retorna_Puntero_Datos(),status);
				Proceso_Paralelo->Recibe_Vector_Double(0,13,nNodosTMP,Proceso_Paralelo->Retorna_NodosY()->Retorna_Puntero_Datos(),status);
				Proceso_Paralelo->Recibe_Vector_Double(0,12,nElementos*17,Proceso_Paralelo->Retorna_Datos_Elementos_Vector()->Retorna_Puntero_Datos(),status);
				Tiempo_Parcial_Comunicacion_Grilla+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_Grilla;
				
				PerteneciaCentroides = new Vector_Entero(nNodosTMP);
				
				Tiempo_Inicial_Comunicacion_Grilla=MPI_Wtime();
				for(int i=0;i<size;i++)
				{	
					Proceso_Paralelo->Envio_Vector_Double(i,25,0,2,Proceso_Paralelo->Retorna_Centroides()->Retorna_Puntero_Datos());
				}
				
				for(int i=0;i<size;i++)
				{	
					Proceso_Paralelo->Recibe_Vector_Double(i,25,2,DistanciaTmp->Retorna_Puntero_Datos(),status);
					DistanciasGenerales->Asigna(i*2,DistanciaTmp->Retorna(0)); 
					DistanciasGenerales->Asigna(i*2+1,DistanciaTmp->Retorna(1)); 
				}
				
				Tiempo_Parcial_Comunicacion_Grilla+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_Grilla;
				
				for(unsigned long long i=0;i<nNodosTMP;i++)
				{		
							for(unsigned long long j=0;j<  (unsigned long long) size;j++)
							{	
							x1=Proceso_Paralelo->Retorna_NodosX()->Retorna(i);
							y1=Proceso_Paralelo->Retorna_NodosY()->Retorna(i);
							x2=DistanciasGenerales->Retorna(j*2);
							y2=DistanciasGenerales->Retorna(j*2+1);		
							
							distancia_centroide = GeometriaTrabajo->Distancia_Dos_Puntos(x1,x2,y1,y2);
							
								if(j==0)
								minimo = distancia_centroide;	
									
									if(distancia_centroide <= minimo)
									{
									minimo = distancia_centroide;
									pMinimo = j;					
									}	
						   }
			
				PerteneciaCentroides->Asigna(i,pMinimo);		   
						   
			   }	

   GeometriaTrabajo->Calcula_Densidades_Puntos_Grilla(rank,PerteneciaCentroides,nElementos,dGrilla,Proceso_Paralelo->Retorna_Datos_Elementos_Vector()->Retorna_Puntero_Datos(),Proceso_Paralelo->Retorna_PuntosX()->Retorna_Puntero_Datos(),Proceso_Paralelo->Retorna_PuntosY()->Retorna_Puntero_Datos(),Proceso_Paralelo->Retorna_NodosX()->Retorna_Puntero_Datos(),Proceso_Paralelo->Retorna_NodosY()->Retorna_Puntero_Datos(),Puntos_Dominio); 
   MPI_Gather(Puntos_Dominio,1,MPI_UNSIGNED_LONG_LONG,Numero_Nodos_Dominio->Retorna_Puntero_Datos(),1,MPI_UNSIGNED_LONG_LONG,0,MPI_COMM_WORLD);
  
	dimensiones[0]=dGrilla->Columnas();
    Tiempo_Inicial_Comunicacion_Grilla=MPI_Wtime();
	Proceso_Paralelo->Envio_Dato_Entero(0,19,dimensiones[0]);
    Proceso_Paralelo->Envio_Vector_Double(0,20,0,dimensiones[0],dGrilla->Retorna_Puntero_Datos());
	Tiempo_Parcial_Comunicacion_Grilla+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_Grilla;
    
   
	 if(rank==0)
	 {	
		 fGlobal = GeometriaTrabajo->Get_ResolucionX();
		 cGlobal = GeometriaTrabajo->Get_ResolucionY();

		 for(int i=0;i<size;i++)
		 {
       	     Tiempo_Inicial_Comunicacion_Grilla=MPI_Wtime();
     		 Proceso_Paralelo->Recibe_Dato_Entero(i,19,dimensiones_llegada);
			 Densidad_FEM_TMP = new Vector(dimensiones_llegada[0]);
			 Proceso_Paralelo->Recibe_Vector_Double(i,20,dimensiones_llegada[0],Densidad_FEM_TMP->Retorna_Puntero_Datos(),status);
	         Tiempo_Parcial_Comunicacion_Grilla+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_Grilla;
 		 			 
			 fLocal = Proceso_Paralelo->Retorna_FC()->Retorna(0,i);
			 cLocal = Proceso_Paralelo->Retorna_FC()->Retorna(1,i);  
			 bFila = Proceso_Paralelo->Retorna_MP()->Retorna(0,i);
			 bColumna = Proceso_Paralelo->Retorna_MP()->Retorna(1,i);			 
		 
			 contador =0;			 
			 desplazamiento=0;			 
	
					 for(unsigned long long i=0;i<fLocal ;i++)
					 for(unsigned long long j=0;j<cLocal ;j++)	 
					 {
						 desplazamiento =  (i   +    Proceso_Paralelo->Retorna_FC()->Retorna(0,0)*bFila   )*(fGlobal  )  +  j  +  Proceso_Paralelo->Retorna_FC()->Retorna(1,0)*bColumna;
						 DENSIDADES_GRILLA->Asigna(desplazamiento, Densidad_FEM_TMP->Retorna(contador) );
						 contador++;	 
					 }
	
		   }
					
     }
	
	if(rank==0)
    Tiempo_Inicial_Comunicacion_Grilla=MPI_Wtime();
	
    Puntos_Dominio[0]=Numero_Nodos_Dominio->Sumatoria_Vector();
    }
     else
	 {
		 GeometriaTrabajo->Calcula_Densidades_Puntos(DENSIDADES_GRILLA,Puntos_Dominio);
		 
	 }	 

	
	if(size>1) 
	MPI_Barrier(MPI_COMM_WORLD);
   
    if(rank==0)
	{
		
	if(size>1)
	{	
    Tiempo_Parcial_Comunicacion_Grilla+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_Grilla;
    printf("\n Tiempo en comunicacion en grilla es: %f \n",Tiempo_Parcial_Comunicacion_Grilla);
	tTotalComunicacion+=Tiempo_Parcial_Comunicacion_Grilla;	
	tTotalComunicacionGrilla=Tiempo_Parcial_Comunicacion_Grilla;	
	}
	
		double Densidad_Total_Tmp =0.0;
		double dNodal,escala;
		escala = (double) (nNodosTMP) /  Puntos_Dominio[0];
	    double Unidad_Area = GeometriaTrabajo->Area_Global()  /  Puntos_Dominio[0];
		printf("\nDiferencial de Area: %f  Nodos Origen = %Ld... Suma nodos internos =%Ld\n",Diferencial_Area, nNodosTMP,Puntos_Dominio[0]);
	    printf("\n Area dominio: %f. Unidad de Area por nodo es de (%f) mm^2 ..suma = %Ld\n", GeometriaTrabajo->Area_Global()  , Unidad_Area, Puntos_Dominio[0]);
	
		Archivo *Grilla=new Archivo();
		Grilla->SetNombre(GetNombreArchivoGrilla());
		Grilla->SetModo(GRABA_ARCHIVO);	
		Grilla->Abrir();
	
		for(unsigned long long  i=0;i<DENSIDADES_GRILLA->Columnas();i++)
		{
			dNodal=DENSIDADES_GRILLA->Retorna(i);//*escala;
			Densidad_Total_Tmp+=dNodal;
			fprintf(Grilla->GetArchivo(), "%f\n" ,dNodal);

		}
			Grilla->Cerrar();
	
		
		printf("\nEl numero estimado de fotones en el dominio es de: %f...\n",Densidad_Total_Tmp);	
	}

	
}


/// Establece condicional de la solucion matricial 1-Matrices normales, 2-Matrices reducidas
 void FEM::Establecer_Condicion_Matricial_FEM(int condicion,int metodo)
{
	unsigned long long  indice,ubicacion,DimOriginal,DimFrontera,DimensionNueva,DimReducida;
	DimOriginal=B->Columnas();
	Vector *Tmp = new Vector(DimOriginal);
	Set_orden_nodal(DimOriginal);
	Set_tipo_solucion_matricial(condicion);
	
	if(condicion==0)
	{
		printf("Como no definio que condicion de borde quiere utilizar el programa no continuara su ejecucion...\n");
		exit(1);
	}
	
	if(condicion==1)
	{
		
		Sistema->Set_nDesconocido(DimOriginal);
		printf("Se trabajara con la dimension original en el sistema de ecuaciones\n");
	}
	
	if(condicion==2) 
	{
		printf("Se trabajara con una dimension menor en el sistema de ecuaciones\n");
		DimFrontera=Get_Geometria2D_Triangulos()->Get_Numero_Nodos_Frontera_Dominio_Global();
		DimensionNueva=DimOriginal-DimFrontera;
		Sistema->Set_nDesconocido(DimensionNueva);
		DimReducida = DimOriginal - DimFrontera;
	    
		if(metodo == LSQR_DISPERSO)
		{	
						std::map<unsigned long long  ,unsigned long   long>::iterator posicion;
						std::map<unsigned long long  ,double>::iterator indice_nuevo;
						map<unsigned long   long,unsigned long   long> Nuevos_Indices; 
						map<unsigned long   long,unsigned long   long> Nuevos_Indices_Fila; 
						map<unsigned long  long ,unsigned long   long> Nuevos_Indices_Columna;  
					
						for(unsigned long long   cont = 0;cont<DimOriginal;cont++)
						{
							Mapeo_FEM_Dirichlet[cont]=cont;
						}
									  
						for(unsigned long long   cont = 0;cont<DimFrontera;cont++)
						{
							Mapeo_FEM_Dirichlet.erase(Get_Geometria2D_Triangulos()->Get_Nodos_Frontera_Geometria()->Retorna(cont));
						}
						
						B->Copia(Tmp);
						B->Libera_Memoria();
						B->Solicita_Memoria(DimReducida,0.0);
						indice=0;
				
						for(posicion = Mapeo_FEM_Dirichlet.begin(); posicion != Mapeo_FEM_Dirichlet.end();)
						{
							B->Asigna(indice,Tmp->Retorna(  posicion->second ) );
							Nuevos_Indices[posicion->second]=indice;
							++posicion;
							indice++;
						}
						
						indice=0;
						unsigned long long tmp;
						/// Elimina la informacion de tipo map de columnas, filas y datos, a partir del vector de entrada donde se indica que filas - columnas se quieren eliminar de la matriz
						for (posicion = Mapeo_FEM_Fila.begin(); posicion != Mapeo_FEM_Fila.end();) // recorre el mapeo de las filas
						{
							ubicacion =  posicion->second;  
								if(Mapeo_FEM_Dirichlet[ubicacion])
								{
								 tmp =	Nuevos_Indices[ubicacion];
								 Mapeo_FEM_Fila[posicion->first]=tmp;                                                       
								++posicion;
								}
								else
								{	
									ubicacion = posicion->first;
									Mapeo_FEM_Columna.erase(ubicacion);                                                        
									Matriz_A_FEM.erase(ubicacion);                                                              
									Mapeo_FEM_Fila.erase(posicion++); 
								}
								indice++;
						}		
				
						indice =0;
						for (posicion = Mapeo_FEM_Columna.begin(); posicion != Mapeo_FEM_Columna.end();) // recorre el mapeo de las columnas
						{
							ubicacion =  posicion->second;  
				
							if(Mapeo_FEM_Dirichlet[ubicacion])
							{
								
								 tmp =	Nuevos_Indices[ubicacion];
								 Mapeo_FEM_Columna[posicion->first]=tmp;                                              
								++posicion;
							}
								else
								{	
								ubicacion = posicion->first;	
								Mapeo_FEM_Fila.erase(ubicacion);                                                        
								Matriz_A_FEM.erase(ubicacion);                                                            
								Mapeo_FEM_Columna.erase(posicion++);   
								}
								indice++;
						}		
						
    	   }
		   else
		   {
				   Sistema->Redimensionar_Sistema(A,B,Get_Geometria2D_Triangulos()->Get_Nodos_Frontera_Geometria(),DimFrontera);   	
           }
		

		
		}
}

/// Resuelve el sistema de ecuaciones con un metodo especifico:  Jacobi / Gauss - Seidel / Factorizacion LU / Factorizacion Cholesky / LSQR (Matriz Completa) / LSQR (Matriz Dispersa en notacion vectorial)  (Secuencial - Paralelo)
void FEM::Resolver_Sistema_FEM(int metodo,int traza, unsigned  long long iteraciones,int Tipo_Parada)
  {
	 double finFactorizacion,iniT,finT,iniTiempo;  
	 int size,rank,i;
     int verificacion_dDominante;
	 MPI_Status status;	
     MPI_Comm_rank(MPI_COMM_WORLD,&rank);	
	 MPI_Comm_size(MPI_COMM_WORLD, &size);
     Proceso_Paralelo=new Rutinas_Paralelas(); 
	 Sistema->Set_Traza(traza); 
	 unsigned long  long Incognitas=Sistema->Get_nDesconocido();
	 Sistema->Set_Iteraciones(iteraciones);
	
	 X=new Vector(Incognitas,"Vector densidades X");
	 if(rank==0) 
	 {	 
  	    
		  iniTiempo=MPI_Wtime();
      }

   if(metodo == JACOBI)	  
   {
	   
	   if(size==1)
	   {   
	   verificacion_dDominante=A->Dominante(); 
       if(verificacion_dDominante == 1) 	  
	   {
		  printf("Matriz Diagonalmente dominate...\n");

		   Sistema->Resuelve_Jacobi(A,X,B,0.00001,Tipo_Parada,Sistema->Get_Traza());  // 1 iter 0 tol
	   }
	   else
	   {
		   printf("Debe utilizar otro metodo para este tipo de Matriz\n");
	      
		}	   
		   finT=MPI_Wtime();
	   printf("El tiempo usado para resolver mediante Jacobi fue de %f segundos\n",finT-iniTiempo);
	   printf("-- Fin resolver sistema--- \n");
	   }
	   else 
	   {
		   printf("ESTE METODO NO ESTA PARA CALCULO PARALELO\n");
		   MPI_Finalize();	
		   exit(1);
	   }
	   
  }
     if(metodo == GAUSS_SEIDEL)	  
   {
	   
	   if(size==1)
	   {   
	   verificacion_dDominante=A->Dominante(); 
       if(verificacion_dDominante == 1) 	  
	   {
		  printf("Matriz Diagonalmente dominate...\n");
		   Sistema->Resuelve_Gauss_Seidel(A,X,B, 0.00001,Tipo_Parada,Sistema->Get_Traza()); 
       }
	   else
	   {
		   printf("Debe utilizar otro metodo para este tipo de Matriz\n");
	      
		}	     
		   finT=MPI_Wtime();
	   printf("El tiempo usado para resolver mediante Gauss - Seidel fue de %f segundos\n",finT-iniTiempo);
	   printf("-- Fin resolver sistema--- \n");
	   }
	   else 
	   {
		   printf("ESTE METODO NO ESTA PARA CALCULO PARALELO\n");
		   exit(1);
	   }
	   
  }
   if(metodo == LU)
   {
	   
	    if(size==1)
	   {   
	   Sistema->Factoriza_LU(A);
	   finFactorizacion=MPI_Wtime();
	   iniT=MPI_Wtime();
	   Sistema->Solve_LU(A,X,B);   
	   finT=MPI_Wtime();
	   printf("El tiempo LU fue %f segundos\n",finFactorizacion-iniTiempo);
	   printf("El tiempo Resover  LU fue %f segundos\n",finT-iniT);
	   printf("El porcentaje de tiempo LU fue de %f \n",100.0*(finFactorizacion-iniTiempo)/(finT-iniTiempo));	
	   printf("El porcentaje de tiempo Solve LU fue de %f \n",100.0*(finT-iniT)/(finT-iniTiempo));	
	   printf("-- Fin resolver sistema--- ");
	   	   }
	   else 
	   {
		   printf("ESTE METODO NO ESTA PARA CALCULO PARALELO\n");
		   MPI_Finalize();
		   exit(1);
	   }
	}
   if(metodo == LLT)
   {
	   
	   
	    if(size==1)
	   {   
				if(Get_tipo_solucion_matricial()!=2)   
				{   
				Sistema->Factoriza_LLT(A);
				finFactorizacion=MPI_Wtime();
				iniT=MPI_Wtime();
				Sistema->Solve_LLT(A, X, B);
				finT=MPI_Wtime();
				printf("El tiempo LLT fue %f segundos\n",finFactorizacion-iniTiempo);
				printf("El tiempo Resover  LLT fue %f segundos\n",finT-iniT);
				printf("El porcentaje de tiempo LLT fue de %f \n",100.0*(finFactorizacion-iniTiempo)/(finT-iniTiempo));	
				printf("El porcentaje de tiempo Solve LLT fue de %f \n",100.0*(finT-iniT)/(finT-iniTiempo));	
				printf("-- Fin resolver sistema-- ");
				}
                else
                {
					printf(" \n ***** ATENCION ***** \n");
				    printf("En el metodo de resolucion LLT para condicion deborde de tipo Dirichlet = 0, la matriz reducida no es una matriz definida positiva \n");
				    printf("y se requiere un proceso de factorizacion extra que realize modificaciones a la matriz reducida, el cual no esta implemento en este programa\n");
					printf("por lo cual el metodo de resolucion LLT para condicion de borde de tipo Dirichlet = 0 no esta disponible \n");
                    MPI_Finalize();	
					exit(1);
				}					
       }
	   else 
	   {
		   printf("ESTE METODO NO ESTA PARA CALCULO PARALELO\n");
		   MPI_Finalize();	
		   exit(1);
	   }
	}

	   if(metodo == LSQR)
   {
	    // Tipo_Parada por defecto 0 en base a la tolerancia , 1 = Numero de iteraciones fijo (utilizado en graficos de convergencia para comparacion de metodos iterativos)  
	    if(size==1)
	   {   
	    iniT=MPI_Wtime();
		Sistema->LSQR_Resolver_Normal(A,X,B,Tipo_Parada,Sistema->Get_Traza());
    	finT=MPI_Wtime();
        printf("El tiempo Resover  LSQR fue %f segundos\n",finT-iniT);
	    printf("-- Fin resolver sistema-- ");
		}
	   else 
	   {
		   printf("ESTE METODO NO ESTA PARA CALCULO PARALELO\n");
		   MPI_Finalize();	
		   exit(1);
	   }
	}

       if(metodo == LSQR_DISPERSO )
   {
	 // Tipo_Parada por defecto 0 en base a la tolerancia , 1 = Numero de iteraciones fijo (utilizado en graficos de convergencia para comparacion de metodos iterativos)  
	 int TAG_B=0;
	 int TAG_DImX=1;
	 int TAG_Indices=2;
	 int TAG_Columnas=3;
	 int TAG_Datos=4; 
	 int TAG_Particion=5;
	 int TAG_nDatos=6;  
	 unsigned long  long *dimX=new unsigned long  long[1];
	 unsigned long  long *iParticion=new unsigned  long long[1]; 
	 unsigned long  long *nColumnas=new unsigned  long long[1];  
     unsigned long long  largo_vector;
	 Matriz_Dispersa *MDA;  
	 Vector_Entero *vParticion=new Vector_Entero((unsigned long long)size);
     Vector_Entero *INDICES;
     Vector_Entero *COLUMNAS;
	 Vector *DATOS;
		   
	
	   if(rank==0)
	   {	   
	   	printf("Generando Matriz dispersa.....");
		MDA = new Matriz_Dispersa(Matriz_A_FEM,B->Columnas(),Mapeo_FEM_Fila,Mapeo_FEM_Columna);
	    finFactorizacion=MPI_Wtime();
        iniT=MPI_Wtime();
 	    Sistema->SetNP(GetProcesadores()); // Se indica al sistema lineal cuantos procesadores va a utilizar BORRAR PERO REVISAR
		
		        // Si es calculo paralelo se realiza el proceso de paso de mensajes
				if(size>1)
				{
					
							unsigned long  long contador=0;	
						    Proceso_Paralelo->Obtener_Vector_Particion(size,MDA->vIndiceFila->Columnas(),vParticion);
				            Sistema->Asigna_Indice_FIla(vParticion);
			                unsigned long  long desplazamiento=0;
					        Tiempo_Inicial_Comunicacion_FEM=MPI_Wtime();
								for( i=0;i<size;i++) // Solo envio a los otros procesos, el root ya tiene la informacion, una alternativa es crear PACK, pero no consume mayores recursos esta solucion
								{	
									Proceso_Paralelo->Envio_Dato_Entero(i,TAG_DImX,X->Columnas()); 
									Proceso_Paralelo->Envio_Vector_Double(i,TAG_B,0,B->Columnas(),B->Retorna_Puntero_Datos()); 
				                    
                                    if(i!=size-1)
                                    {										
                					Proceso_Paralelo->Envio_Dato_Entero(i,TAG_Particion,vParticion->Retorna(i)+1);  
									Proceso_Paralelo->Envio_Vector_Entero(i,TAG_Indices,contador,vParticion->Retorna(i)+1,MDA->vIndiceFila->Retorna_Puntero_Datos()); 
							    	}
										else
										{
										Proceso_Paralelo->Envio_Dato_Entero(i,TAG_Particion,vParticion->Retorna(i));  
										Proceso_Paralelo->Envio_Vector_Entero(i,TAG_Indices,contador,vParticion->Retorna(i),MDA->vIndiceFila->Retorna_Puntero_Datos()); 
										}	
									
									desplazamiento=MDA->vIndiceFila->Retorna(contador);
								
									contador=contador+vParticion->Retorna(i);
									
							
									if(i==(size-1))
									{	
										largo_vector=MDA->vIndiceFila->Retorna(contador-1)  - desplazamiento;	
										
									}
										else
										{
											largo_vector=MDA->vIndiceFila->Retorna(contador)  - desplazamiento;	
								      	}
										
                                     	Proceso_Paralelo->Envio_Dato_Entero(i,TAG_nDatos,largo_vector);  
										Proceso_Paralelo->Envio_Vector_Entero(i,TAG_Columnas,desplazamiento,largo_vector,MDA->vColumna->Retorna_Puntero_Datos()); //Paquete 2
							            Proceso_Paralelo->Envio_Vector_Double(i,TAG_Datos,desplazamiento,largo_vector,MDA->vDato->Retorna_Puntero_Datos()); //Paquete 2
	
								}
				         
				}
		   
	   }
	    Tiempo_Parcial_Comunicacion_FEM=MPI_Wtime() - Tiempo_Inicial_Comunicacion_FEM;
	   //Si es calculo paralelo se realiza el proceso de recepcion de mensajes
	   if(size>1)
	   {   Tiempo_Inicial_Comunicacion_FEM=MPI_Wtime();
	    	Proceso_Paralelo->Recibe_Dato_Entero(0,TAG_DImX,dimX); 
		    Tiempo_Parcial_Comunicacion_FEM+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_FEM;
		    B=new Vector(dimX[0]);
		    X=new Vector(dimX[0]);
		    Tiempo_Inicial_Comunicacion_FEM=MPI_Wtime();
			Proceso_Paralelo->Recibe_Vector_Double(0,TAG_B,dimX[0],B->Retorna_Puntero_Datos(),status); 
            Proceso_Paralelo->Recibe_Dato_Entero(0,TAG_Particion,iParticion); 
		    Tiempo_Parcial_Comunicacion_FEM+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_FEM;
		    INDICES=new Vector_Entero(iParticion[0],"INDICES");
		    Tiempo_Inicial_Comunicacion_FEM=MPI_Wtime();
		    Proceso_Paralelo->Recibe_Vector_Entero(0,TAG_Indices,iParticion[0],INDICES->Retorna_Puntero_Datos(),status);
            Proceso_Paralelo->Recibe_Dato_Entero(0,TAG_nDatos,nColumnas); 
		    Tiempo_Parcial_Comunicacion_FEM+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_FEM;
		    DATOS=new Vector(nColumnas[0],"DATOS");
		    Tiempo_Inicial_Comunicacion_FEM=MPI_Wtime();
		    COLUMNAS=new Vector_Entero(nColumnas[0],"COLUMNAS");
		    Proceso_Paralelo->Recibe_Vector_Entero(0,TAG_Columnas,nColumnas[0],COLUMNAS->Retorna_Puntero_Datos(),status); 
		    Proceso_Paralelo->Recibe_Vector_Double(0,TAG_Datos,nColumnas[0],DATOS->Retorna_Puntero_Datos(),status); 
            Tiempo_Parcial_Comunicacion_FEM+=MPI_Wtime() - Tiempo_Inicial_Comunicacion_FEM;
		}			
		
		
		
		
		
        // Si el calculo paraleleo se utilizan los vectores particionados y procesos de comunicacion, recoleccion y calculo paralelo
	    if(size>1)
		{
			iniT=MPI_Wtime();
			Sistema->LSQR_Resolver_Paralelo(X,B,iParticion,COLUMNAS,INDICES,DATOS,Tipo_Parada,traza);	
        }
		// Calculo secuencial
		else
		{  iniT=MPI_Wtime();
			Sistema->LSQR_Resolver(MDA,X,B,Tipo_Parada,Sistema->Get_Traza());
		}

		
        if(rank==0)
		{	
		
		finT=MPI_Wtime();
	
		if(size==1)
		{
			Tiempo_Parcial_Comunicacion_FEM=0.0;
		}	
		
		
		printf("El tiempo en el pre- procesamiento de la comunicacionfue de %f segundos\n",Tiempo_Parcial_Comunicacion_FEM);	
		printf("El tiempo utilizado en comunicacion en LSQR fue de:  %f segundos\n",Sistema->tSistema);	
		printf("El tiempo total de comunicacion de la aproximacion Fem fue de %f segundos\n",Sistema->tSistema+Tiempo_Parcial_Comunicacion_FEM);	
		printf("El tiempo Resover  LSQR fue %f segundos\n",finT-iniT);
	    printf("-- Fin resolver sistema-- \n");
		tTotalComunicacion+=Sistema->tSistema+Tiempo_Parcial_Comunicacion_FEM;		
        tTotalComunicacionFEM=Sistema->tSistema+Tiempo_Parcial_Comunicacion_FEM;		
	    }
			
			
}



	if(rank==0)	
	{
		
		double suma_fotones_fuentes=0.0;
		double factor;
		
		for(unsigned long long i=0;i < B->Columnas();i++)
		suma_fotones_fuentes+=B->Retorna(i);	
		
		// SE DEBE PROFUNDIZAR EN EL AREA EFECTIVA DE CADA NODO Y UTILIZAR VALORES DE FLUENCIA CON DATOS REALES Y NO EXPERIMENTALES
		//factor = 1.0/(Get_sCarga()*GeometriaTrabajo->Retorna_Numero_Nodos()); // En caso de distribuir la carga de fotonos NO con valores experimentales y normalizados segun numero de fotones en la fuente
		factor = 1.0/(GeometriaTrabajo->Retorna_Numero_Nodos());  // Valores cuantificables de fotones por unidad de area, mientras mayor resolucion menos fotones por unidad de area, ya que el area de flujo es menor

                // El error implicito en este factor es eliminado al momento de calcular la grilla de densidades donde la unidad de area es contanste en todo el dominio al ser una grilla de NxN
		X->Multiplica( factor );
		printf("Factor escala = %f \n Fluencia de Fotones totales en fuentes =%f (1/Cm^2) \n Magnitud del total de fotones a estimar en fuentes =%f Fotones\n",factor,suma_fotones_fuentes,Get_sCarga());
		
		/// Almacenamiento de solucion a archivo
		Archivo *Sol=new Archivo();
		Sol->SetNombre(GetNombreArchivoX());
		Sol->SetModo(GRABA_ARCHIVO);	
		Sol->Abrir();
		
			

			if(Get_tipo_solucion_matricial()==1)
			{	
				
			 printf("Generando archivo solucion\nArea dominio = %f Cm^2 \n Nodos = %ld \nElementos=%ld ",GeometriaTrabajo->Area_Global(),GeometriaTrabajo->Retorna_Numero_Nodos(),GeometriaTrabajo->Retorna_Numero_Elementos());
			 Sol->Escribe_Vector(X,X->Columnas());   
		
			}
		
			// Ensamble solucion reducida  
			if(Get_tipo_solucion_matricial()==2)
			{
				printf("Ensamble de solucion reducida y generando archivo solucion...\n");
				Vector *SolucionTMP=new Vector(Get_orden_nodal());
					
					unsigned long  long cont=0;
					for(unsigned long  long i=0;i<Get_orden_nodal();i++)
					{
						if(Get_Geometria2D_Triangulos()->Get_Nodos_Frontera_Geometria()->Buscar_Valor(i)==1)
						SolucionTMP->Asigna(i,0.0);
						else
						{	
						SolucionTMP->Asigna(i,X->Retorna(cont));
						cont++;
						}
					}
					
					X =  new Vector(Get_orden_nodal());
					
					for(unsigned long  long i=0;i<SolucionTMP->Columnas();i++)
					{
						
						X->Asigna(i,SolucionTMP->Retorna(i));
					}
					Sol->Escribe_Vector(SolucionTMP,SolucionTMP->Columnas());   
		
					printf("Area del dominio= %f Cm^2\n Nodos = %ld Elementos=%ld\n",GeometriaTrabajo->Area_Global(),GeometriaTrabajo->Retorna_Numero_Nodos(),GeometriaTrabajo->Retorna_Numero_Elementos());
						
			}
		
	 Sol->Cerrar(); 

	}
	
	
}
