function fitness = fitness_rmse(A,b,w)

   % w es de 1X289, debe ser de 289x1
N = length(b); %289
E= 0;
for i=1:N
    producto = 0;
    for j=1:N
       producto = producto + A(i,j)*w(j); 
    end
    E = E + abs(producto-b(i));
end
rmse = sqrt(E/N);

%rmse = sqrt(sum(abs(A*w' - b))/N);
%rmse = sqrt(sum((A*w' - b)^2)/N);


fitness = rmse; % raiz okk

end