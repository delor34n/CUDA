
%se hace el grafico
[ux,uy] = pdegrad(p0,t0,xFinalGenetico); % Calculate gradient
ugrad = [ux;uy];

[lux,luy] = pdegrad(p0,t0,xSalidaAXb); % Calculate gradient
 lugrad = [lux;luy];
 
 
subplot(1,2,1); 
h = pdeplot(p0,e0,t0,'xydata',xFinalGenetico,'zdata',xFinalGenetico,...
    'colormap','jet','mesh','on','flowdata',ugrad)

subplot(1,2,2); 
h1 = pdeplot(p0,e0,t0,'xydata',xSalidaAXb,'zdata',xSalidaAXb,...
     'colormap','jet','mesh','on','flowdata',lugrad)