#!/bin/bash 

time mpirun -np 1  ./FEMETRAD   -Q ./Fisica/Fuentes/Fuentes1.dat -N ./Geometria/Dominio2/16641/nodos.dat -E ./Geometria/Dominio2/16641/elementos.dat -F ./Geometria/Dominio2/16641/fronteras.dat -A ./Fisica/Difusion/Ua.dat -S ./Fisica/Difusion/Us.dat A -X 16641Resultados1.dat  -M 6   -g 0 -B 1 -i -4.0 -j -4.0 -k 4.0 -l 4.0 -r 1000 -f 0.0 -G 16641grilla1.dat    -P 1 -t Tiempos.dat 

time mpirun -np 2  ./FEMETRAD   -Q ./Fisica/Fuentes/Fuentes1.dat -N ./Geometria/Dominio2/16641/nodos.dat -E ./Geometria/Dominio2/16641/elementos.dat -F ./Geometria/Dominio2/16641/fronteras.dat -A ./Fisica/Difusion/Ua.dat -S ./Fisica/Difusion/Us.dat A -X 16641Resultados2.dat  -M 6   -g 0 -B 1 -i -4.0 -j -4.0 -k 4.0 -l 4.0 -r 1000 -f 0.0 -G 16641grilla2.dat    -P 1 -t Tiempos.dat 

time mpirun -np 3 ./FEMETRAD   -Q ./Fisica/Fuentes/Fuentes1.dat -N ./Geometria/Dominio2/16641/nodos.dat -E ./Geometria/Dominio2/16641/elementos.dat -F ./Geometria/Dominio2/16641/fronteras.dat -A ./Fisica/Difusion/Ua.dat -S ./Fisica/Difusion/Us.dat A -X 16641Resultados3.dat  -M 6   -g 0 -B 1 -i -4.0 -j -4.0 -k 4.0 -l 4.0 -r 1000 -f 0.0 -G 16641grilla3.dat    -P 1 -t Tiempos.dat 

time mpirun -np 4  ./FEMETRAD   -Q ./Fisica/Fuentes/Fuentes1.dat -N ./Geometria/Dominio2/16641/nodos.dat -E ./Geometria/Dominio2/16641/elementos.dat -F ./Geometria/Dominio2/16641/fronteras.dat -A ./Fisica/Difusion/Ua.dat -S ./Fisica/Difusion/Us.dat A -X 16641Resultados4.dat  -M 6   -g 0 -B 1 -i -4.0 -j -4.0 -k 4.0 -l 4.0 -r 1000 -f 0.0 -G 16641grilla4.dat    -P 1 -t Tiempos.dat 

time mpirun -np 5  ./FEMETRAD  -Q ./Fisica/Fuentes/Fuentes1.dat -N ./Geometria/Dominio2/16641/nodos.dat -E ./Geometria/Dominio2/16641/elementos.dat -F ./Geometria/Dominio2/16641/fronteras.dat -A ./Fisica/Difusion/Ua.dat -S ./Fisica/Difusion/Us.dat A -X 16641Resultados5.dat  -M 6   -g 0 -B 1 -i -4.0 -j -4.0 -k 4.0 -l 4.0 -r 1000 -f 0.0 -G 16641grilla5.dat    -P 1 -t Tiempos.dat 

time mpirun -np 6  ./FEMETRAD  -Q ./Fisica/Fuentes/Fuentes1.dat -N ./Geometria/Dominio2/16641/nodos.dat -E ./Geometria/Dominio2/16641/elementos.dat -F ./Geometria/Dominio2/16641/fronteras.dat -A ./Fisica/Difusion/Ua.dat -S ./Fisica/Difusion/Us.dat A -X 16641Resultados6.dat  -M 6   -g 0 -B 1 -i -4.0 -j -4.0 -k 4.0 -l 4.0 -r 1000 -f 0.0 -G 16641grilla6.dat    -P 1 -t Tiempos.dat 

time mpirun -np 7  ./FEMETRAD   -Q ./Fisica/Fuentes/Fuentes1.dat -N ./Geometria/Dominio2/16641/nodos.dat -E ./Geometria/Dominio2/16641/elementos.dat -F ./Geometria/Dominio2/16641/fronteras.dat -A ./Fisica/Difusion/Ua.dat -S ./Fisica/Difusion/Us.dat A -X 16641Resultados7.dat  -M 6   -g 0 -B 1 -i -4.0 -j -4.0 -k 4.0 -l 4.0 -r 1000 -f 0.0 -G 16641grilla7.dat    -P 1 -t Tiempos.dat 

time mpirun -np 8 ./FEMETRAD    -Q ./Fisica/Fuentes/Fuentes1.dat -N ./Geometria/Dominio2/16641/nodos.dat -E ./Geometria/Dominio2/16641/elementos.dat -F ./Geometria/Dominio2/16641/fronteras.dat -A ./Fisica/Difusion/Ua.dat -S ./Fisica/Difusion/Us.dat A -X 16641Resultados8.dat  -M 6   -g 0 -B 1 -i -4.0 -j -4.0 -k 4.0 -l 4.0 -r 1000 -f 0.0 -G 16641grilla8.dat    -P 1 -t Tiempos.dat 


