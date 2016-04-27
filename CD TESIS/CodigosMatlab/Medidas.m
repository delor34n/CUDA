% se obtiene el error absoluto, realtivo y cuadratico medio entre
%la sol AG y la sol exacta

nodos = size(A,1);
Ea = sum(abs(xSalidaAXb-xFinalGenetico))/nodos;

% Er = b;
% Ecm = b;
% 
%  for i=1:nodos   
%     if xSalidaAXb(i) ~= 0 
%         Er(i) = abs(xSalidaAXb(i)-xFinalGenetico(i))/abs(xSalidaAXb(i));
%     else
%         Er(i) = abs(xSalidaAXb(i)-xFinalGenetico(i))/0.005;
%     end
%          
%     if xFinalGenetico(i) > 0
%         Ecm(i) = (xSalidaAXb(i) - xFinalGenetico(i))^2;
%     else
%         Ecm(i) = (xSalidaAXb(i) - xFinalGenetico(i))^2;
%     end
%  end
  

% Ecm_final = sum(abs(Ecm));
% Er_final = sum(abs(Er));

%Comparacion solucion numerica vs Algoritmos Geneticos para el RMSE
 rmseAGvsReal = sqrt(mse(xFinalGenetico-xSalidaAXb));

 %coeficiente de correlación de Pearson
  RHO = corr(xFinalGenetico,xSalidaAXb);
  