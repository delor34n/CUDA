#include "Archivo.hpp" 
#include "FEM2D.hpp"

///Programa principal -  Soluciona la ecuacion diferencial de transporte de la luz en medios difusos con elementos fiinitos triangulares lineales de 3 nodos
int main(int argc, char *argv[])
{
int myrank, size;
int c;

double inicio,final;
double inicioGrilla,finalGrilla;	
double inicioABX,finABX;
double inicioCargaDatos,finCargaDatos;
double inicioCalculoFEM,finCalculoFEM,tiempoComunicacionFEM;
int condicion_frontera,r;
int Tipo_resolucion_AX,Traza,Tipo_Parada,Calculo_Grilla;	
int Iteraciones=0;
// Utilizadas en el calculo  paralelo y secuencial de grilla
Vector *Dominio = new Vector(4);
Vector_Entero *Resolucion = new Vector_Entero(2);	
char *strArchivoNodosFilename = NULL;      // Nombre de archivo de Nodos
char *strArchivoElementosFilename = NULL;       // Nombre de archivo de Elementos
char *strArchivoFronterasFilename = NULL;
char *strArchivoAbsorcionFilename = NULL;
char *strArchivoScatteringFilename = NULL;
char *strArchivoFuentesFilename = NULL;
char *strArchivoGrillaFilename = NULL;	
char *strArchivoX = NULL;
char *strArchivoTiempos = NULL;
double Fase;
tiempoComunicacionFEM=0.0;

FEM2D *ProblemaFEM = new FEM2D() ;	
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
MPI_Comm_size(MPI_COMM_WORLD, &size);
MPI_Status status;

if (myrank == 0)
{
	
	inicio=MPI_Wtime();
			
		while ((c = getopt (argc, argv, "t:g:f:B:G:P:I:F:M:Q:N:E:T:A:S:X:i:j:k:l:r:")) != -1)
		{
					switch (c)
					{
						case 'g':
						Calculo_Grilla=atoi(optarg);	
						if(Calculo_Grilla==1)
						printf ("Se calculara una grilla a partir de la aproximacion FEM inicial. \n");
						break;
						case 'B':
						condicion_frontera=atoi(optarg);	
						if(condicion_frontera==1)
						printf ("Se calculara la FEM con condicion de borde tipo Robin, existen fotones en el borde\n");
						if(condicion_frontera==2)
						printf ("Se calculara la FEM con condicion de borde tipo DIrichlet Borde =0, no se escapan fotones fuera del dominio \n");
						break;
						case 'P':
						Tipo_Parada=atoi(optarg);	
						if(Tipo_Parada==1)
						printf ("Se iterara sin condicion de tolerancia en los metodos iterativos. \n");
						break;
						case 'T':
						Traza=atoi(optarg);	
						if(Traza==1)
						printf ("Se exportara la convergencia del error en los metodos iterativos. \n");
						break;
						case 'M':
						Tipo_resolucion_AX=atoi(optarg);	
						printf ("El Metodo para resolver Ax=b es: ");
						if(Tipo_resolucion_AX==1)
						printf ("Metodo Jacobi\n");	
						if(Tipo_resolucion_AX==2)
						printf ("Metodo Gauss - Seidel\n");	
						if(Tipo_resolucion_AX==3)
						printf ("Metodo LSQR\n");	
						if(Tipo_resolucion_AX==6)
						printf ("Metodo LSQR disperso\n");	
						if(Tipo_resolucion_AX==4)
						printf ("Metodo directo LU\n");	
						if(Tipo_resolucion_AX==5)
						printf ("Metodo directo LLT -  Cholesky\n");	
						break;
						case 'I':
						Iteraciones=atoi(optarg);	
						printf ("Iteraciones: %d\n", Iteraciones);
						break;
						case 'N':
						strArchivoNodosFilename = optarg;
						break;
						case 'E':
						strArchivoElementosFilename = optarg;
						break;
						case 'F':
						strArchivoFronterasFilename = optarg;
						break;
						case 'A':
						strArchivoAbsorcionFilename = optarg;
						break;
						case 'S':
						strArchivoScatteringFilename = optarg;
						break;
						case 'Q':
						strArchivoFuentesFilename = optarg;
						break;
						case 'G':
						strArchivoGrillaFilename = optarg;
						break;
						case 'X':
						strArchivoX = optarg;
						break;
						case 't':
						strArchivoTiempos = optarg;
						printf ("Archivo tiempos: %s\n", optarg);
						break;
						case 'i':
						Dominio->Asigna(0,atof(optarg)); 
						printf ("Minimo x: %f\n",  Dominio->Retorna(0));
						break;
						case 'j':
						Dominio->Asigna(1,atof(optarg));	
						printf ("Minimo y: %f\n",  Dominio->Retorna(1));
						break;
						case 'k':
						Dominio->Asigna(2,atof(optarg));	
						printf ("Maximo x: %f\n",  Dominio->Retorna(2));
						break;
						case 'l':
						Dominio->Asigna(3,atof(optarg));	
						printf ("Maximo y: %f\n", Dominio->Retorna(3));
						break;
						case 'r':
						r=atoi(optarg);	
						Resolucion->Asigna(0,(unsigned long long) r);	
						Resolucion->Asigna(1,Resolucion->Retorna(0));
						printf ("Resolucion cuadrada de orden: %Ld\n", Resolucion->Retorna(1));
						break;
						case 'f':
						Fase =atof(optarg);	
						printf ("Funcion de fase : %f\n",Fase);
						break;
						case '?':
						printf ("?\n");
								if (isprint (optopt))
								fprintf (stderr, "Opcion desconocida '-%c'.\n", optopt);
							else
								fprintf (stderr, "Caracter desconocido de opcion '\\x%x'.\n", optopt);
							exit(1);
			
						default:
							printf ("DEFAULT OPTION\n");
							exit(1);
					}
		}
    ProblemaFEM->tTotalGlobal=0.0;
	ProblemaFEM->Tiempo_Inicial_Comunicacion_Grilla=0.0;
    ProblemaFEM->Tiempo_Inicial_Comunicacion_FEM=0.0;	
	
	// Carga de datos
	inicioCargaDatos=MPI_Wtime();
	ProblemaFEM->SetProcesadores(size); // Se asigna el numero de procesadores a utilizar
	ProblemaFEM->SetGeometria(1);
	ProblemaFEM->VerTipoGeometria();
	ProblemaFEM->SetNombreArchivoNodos(strArchivoNodosFilename);
	ProblemaFEM->SetNombreArchivoElementos(strArchivoElementosFilename);
	ProblemaFEM->SetNombreArchivoFronteras(strArchivoFronterasFilename);
	ProblemaFEM->SetNombreArchivoX(strArchivoX);
    ProblemaFEM->SetNombreArchivoGrilla(strArchivoGrillaFilename);
	ProblemaFEM->SetNombreArchivoTiempos(strArchivoTiempos);
	
	ProblemaFEM->CargarGeometriaFEM();

	// Carga de datos fisicos del problema
	ETRAD *FisicaProblema=new ETRAD();
    FisicaProblema->SetNombreArchivoAbsorcion(strArchivoAbsorcionFilename);
 	FisicaProblema->SetNombreArchivoScattering(strArchivoScatteringFilename);
	FisicaProblema->SetNombreArchivoFuentes(strArchivoFuentesFilename);
	FisicaProblema->Asigna_Funcion_Fase(Fase);	
 	ProblemaFEM->Inicializa_Problema_ETRAD(FisicaProblema,0); 
	finCargaDatos=MPI_Wtime();
		
	// Reserva de memoria de las matrices globales
	ProblemaFEM->Inicializar_FEM2D(Tipo_resolucion_AX);
	inicioCalculoFEM=MPI_Wtime();
	
	// Construccion elemental de matrices locales
    ProblemaFEM->Contruir_Matrices_Problema(); 
 
 
    // Ensamblado de matrices globales
    ProblemaFEM->Llena_Matriz_B_FEM();
	ProblemaFEM->Llena_Matriz_A_FEM(condicion_frontera,Tipo_resolucion_AX);
	finCalculoFEM=MPI_Wtime();
	inicioABX=MPI_Wtime();
	 
	
	// Establece las condiciones a utilizar al momento de resolver el sistema de ecuaciones
	// 1 - Normal 2- Sistema reducido
	ProblemaFEM->Establecer_Condicion_Matricial_FEM(condicion_frontera,Tipo_resolucion_AX);
	
	

	if(Iteraciones == 0)
	{	
			Iteraciones = 	ProblemaFEM->Get_Geometria2D_Triangulos()->Retorna_Numero_Nodos();
			printf("Iteraciones = %d",Iteraciones);
	}	
		
		if(size>1)
		{tiempoComunicacionFEM=MPI_Wtime();
		
			  for(int i=1;i<size;i++)	
			  {
				  MPI_Send(&Resolucion->Retorna_Puntero_Datos()[0],2,MPI_UNSIGNED_LONG_LONG,i,1000,MPI_COMM_WORLD);	
				  MPI_Send(&Dominio->Retorna_Puntero_Datos()[0],4,MPI_DOUBLE,i,1001,MPI_COMM_WORLD);	
			  }	
		
		}	
	
}

if(myrank!=0)
{
		MPI_Recv(&Resolucion->Retorna_Puntero_Datos()[0],2,MPI_UNSIGNED_LONG_LONG,0,1000, MPI_COMM_WORLD,&status);
		MPI_Recv(&Dominio->Retorna_Puntero_Datos()[0],4,MPI_DOUBLE,0,1001, MPI_COMM_WORLD,&status);
}

	if(size>1)
	{	
		MPI_Bcast(&Tipo_resolucion_AX,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(&Traza,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(&Iteraciones,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(&Tipo_Parada,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(&Calculo_Grilla,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(&Fase,1,MPI_INT,0,MPI_COMM_WORLD);
		tiempoComunicacionFEM=MPI_Wtime() - tiempoComunicacionFEM;
	}

	// Resolucion del sistema de ecuaciones
	ProblemaFEM->Resolver_Sistema_FEM(Tipo_resolucion_AX,Traza,Iteraciones,Tipo_Parada);
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	if(myrank==0)
	{	
    	finABX=MPI_Wtime();
    }
	

if (myrank == 0)
{
        final=MPI_Wtime();
		printf("El tiempo empleado en Cargar datos fue %f segundos\n",finCargaDatos-inicioCargaDatos);
		printf("El tiempo empleado en Calculo FEM %f segundos\n",finCalculoFEM-inicioCalculoFEM);
		printf("El tiempo empleado en AX=B %f segundos\n",finABX-inicioABX);
		printf("El tiempo empleado Total fue %f segundos\n",final-inicio);
		printf("El porcentaje de tiempo usado en Cargar datos fue es %f \n",100.0*(finCargaDatos-inicioCargaDatos)/(final-inicio));	
		printf("El porcentaje de tiempo usado en Calculo FEM fue %f \n",100.0*(finCalculoFEM-inicioCalculoFEM)/(final-inicio));	
		printf("El porcentaje de tiempo usado en AX=B es %f \n",100.0*(finABX-inicioABX)/(final-inicio));	
		ProblemaFEM->tTotalGlobal+=final-inicio;
	
}
    if(size>1)
    MPI_Barrier(MPI_COMM_WORLD);

						
   if(Calculo_Grilla==1)
  {	   
					if (myrank == 0)
					{
						// Calculo a partir de una grilla 2d definida
						inicioGrilla=MPI_Wtime();
						// Carga en los objetos la propiedad de densidad nodal obtenida por la FEM
						ProblemaFEM->Densidad_FEM();
						  
					
					}
					
					
						// Ingreso de datos del dominio global (Limites, resolucion requerida, etc.)
						ProblemaFEM->Ingreso_Datos_Grilla_FEM(Dominio,Resolucion,size);
				
					
						// Carga en una matriz toda la informacion necesaria para el calculo paralelo de la grilla
						ProblemaFEM->Carga_Datos_Elementos_Grilla();
					
						//Calculo de la grilla 2d FEM 
						ProblemaFEM->Calculo_Grilla_FEM();



						if(myrank ==0)
						{	
							
						finalGrilla=MPI_Wtime();
						
						printf("El tiempo empleado en el calculo de la grilla fue de %f segundos\n",finalGrilla-inicioGrilla);
						ProblemaFEM->tTotalGlobal+=finalGrilla-inicioGrilla;
						}

}

if(myrank==0)
{	
	// Si se desa almacenar en unarchivo los tiempos de claculo y comunicacion
	if(ProblemaFEM->GetNombreArchivoTiempos()!=NULL)
	{	
	
		
/// Almacenamiento de tiempos a archivo
		Archivo *Tiempos=new Archivo();
		Tiempos->SetNombre(ProblemaFEM->GetNombreArchivoTiempos());
		Tiempos->SetModo(GRABA_LINEA_ARCHIVO);	
		Tiempos->Abrir();
	
ProblemaFEM->Mostrar_Tiempo_Comunicacion();
ProblemaFEM->Mostrar_Tiempo_Global();	
printf("Programa finalizado correctamente %s\n",ProblemaFEM->GetNombreArchivoTiempos());	

	fprintf(Tiempos->GetArchivo(), " PROCESO:       [Borde]            [Metodo S.E.]         [Condicion Parada S.E. con N Iteraciones]\n");
		
	if(condicion_frontera==2)
	fprintf(Tiempos->GetArchivo(), "  Ririchlet=0");	
  	if(condicion_frontera==1)	
	fprintf(Tiempos->GetArchivo(), "     Robin");	
  		
	if(Tipo_resolucion_AX==1)
	fprintf(Tiempos->GetArchivo(), "                   Jacobi");	
  	if(Tipo_resolucion_AX==2)	
	fprintf(Tiempos->GetArchivo(), "            Gauss-Seidel");	
	if(Tipo_resolucion_AX==3)
	fprintf(Tiempos->GetArchivo(), "        LSQR Matriz Normal");	
  	if(Tipo_resolucion_AX==4)	
	fprintf(Tiempos->GetArchivo(), "                     LU");	
	if(Tipo_resolucion_AX==5)
	fprintf(Tiempos->GetArchivo(), "               Cholesky");	
  	if(Tipo_resolucion_AX==6)	
	fprintf(Tiempos->GetArchivo(), "            LSQR Disperso");	
	
	if(Tipo_Parada==0)	
	fprintf(Tiempos->GetArchivo(), "               No en base a tolerancia ");	
	if(Tipo_Parada==1)	
	fprintf(Tiempos->GetArchivo(), "                       SI");	
		
	fprintf(Tiempos->GetArchivo(), "\n");	
	fprintf(Tiempos->GetArchivo(), "CPU:%d[Nodos]\t[Elementos]\t[Fuentes ]\t[Resolucion Grilla]\tTiempos:[Carga Datos]\t[Geometria FEM]\t[S.E FEM]\t[Total FEM]\t[Grilla]\t[Global]\t[Error estimacion] \n" ,size);
	
	fprintf(Tiempos->GetArchivo(), "\t\t\t\t%ld\t" ,ProblemaFEM->Get_Geometria2D_Triangulos()->Retorna_Numero_Nodos());	
    fprintf(Tiempos->GetArchivo(), "     %ld\t" ,ProblemaFEM->Get_Geometria2D_Triangulos()->Retorna_Numero_Elementos());	
	fprintf(Tiempos->GetArchivo(), "        %d\t" ,ProblemaFEM->Get_Geometria2D_Triangulos()->Retorna_Numero_de_Fuentes());	
    fprintf(Tiempos->GetArchivo(), "      [%d][%d]\t" ,r,r);		
    fprintf(Tiempos->GetArchivo(), "          %f\t" ,finCargaDatos-inicioCargaDatos);
	fprintf(Tiempos->GetArchivo(), "          %f\t" ,finCalculoFEM-inicioCalculoFEM);	
	fprintf(Tiempos->GetArchivo(), "      %f\t" ,finABX-inicioABX);	
	fprintf(Tiempos->GetArchivo(), " %f\t" ,finalGrilla-inicioGrilla);	 
	fprintf(Tiempos->GetArchivo(), "%f\t" ,final-inicio);	
	final=MPI_Wtime();
	fprintf(Tiempos->GetArchivo(), "%f\t" ,final-inicio);	
 	fprintf(Tiempos->GetArchivo(), "%f\n" ,final-inicio - ( finCargaDatos-inicioCargaDatos  + finCalculoFEM-inicioCalculoFEM +finABX-inicioABX  + finalGrilla-inicioGrilla));
 
	
	fprintf(Tiempos->GetArchivo(), "[Tiempo Comunicacion FEM] [Tiempo Comunicacion Grilla] [Tiempo Comunicacion Global] [Error estimacion Tiempo Comunicacion Global] \n");
	fprintf(Tiempos->GetArchivo(), "       %f" ,ProblemaFEM->tTotalComunicacionFEM + tiempoComunicacionFEM);
    fprintf(Tiempos->GetArchivo(), "                       %f" ,ProblemaFEM->tTotalComunicacionGrilla);
    fprintf(Tiempos->GetArchivo(), "                 %f" ,ProblemaFEM->tTotalComunicacion);
	fprintf(Tiempos->GetArchivo(), "                  %f\n\n" ,ProblemaFEM->tTotalComunicacion - ProblemaFEM->tTotalComunicacionFEM - tiempoComunicacionFEM - ProblemaFEM->tTotalComunicacionGrilla);
  
    




Tiempos->Cerrar();
	
   }	
}

MPI_Finalize();
   
}
