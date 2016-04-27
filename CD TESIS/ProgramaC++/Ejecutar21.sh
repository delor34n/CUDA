#!/bin/bash 

#time mpirun -np 1  ./FEMETRAD   -Q ./Fisica/Fuentes/Fuentes1.dat -N ./Geometria/Dominio2/16641/nodos.dat -E ./Geometria/Dominio2/16641/elementos.dat -F ./Geometria/Dominio2/16641/fronteras.dat -A ./Fisica/Difusion/Ua.dat -S ./Fisica/Difusion/Us.dat A -X ./16641Resultados1.dat  -M 6   -g 1 -B 1 -i -4.0 -j -4.0 -k 4.0 -l 4.0 -r 1000 -f 0.0 -G ./16641grilla1.dat    -P 1 -t ./Tiempos.dat 

time mpirun -np 1  ./FEMETRAD2   -Q ./Fisica/Fuentes/Fuentes6.dat -N ./Geometria/Dominio2/4225/nodos.dat -E ./Geometria/Dominio2/4225/elementos.dat -F ./Geometria/Dominio2/4225/fronteras.dat -A ./Fisica/Difusion/Ua.dat -S ./Fisica/Difusion/Us.dat A -X ./4225Resultados1.dat  -M 2   -g 1 -B 1 -i -4.0 -j -4.0 -k 4.0 -l 4.0 -r 50 -f 0.0 -G ./4225grilla1.dat    -P 1 -t ./Tiempos.dat 

