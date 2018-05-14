CC=mpicc 
CFLAGS= -lm

all: mpiblurfilter mpitresh

mpiblurfilter: blurmain.c blurfilter.c ppmio.c gaussw.c transpose.c
	$(CC) $(CFLAGS) blurmain.c blurfilter.c ppmio.c gaussw.c transpose.c -o mpiblurfilter


