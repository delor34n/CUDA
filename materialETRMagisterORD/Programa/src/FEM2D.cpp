   #include "FEM2D.hpp"
   
   /// Libera carga de memoria	
   void FEM2D::Libera_Memoria_Carga()
   {
	  
	  PerteneciaEL->Libera_Memoria();
	  PerteneciaELF->Libera_Memoria();
	  RigidezEl->Libera_Memoria();	
      MasaEl->Libera_Memoria();   
	  QEl->Libera_Memoria();	 
	  BordeEl->Libera_Memoria();
	   
	}
   
   
   /// Reserva memoria para el sistema de ecuaciones, segun geometria utilizada
    void FEM2D::Inicializar_FEM2D(int Tipo_resolucion_AX)
   {  

	  unsigned long long nodos;
      if(GetGeometria()==GEOMETRIA_TRIANGULOS)
	  {
		  nodos=Get_Geometria2D_Triangulos()->Retorna_Numero_Nodos();
         
		  // Uso de matrices tradicional		
		  if(Tipo_resolucion_AX!=LSQR_DISPERSO)
		  Set_Reserva_Memoria_A_FEM(nodos);
		  
				  Set_Reserva_Memoria_B_FEM(nodos);	 
		}
            
		  
		 else if (GetGeometria() == GEOMETRIA_CUADRILATEROS)
		{printf("Geometria no disponible...\n");}
	   
			 else if (GetGeometria() == GEOMETRIA_TETRAHEDROS)
			{printf("Geometria no disponible...\n");}
	   
					else
					{
						printf("No se puede iniciar el proceso FEM2D, la geometria solicitada no esta disponible\n");
					}				  
   }
   
   /// Realiza un llamada al metodo de contruccion de la vector de carga global B segun la geometria utilizada 
   void FEM2D::Llena_Matriz_B_FEM()
   {
	   
	 if(GetGeometria()==GEOMETRIA_TRIANGULOS)
	 {Llena_Matriz_B_FEM_2D_Triangulos();}
		
		 if (GetGeometria() == GEOMETRIA_CUADRILATEROS)
		{printf("Geometria no disponible...\n");}
	   
			 if (GetGeometria() == GEOMETRIA_TETRAHEDROS)
			{printf("Geometria no disponible...\n");}

	}
   
	/// Realiza un llamada al metodo de contruccion de la matriz global A segun la geometria utilizada 
	void FEM2D::Llena_Matriz_A_FEM(int borde,int tipo_solucion_matricial)
	{
		
	 if(GetGeometria()==GEOMETRIA_TRIANGULOS)	   
	 {
		 
		 Llena_Matriz_A_FEM_2D_Triangulos(tipo_solucion_matricial);
		 
		 // Si se considera el borde en el sistema
		 if(borde==1)
		 {
		  printf("Utilizando fronteras con Hash...\n");	 
		 Aporte_Matriz_A_FEM_Borde_2D_Triangulos(tipo_solucion_matricial);
		 }
		 		printf("FIN BORDE 2D...\n");	 

			 }

		 if (GetGeometria() == GEOMETRIA_CUADRILATEROS)
		{printf("Geometria no disponible...\n");}
   
			 if (GetGeometria() == GEOMETRIA_TETRAHEDROS)
			{printf("Geometria no disponible...\n");}
	 
	}
   
	/// Suma un aporte nodal al vector de cargas global B del sistema de ecuaciones
	void FEM2D::Set_Suma_Nodal_B_FEM(unsigned long long i,double aNodal)
   {B->Asigna(i, aNodal + B->Retorna(i) );}
   
   /// Suma un aporte nodal a la matriz global A del sistema de ecuaciones
    void FEM2D::Set_Suma_Nodal_A_FEM(unsigned long long i,unsigned long long j,double aNodal)
   { A->Asigna(i,j,A->Retorna(i,j) + aNodal);}      
   
   /// Reserva memoria para la matriz global A del sistema de ecuaciones
   	void FEM2D::Set_Reserva_Memoria_A_FEM(unsigned long long nNodo)
    {A=new Matriz(nNodo,nNodo);}   
	
    /// Reserva memoria para la matriz global B del sistema de ecuaciones	
	void FEM2D::Set_Reserva_Memoria_B_FEM(unsigned long long nNodos)
   {B=new Vector(nNodos,"Vector de Cargas");} 
	  
   
   /// Construye el vector de cargas global B del sistema de ecuaciones calculada con elementos finitos triangulares
	void FEM2D::Llena_Matriz_B_FEM_2D_Triangulos()
   {
	
	 unsigned long long nElementos;unsigned long long iTmp;
	 EL=Get_Geometria2D_Triangulos()->Retornar_Elementos_Geometria();
	 nElementos=Get_Geometria2D_Triangulos()->Retorna_Numero_Elementos(); 
	 
	   for(unsigned long long idt=0;idt<nElementos;idt++)
       {
		 PerteneciaEL=EL[idt].Get_Matriz_Pertenecia_Elemento();
		 QEl=EL[idt].Get_CargasV();
				   
		   for(int j=0;j<3;j++)
			{	
				   iTmp=PerteneciaEL->Retorna(0,j);
	     	       Set_Suma_Nodal_B_FEM(iTmp, QEl->Retorna(j));  
			}
	   }

    }	
   
   /// Construye la matriz global A del sistema de ecuaciones a partir de las matrices de rigidez y de masa, calculada con elementos finitos triangulares
	void FEM2D::Llena_Matriz_A_FEM_2D_Triangulos(int tipo_AX)
   {  	   	printf("Llenando  Matriz Hashing.....\n");
	   unsigned long long nElementos;
	   unsigned long long  nNodos=Get_Geometria2D_Triangulos()->Retorna_Numero_Nodos();
	   unsigned long long iTmp,jTmp;
	   EL=Get_Geometria2D_Triangulos()->Retornar_Elementos_Geometria();
	   nElementos=Get_Geometria2D_Triangulos()->Retorna_Numero_Elementos();
	   for(unsigned long long idt=0;idt<nElementos;idt++)
       {
		 PerteneciaEL=EL[idt].Get_Matriz_Pertenecia_Elemento();
		 RigidezEl=EL[idt].Get_Matriz_Rigidez_Elemento();
		 MasaEl=EL[idt].Get_Matriz_Masa_Elemento();   
	//	printf("Dominio =%d",EL[idt].Get_Dominio());
		   for(int i=0;i<3;i++)
			{	
				for(int j=0;j<3;j++)	
				{
				   iTmp= ( unsigned long long ) PerteneciaEL->Retorna(j,i);
				   jTmp= ( unsigned long long ) PerteneciaEL->Retorna(i,j);			
				   if(tipo_AX==LSQR_DISPERSO)//LSQR Paralelo con matrices Hash y Vectores dispersos
				   {
				   Hashing_FEM( (nNodos*iTmp + jTmp),RigidezEl->Retorna(i,j),iTmp,jTmp);	 	
				   Hashing_FEM( (nNodos*iTmp + jTmp),MasaEl->Retorna(i,j),iTmp,jTmp);	
					   
					}
                    else // Metodos secuenciales y con matrices normales
                    {
				      Set_Suma_Nodal_A_FEM(iTmp,jTmp,RigidezEl->Retorna(i,j));
		              Set_Suma_Nodal_A_FEM(iTmp,jTmp,MasaEl->Retorna(i,j));

						}						
					
					
		        }
			}
		}		
		
   }	
   
      /// Aporta a la matriz global A del sistema de ecuaciones los calculos del borde global 
	void FEM2D::Aporte_Matriz_A_FEM_Borde_2D_Triangulos(int tipo_AX)
   {  printf("Llenando  Matriz Map - Hashing Borde. ....");
	    unsigned long long nNodos=Get_Geometria2D_Triangulos()->Retorna_Numero_Nodos();
	   ELF=Get_Geometria2D_Triangulos()->Retornar_Fronteras_Geometria();
	   unsigned long long nElementosFrontera;
	   unsigned long long iTmp,jTmp;
	   nElementosFrontera=Get_Geometria2D_Triangulos()->Get_Numero_Nodos_Frontera_Dominio_Global();
	   
	   for(unsigned long long idt=0;idt<nElementosFrontera;idt++)
       {
		 PerteneciaELF=ELF[idt].Get_pertenencia_Borde();
		 BordeEl=ELF[idt].Get_MIB();
		   for(int i=0;i<2;i++)
			{	
				for(int j=0;j<2;j++)	
				{
				   					   
				   iTmp=( unsigned long long ) PerteneciaELF->Retorna(i);
				   jTmp=( unsigned long long ) PerteneciaELF->Retorna(j);			
				   if(tipo_AX==LSQR_DISPERSO) //LSQR con hash y Vectores dispersos
                   Hashing_FEM(nNodos*iTmp + jTmp,BordeEl->Retorna(i,j),iTmp,jTmp);	 	
		           else // Metodos secuenciales y con matrices normales
				   Set_Suma_Nodal_A_FEM(iTmp,jTmp,BordeEl->Retorna(i,j));
		        }
			}
		}
		
	  	
   }
