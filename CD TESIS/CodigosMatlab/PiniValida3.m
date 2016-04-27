function [pobla_ini] = PiniValida3(b,N,nodos,sumF,front)

x = floor(rand(N,nodos)); % matriz inicial con 0

fronteras = sort(front); % primera fila estan los nodos frontera, 1x64

% condicion 1 borde 0, -1 por ahora
for fila =1:N
      for nodo=1:nodos
         if find(fronteras == nodo) % el nodo esta en la frontera
                x(fila,nodo) = -1; 
         end
      end  
end

%cond 2 sum(nodos) = suma de la fuente

poblacion = [];
    
    for ind=1:N
       sumFaux = sumF; 
       V = [];
       for i=1:nodos
           if x(ind,i) == 0
               V(i) = i;
           end
       end % se crea arrglo de con los nodos q no son borde
        
       temp = [];
       masas = [];

       while sumFaux > 0 && size(V,2) > 0
          rng('shuffle'); % genera aleatorios distintos
          nodosaux = nodos;
          r = randi([1 nodosaux],1,1); % eligo nodo al azar
          
          if x(ind,r) == 0
              temp = [temp,r]; 
              %V(r) = [];
              nodosaux = nodosaux - 1; 
          
              sumFaux =round(sumFaux);
              R = randi([1 sumFaux],1,1); 
              masas = [masas,R]; 
              sumFaux = sumFaux - R;
          end
       end
       
       DimTemp = size(temp,2); %  va ser igual a DimMasas
       
       for j=1:DimTemp % se asignan las masas a los nodos q no son borde
           x(ind,temp(j)) = masas(j);
       end   
       %poblacion = [poblacion;crom];
    end
    % debe cumplirse que size(find(x(2,:) > 0),2) = size(masas,2)
    %sum(masas) = sumF

    for fila= 1:N
        for col = 1:nodos
           if x(fila,col) == -1
               x(fila,col) = 0;
           end
           if b(col) > 0
               x(fila,col) = b(col) - 0.2*b(col);
           end
        end
    end
    
    pobla_ini = round(x); % 10 x 289
end
