%particles : 1000 particles, 4 cores , vert size: 1000 2000 4000 8000 16000 32000
A = [0 1 2 3 4 5];
P = [0.603895 0.274415 0.123414 0.0542842 0.0227248 0.00950015];

%pressure:
AB = [0 1 2 3 4 5 6];
B = [0.00457035 0.0102628 0.0431184 0.09559311 0.216873 0.415008 0.669862];
BB =[0.971194 0.972611 1.02657 1.14235 1.43576 3.64342 7.95202];

% time:
C = [95.297 23.8915 6.353 1.7969 0.587256 0.532733 0.201922 ];
D = [0 1 2 3 4 5 6];
 
t0 = 65+127*6+62;
t1 = 75 + 260*6 + 68;
t2 = 388*4 + 336 + 329 + 75 + 62;
t3 = 446 + 437 + 494*2 + 371 +58 +52+377;
t4 = 500+564+573+503+422+45+88+438;
t5 = 575+590+551+572+493+63+65+520;
t6 = 560+574+558+589+571+63+61+547;
t7 = 559+565+573+557+564+61+67+560;
t8 = 577+539+572+531+572+64+53+549;
t9 = 540+537+567+521+563+62+56+505;

T1 = [t0 t1 t2 t3 t4 t5 t6 t7 t8 t9];
T2 = [1 2 3 4 5 6 7 8 9 10];


figure(1);

bar(AB,B);
title('Average pressure varying amount of particles');
xlabel('2^x cores * 1000 particles');
ylabel('Pressure');

hold on;

figure(2);

bar(D,C);
title('Particles: 10000/cores');
xlabel('2^x cores');
ylabel('Time s');

figure(3);

bar(A,P);
title('Pressure varying box size');
xlabel('box size');
ylabel('Pressure');

hold on;

figure(4);

bar(T2,T1);
title('Particle sent per time step (80000 particles)');
xlabel('time step');
ylabel('particles sent');

hold on;

figure(5);

bar(AB,BB);
title('Elapsed time versus number of processes');
ylabel('Elapsed time');
xlabel('processes 2^x');

hold on;

