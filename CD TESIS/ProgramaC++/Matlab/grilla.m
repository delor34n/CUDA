N=10
fid = fopen('4225grilla8.dat','r');
A = fscanf(fid, '%g %g', [N inf]);
SUR=surf(A(1:N,1:N)); 
figure;
SUR2=surf( log( A(1:N,1:N)) ); 
%set(SUR,'colormap' ,'jet','linewidth', 'none')
set(SUR,'LineStyle','none' )
%set(SUR,'colormap','jet' )
colormap(jet)
view(90,90);
