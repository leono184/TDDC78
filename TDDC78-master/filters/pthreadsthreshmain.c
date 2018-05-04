#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "ppmio.h"
#include "thresfilter.h"
#include "pthread.h"

typedef struct _arg_struct{
	int displs;
	int xsize;
	int yrows;
	int id;
	int ysize;
	pixel *src;
	int pthreads;
} arg_struct;

pthread_barrier_t barr;
pthread_mutex_t mutexSum = PTHREAD_MUTEX_INITIALIZER;
long int sum = 0;

void *myThreadFun(void *args){
	
	arg_struct *data = (arg_struct *)args;

	pixel *src = data->src;
	
	int displs  = data->displs;
	int xsize = data->xsize;
	int id = data->id;
	int ysize = data->ysize;
	int yrows = data->yrows;
	int pthreads = data->pthreads;

	printf("Displs: %d , xsize: %d, ysize: %d \n",displs,xsize,yrows);


	pthread_mutex_lock(&mutexSum);
	printf("ID: %d with myownsum: %d \n",id,pthreadsthreshsum(xsize,yrows,src,displs));
	sum += pthreadsthreshsum(xsize,yrows,src,displs);
	printf("Threshold: %ld, pId: %d \n",sum,id);
	pthread_mutex_unlock(&mutexSum);


   	pthread_barrier_wait(&barr);
	int threshold = (sum)/(ysize*xsize);
	printf("Threshold: %d, pId: %d \n",threshold,id);


	pthreadsfilter(xsize,yrows,src,threshold,displs);

}


int main (int argc, char ** argv) {

	if (argc != 4) {
		fprintf(stderr, "Usage: %s threads infile outfile\n", argv[0]);
		return -1;
	}

	#define MAX_RAD 3000
	int radius;
    int xsize, ysize, colmax;
   	pixel *src = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);
	pixel *transpose = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);	
    double w[MAX_RAD];
    struct timespec stime, etime;
	int p = atoi(argv[1]);
    pthread_barrier_init(&barr, NULL, p);

	pthread_t tid[p];

	
	/* read file */
	if(read_ppm (argv[2], &xsize, &ysize, &colmax, (char *) src) != 0){
	    return -1;
	}

	int displs[p];	
	int sendcount[p];
	
	clock_gettime(CLOCK_REALTIME, &stime);

	// partition image 
	int partion = (ysize/p)*xsize; // number of rows in each partition
	int rest = ysize % p; // remaining number of rows
	int i;
	int offset =0;

	/* calculate partition size and offset */
	for(i = 0; i<p; i++){ 
		displs[i]=offset;
		if(i<rest)
			sendcount[i] = partion+xsize;
		else
			sendcount[i] =partion;	
		offset += sendcount[i];	
	}

	arg_struct data[p];

	for(i=0;i<p;i++){
		data[i].src=src;
		data[i].pthreads = p;
		data[i].id = i;
		data[i].xsize = xsize;
		data[i].yrows = sendcount[i]/xsize;
		data[i].ysize = ysize;
		data[i].displs = displs[i];
		
		pthread_create(&tid[i], NULL, myThreadFun, (void *)&data[i]);
	}

	
	for(i =0 ;i<p;i++){
		pthread_join(tid[i],NULL);
	}

	clock_gettime(CLOCK_REALTIME, &etime);

	printf("Filtering took: %g secs\n", (etime.tv_sec  - stime.tv_sec) +
	   1e-9*(etime.tv_nsec  - stime.tv_nsec)) ;

	/* write result */
	printf("Writing output file\n");

	if(write_ppm (argv[3], xsize, ysize, (char *)src) != 0){
		return -1;
	}
    return(0);
}
