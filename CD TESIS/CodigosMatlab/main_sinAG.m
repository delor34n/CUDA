ent =load('entradas.dat'); % se cargan los parametros de entrada
%carpeta principal add to path todo
if ent(1) == 289
    p= load('nodos289.dat');
    e = load('fronteras289.dat');
    t = load('elementos289.dat');
    if ent(2) == 1
         A = load('matriz289_1.dat');
         b = load('vector289_1.dat'); 
         xFinalGenetico = load('289_1.dat');
    else
     if ent(2) == 2
        A = load('matriz289_2.dat');
        b = load('vector289_2.dat');
        xFinalGenetico = load('289_2.dat');
     else
      if ent(2) == 3
            A = load('matriz289_3.dat');
            b = load('vector289_3.dat'); 
            xFinalGenetico = load('289_3.dat');
      else
       if ent(2) == 4
         A = load('matriz289_4.dat');
         b = load('vector289_4.dat');         
         xFinalGenetico = load('289_4.dat');    
       else
        if ent(2) == 5
          A = load('matriz289_5.dat');
          b = load('vector289_5.dat'); 
          xFinalGenetico = load('289_5.dat');
        else
         if ent(2) == 6
            A = load('matriz289_6.dat');
            b = load('vector289_6.dat');           
            xFinalGenetico = load('289_6.dat'); 
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
         xFinalGenetico = load('1089_1.dat');
       else
        if ent(2) == 2
          A = load('matriz1089_2.dat');
          b = load('vector1089_2.dat');           
          xFinalGenetico = load('1089_2.dat');  
        else
          if ent(2) == 3
            A = load('matriz1089_3.dat');
            b = load('vector1089_3.dat');           
            xFinalGenetico = load('1089_3.dat'); 
          else
           if ent(2) == 4
            A = load('matriz1089_4.dat');
            b = load('vector1089_4.dat');          
            xFinalGenetico = load('1089_4.dat');  
           else
            if ent(2) == 5
              A = load('matriz1089_5.dat');
              b = load('vector1089_5.dat');           
              xFinalGenetico = load('1089_5.dat'); 
            else
             if ent(2) == 6
               A = load('matriz1089_6.dat');
               b = load('vector1089_6.dat');          
               xFinalGenetico = load('1089_6.dat');  
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
              xFinalGenetico = load('4225_1.dat');  
            else
             if ent(2) == 2
               A = load('matriz4225_2.dat');
               b = load('vector4225_2.dat');             
               xFinalGenetico = load('4225_2.dat');
             else
              if ent(2) == 3
                A = load('matriz4225_3.dat');
                b = load('vector4225_3.dat');           
                xFinalGenetico = load('4225_3.dat'); 
              else
               if ent(2) == 4
                 A = load('matriz4225_4.dat');
                 b = load('vector4225_4.dat');             
                 xFinalGenetico = load('4225_4.dat');
               else
                if ent(2) == 5
                 A = load('matriz4225_5.dat');
                 b = load('vector4225_5.dat');            
                 xFinalGenetico = load('4225_5.dat');
                else
                 if ent(2) == 6
                  A = load('matriz4225_6.dat');
                  b = load('vector4225_6.dat');             
                  xFinalGenetico = load('4225_6.dat');
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
 
 numberOfVariables = size(A,1);
 nodos = size(A,1);
 
 %solucion exacta
 xx = linsolve(A,b);
 xSalidaAXb = xx/max(xx);
 
 % Medidas
 Medidas
     
   % se grafica el espectro
 espectro289Photon

