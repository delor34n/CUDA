#include "Vector.hpp"
#include <string.h>

/// Solicita la memoria para el vector
void Vector::Solicita_Memoria(unsigned long long col, double val)
{
   V = new double[col];
   if (!V)
   {
      Ver_Informacion();      
      exit(1);
   }
   Columna = col;
   Inicializa(val);
}

/// Asigna nombre a la matriz
void Vector::AsignaNombre(const char *nmb)
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
void Vector::Inicializa(const double val)
{
   unsigned long long i;
   for (i = 0; i < Columna; i++) V[i] = val;
}

/// Asigna valor en columa especifica
void Vector::Asigna(unsigned long long col, double val)
{
   if (col < 0 || col >= Columna)
   {
      Ver_Informacion();      
	   
      printf("Error indice desbordado V(%Ld)\n",col);
      exit(1);
   }
   V[col] = val;
}


/// Retorna valor en columna especifica
double Vector::Retorna(unsigned long long col)
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
void Vector::Visualiza()
{
   unsigned long long i;
  
   Ver_Informacion();      
 
      for (i = 0; i < Columna; i++)
      {
        
			printf(" %f ",V[i]);

      }
   
   printf("\n");
}



/// Copia el vector a otro vector a
void Vector::Copia(Vector *a)
{
 
   if (Columnas() != a->Columnas())
   {
      printf("Error en dimensión de los vectores al copiar\n");
      Ver_Informacion();      
      a->Ver_Informacion();      
      exit(1);
   }
   unsigned long long i;
   for (i = 0; i < Columna; i++)
   {
      a->Asigna(i,V[i]);
   }
}


/// Suma los vectores a y b
void Vector::Suma(Vector *a, Vector *b)
{
 
   if (Columnas() != a->Columnas() || Columnas() != b->Columnas())
   {
      printf("Error en dimensión de los vectores al sumar\n");
      Ver_Informacion();      
      a->Ver_Informacion();      
      b->Ver_Informacion();      
      exit(1);
   }

   
   unsigned long long i;
   for (i = 0; i <Columna; i++) V[i] = a->Retorna(i) + b->Retorna(i);
}

/// Suma al vector a 
void Vector::Suma(Vector *a)
{
   if (Columnas() != a->Columnas())
   {
      printf("Error en dimensión de los vectores al sumar\n");
      Ver_Informacion();      
      a->Ver_Informacion();      
      exit(1);
   }

   unsigned long long i;
   for (i = 0; i < Columna; i++) V[i] = V[i] + a->Retorna(i);
}

/// Resta los vectores a menos b
void Vector::Resta(Vector *a, Vector *b)
{
   if (Columnas() != a->Columnas() || Columnas() != b->Columnas())
   {
      printf("Error en dimensión de los vectores al restar\n");
      Ver_Informacion();      
      a->Ver_Informacion();      
      b->Ver_Informacion();      
      exit(1);
   }

   unsigned long long i;
   for (i = 0; i <Columna; i++) V[i] = a->Retorna(i) - b->Retorna(i);
}

/// Resta el vector a
void Vector::Resta(Vector *a)
{
   if (Columnas() != a->Columnas())
   {
      printf("Error en dimensión de los vectores al restar\n");
      Ver_Informacion();      
      a->Ver_Informacion();      
      exit(1);
   }

   unsigned long long i;
   for (i = 0; i < Columna; i++) V[i] = V[i] - a->Retorna(i);
}

/// Multiplica el vector por el escalar 
void Vector::Multiplica(double esc)
{
   unsigned long long i;
   for (i = 0; i < Columna; i++) V[i] = V[i] * esc;
}


/// Calcula la norma infinito
double Vector::Norma_inf(void)
{
   unsigned long long i;
   double x, max = 0.0;
   
   for (i = 0; i < Columna; i++)
   {
      x = V[i];
      if (x > max) max = x;
   }
   return max;
}

/// Calcula el producto punto 
double Vector::Producto_Punto()
{
   unsigned long long i;
   double tmp, val = 0.0;
   for (i = 0; i < Columna; i++)
   {
      tmp = V[i];
      val += tmp * tmp;
   }
   return val;   
}

/// Calcula el  Vector Normal
double Vector::Vector_Normal()
{
   unsigned long long i;
   double tmp, val = 0.0;
   for (i = 0; i < Columna; i++)
   {
      tmp = V[i];
      val += tmp * tmp;
   }
   return( (double)sqrt((double)val) );   
}

/// Calcula el producto punto con otro vector
double Vector::Producto_Punto(Vector *a)
{
   unsigned long long i;
   double val = 0.0;
   for (i = 0; i < Columna; i++)
   {
      val += V[i] * a->Retorna(i);
   }
   return val;   
}
/// Sumatoria de los elementos de un vector
double Vector::Sumatoria_Vector()
{
 
   double suma=0;
   for (unsigned long long i = 0; i < Columna; i++)
   {
      suma+=   V[i];
   }
 return suma;
}
/// Obtiene el error norma 2 entre dos vectores
double Vector::Error_Norma2(Vector *a,Vector *b)
{
	 
     for (unsigned long long i = 0; i < Columna; i++) V[i] = a->Retorna(i) - b->Retorna(i);	
	 

	 double sumaTMP=0.0;


	 for(unsigned long long cont=0;cont<Columna;cont++)
     {
	
		 sumaTMP = sumaTMP + pow( V[cont] ,2);

	 }		 

		 return ((double)sqrt((double)sumaTMP)/Columna);  
	 
 }
