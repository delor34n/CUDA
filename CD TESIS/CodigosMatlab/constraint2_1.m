function [c, ceq] = constraint2_1(A,b,w)

filas = size(A,1);
if filas == 289 || filas == 1089
      c = A*w' - b; % A*w' - b <= 0
      % No nonlinear equality constraints:
      ceq = [];
else
    c = b;
    x=w';
    for i=1:filas
        if b(i) > 0
            c(i) = A(i,:)*x - b(i); % A*w' - b <= 0
          % No nonlinear equality constraints:
        else
           c(i) = (-1)*x(i); % -w'<= 0
        end
    end

% No nonlinear equality constraints:
   ceq = [];
end

end