ent =load('entradas.dat'); % se cargan los parametros de entrada
%carpeta principal add to path todo
if ent(1) == 289
    p= load('nodos289.dat');
    e = load('fronteras289.dat');
    t = load('elementos289.dat');
    if ent(2) == 1
         A = load('matriz289_1.dat');
         b = load('vector289_1.dat');    
    else
     if ent(2) == 2
        A = load('matriz289_2.dat');
        b = load('vector289_2.dat');    
     else
      if ent(2) == 3
            A = load('matriz289_3.dat');
            b = load('vector289_3.dat');    
      else
       if ent(2) == 4
         A = load('matriz289_4.dat');
         b = load('vector289_4.dat');    
       else
        if ent(2) == 5
          A = load('matriz289_5.dat');
          b = load('vector289_5.dat');    
        else
         if ent(2) == 6
            A = load('matriz289_6.dat');
            b = load('vector289_6.dat');    
         else
            disp('Error: Numero de fuente no valido');
            %finish; %fin programa niuna funcion funciona
         end
        end
       end
      end
     end
    end

else
    if ent(1) == 1089
       p = load('nodos1089.dat');
       e = load('fronteras1089.dat');
       t = load('elementos1089.dat');
       
       if ent(2) == 1
         A = load('matriz1089_1.dat');
         b = load('vector1089_1.dat');    
       else
        if ent(2) == 2
          A = load('matriz1089_2.dat');
          b = load('vector1089_2.dat');    
        else
          if ent(2) == 3
            A = load('matriz1089_3.dat');
            b = load('vector1089_3.dat');    
          else
           if ent(2) == 4
            A = load('matriz1089_4.dat');
            b = load('vector1089_4.dat');    
           else
            if ent(2) == 5
              A = load('matriz1089_5.dat');
              b = load('vector1089_5.dat');    
            else
             if ent(2) == 6
               A = load('matriz1089_6.dat');
               b = load('vector1089_6.dat');    
             else
               disp('Error: Numero de fuente no valido');
              %finish; %fin programa niuna funcion funciona
             end
            end
           end
          end
        end
       end

    else
        if ent(1) == 4225
            p = load('nodos4225.dat');
            e = load('fronteras4225.dat');
            t = load('elementos4225.dat');
            
            if ent(2) == 1
              A = load('matriz4225_1.dat');
              b = load('vector4225_1.dat');    
            else
             if ent(2) == 2
               A = load('matriz4225_2.dat');
               b = load('vector4225_2.dat');    
             else
              if ent(2) == 3
                A = load('matriz4225_3.dat');
                b = load('vector4225_3.dat');    
              else
               if ent(2) == 4
                 A = load('matriz4225_4.dat');
                 b = load('vector4225_4.dat');    
               else
                if ent(2) == 5
                 A = load('matriz4225_5.dat');
                 b = load('vector4225_5.dat');    
                else
                 if ent(2) == 6
                  A = load('matriz4225_6.dat');
                  b = load('vector4225_6.dat');    
                 else
                  disp('Error: Numero de fuente no valido');
                   %finish; %fin programa niuna funcion funciona
                 end
                end
               end
             end
            end
           end

        else
            disp('Error: Orden de SEL no valido');
            %fin programa
        end
    end
end

if ent(3) >= 2
    N = ent(3);
else
    disp('Ingrese un tamano que sea mayor o igual que 2');
    %fin programa
end
if ent(4) >= 1
    generac = ent(4);
else
    disp('Ingrese una cantidad de genraciones mayor o igual que 1');
    %fin programa
end

 p0 = p';
 e0 = e';
 t0 = t';
 
 sumF = sum(b);
 numberOfVariables = size(A,1);
  
LB = floor(rand(1,numberOfVariables)); %array de 0 
UB = floor(rand(1,numberOfVariables)) + sumF;%array de sumF

tiempo_inic = cputime;

% Poblacion inicial valida
if numberOfVariables == 4225
    [X0] = PiniValida3(b,N,nodos,sumF,e0(1,:));
end

% Funcion de Fitness
FitnessFunction = @(w) fitness_cond(e0(1,:),A,b,w);

% Restriccion
ConstraintFunction = @(w) constraint2_1(A,b,w);

%se aplica AG
if numberOfVariables == 4225
    opts = gaoptimset('InitialPopulation',X0,'Generations',generac,'PopulationSize',N,'PlotFcns',{@gaplotbestf,@gaplotstopping});
else
    opts = gaoptimset('Generations',generac,'PopulationSize',N,'PlotFcns',{@gaplotbestf,@gaplotstopping});
end
[x,fval] =ga(FitnessFunction,numberOfVariables,[],[],[],[],LB,UB, ...
    ConstraintFunction,opts)

tiempo_final = cputime - tiempo_inic;

xFinalGenetico = x'/max(x);

 %solucion exacta
 xx = linsolve(A,b);
 xSalidaAXb = xx/max(xx);
 
 % Medidas
 Medidas
 % se grafica el espectro
 espectro289Photon


