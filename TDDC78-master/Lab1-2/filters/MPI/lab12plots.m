% serial implementation test
standardBlur = [0.467799 0.860397 1.25431 1.43515 1.73382];
standardThres = [0.146508 0.0100344];
% MPI blur image 1 and 4 
mpiblur1 = [0.075280 0.039870 0.023325 0.015035 0.011042 0.007868];
mpiblur4 = [1.438783 0.833833 0.529417 0.388572 0.347569 0.266864];
% MPI threshold image 1 and 4
mpithres1 = [0.002750 0.002198 0.001870 0.002329 0.002957 0.005644];
mpithres4 = [0.051714 0.034824 0.026839 0.024390 0.025038 0.019357];
% MPI and pthreads, blur radius between 10-50 pixels, 16 processes/threads
mpiblur1radius = [0.011073 0.014248 0.018807 0.022092 0.025590];
ptblur1radius = [0.0226864 0.0493265 0.0628907 0.0804301 0.0937333];
% pthreads blur image 1 and 4
ptblur1 = [0.204857 0.107891 0.0554842 0.0295835 0.0302185 0.0261048];
ptblur4 = [3.62438 1.81536 0.932851 0.478451 0.318469 0.292879];
% pthreads threshold image 1 and 4
ptthres1 = [0.00163215 0.00125518 0.00152475 0.0022533 0.00132942 0.00299791];
ptthres4 = [0.0275405 0.020871 0.0173004 0.0171642 0.0165063 0.0168779];

n = [0 1 2 3 4 5];

r = [10 20 30 40 50];
%%
% MPI BLUR
% Threshold over im1 with 1,2,4,8,16,32 threads 
figure(1)
bar(n,mpiblur1)
title("Blur over im1, RADIUS=10")
xlabel('2^x processes')
ylabel('Execution time (s)')

figure(2)
bar(n,mpiblur4)
title("Blur over im4, RADIUS=10")
xlabel('2^x processes')
ylabel('Execution time (s)')
%%
figure(3)
bar(r,mpiblur1radius)
title("Blur over im1 with RADIUS=10,20,30,40,50 and 16 processes ")
xlabel('Blur radius')
ylabel('Execution time (s)')
%%
% MPI THRESHOLD
figure(4)
bar(n,mpithres1)
title("Thresholding im1")
xlabel('2^x processes')
ylabel('Execution time (s)')

figure(5)
bar(n,mpithres4)
title("Thresholding im4")
xlabel('2^x processes')
ylabel('Execution time (s)')

% pthreads BLUR 
figure(6)
bar(n,ptblur1)
title("Blur over im1, RADIUS=10")
xlabel('2^x threads')
ylabel('Execution time (s)')

figure(7)
bar(n,ptblur4)
title("Blur over im4, RADIUS=10")
xlabel('2^x threads')
ylabel('Execution time (s)')

figure(8)
bar(r,ptblur1radius)
title("Blur over im1, RADIUS=10,20,30,40,50 and 16 threads ")
xlabel('Blur radius')
ylabel('Execution time (s)')

% pthreads THRESHOLD
figure(9)
bar(n,ptthres1)
title("Thresholding im1")
xlabel('2^x threads')
ylabel('Execution time (s)')

figure(10)
bar(n,ptthres4)
title("Thresholding im4")
xlabel('2^x threads')
ylabel('Execution time (s)')