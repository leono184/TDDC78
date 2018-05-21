
CFLAGS = -std=c99 -Wall
CC = mpicc

LFLAGS = -lm -lrt
LDFLAGS =

all: parallel sequential

parallel: physics.o main_parallel.o
	$(CC) -o $@ physics.o main_parallel.o $(LFLAGS) $(LDFLAGS)

sequential: physics.o main_sequential.o
	gcc -o $@ physics.o main_sequential.o $(LFLAGS) $(LDFLAGS)

clean:
	-$(RM) *.o parallel sequential
