dlmwrite('fronteras.dat',e',',')
dlmwrite('elementos.dat',t',',')
format long
%dlmwrite('nodos.dat',p',',')
%p=p'
%format long
dlmwrite('nodos.dat',p', 'precision', '%.15f','delimiter', ',')
               
%save NodosM.dat p -ascii 


