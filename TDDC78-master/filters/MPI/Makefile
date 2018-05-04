CC = mpicc

CFLAGS = -g -O3 -march=native 

LFLAGS = -g -lm

all: thres blur

clean:
	-$(RM) *.o thres blur

thres: ppmio.o mpithresh.o thresfilter.o
	${CC} -o $@ ppmio.o mpithresh.o thresfilter.o $(LFLAGS)

blur: ppmio.o gaussw.o blurmain.o blurfilter.o transpose.o
	${CC} -o $@ ppmio.o gaussw.o blurmain.o blurfilter.o transpose.o $(LFLAGS)
