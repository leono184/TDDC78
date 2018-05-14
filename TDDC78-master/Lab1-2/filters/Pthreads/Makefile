CC = gcc

CFLAGS = -g -O3 -march=native 

LFLAGS = -g -lm -lpthread

all: ptthres ptblur

clean:
	-$(RM) *.o ptthres ptblur

ptthres: ppmio.o pthreadsthreshmain.o thresfilter.o
	${CC} -o $@ ppmio.o pthreadsthreshmain.o thresfilter.o $(LFLAGS)

ptblur: ppmio.o gaussw.o pthreadsblurmain.o blurfilter.o
	${CC} -o $@ ppmio.o gaussw.o pthreadsblurmain.o blurfilter.o $(LFLAGS)
