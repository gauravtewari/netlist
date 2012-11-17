fid = fopen('b15_C.out');
size = fscanf(fid, '%u', 1);
fanouts = fscanf(fid, '%u', size+1);
x = [0:size];
fanouts = fanouts';



bar(x,fanouts);
xlabel({'Fanout (x)'});
ylabel({'Num of Gates'});
title({'Number of Gates in Netlist with fanout equal to x'});

