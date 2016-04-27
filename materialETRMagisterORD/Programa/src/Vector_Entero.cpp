#include "Vector_Entero.hpp"

///Solicita memoria para el vector
void Vector_Entero::Solicita_Memoria( unsigned long long col,  unsigned long long  val)
{
   V = new unsigned long long [col];
   if (!V)
   {
      Ver_Informacion();      
      exit(1);
   }
   Columna = col;
Inicializa(val);
}

/// Asigna nombre a la matriz
void Vector_Entero::AsignaNombre(const char *nmb)
{
   Nmb = new char[strlen(nmb)+2];
   if (!Nmb)
   {
      printf("\nError no hay memoria para el nombre del vector\n");
      exit(1);
   }
   strcpy(Nmb, nmb);   
}

/// Inicializa a val el vector
void Vector_Entero::Inicializa(unsigned long long val)
{
   unsigned long long i;
   for (i = 0; i < Columna; i++) V[i] = val;
}

/// Asigna valor en columna
void Vector_Entero::Asigna(unsigned long long col,unsigned long long val)
{
   if (col < 0 || col >= Columna)
   {
      Ver_Informacion();      
      printf("Error indice desbordado (%Ld)\n",col);
      exit(1);
   }
   V[col] = val;
}


/// Retorna valor en columna
unsigned long long Vector_Entero::Retorna( unsigned long long col)
{
   if (col < 0 || col >= Columna)
   {
      Ver_Informacion();      
      printf("Error indice desbordado (%Ld)\n", col);
      exit(1);
   }
   return V[col];
}

/// Visualiza el vector
void Vector_Entero::Visualiza()
{
   unsigned long long i;
   Ver_Informacion();      

      for (i = 0; i < Columna; i++)
      {
         printf(" %Ld ",V[i]);
      }
   
}

/// Busca un valor en el vector y retorna 1 en caso verdadero (solo indica si existe, no cuenta)
unsigned long long Vector_Entero::Buscar_Valor(unsigned long long valor)
{unsigned long long salida=0;
	for (unsigned long long i = 0; i < Columna; i++)
      {
         if(V[i]==valor)
		 {	 
		 salida=1;
		 break;
		}			 
      }
return salida;
}

/// Busca un valor en el vector hasta un cierto indice y retorna 1 en caso verdadero (solo indica si existe, no cuenta)
unsigned long long Vector_Entero::Buscar_Valor_Limite(unsigned long long valor,unsigned long long limite)
{int salida=0;

	for (unsigned long long i = 0; i < limite; i++)
      {
         if(V[i]==valor)
		 {	 
		 salida=1;
		 break;
		}			 
      }
return salida;
}

/// Copia el vector a otro vector a
void Vector_Entero::Copia(Vector_Entero *a)
{
 
   if (Columnas() != a->Columnas())
   {
      printf("Error en dimensión de los vectores al copiar\n");
      Ver_Informacion();      
      a->Ver_Informacion();      
      exit(1);
   }
   unsigned long long  i;
   for (i = 0; i < Columna; i++)
   {
      a->Asigna(i,V[i]);
   }
}

/// Sumatoria de los elementos de un vector
unsigned long long Vector_Entero::Sumatoria_Vector()
{
 
   unsigned long long suma=0;
   for (unsigned long long i = 0; i < Columna; i++)
   {
      suma+=V[i];
   }
 return suma;
}
