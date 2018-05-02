#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "ppmio.h"
#include "blurfilter.h"
#include "gaussw.h"
#include "pthread.h"
#include "transpose.h"

typedef struct _arg_struct{
	int displs;
	int xsize;
	int ysize;
	pixel *src;
	int radius;
	double *w;
} arg_struct;


void *myThreadFun(void *args){
	
	arg_struct *data = (arg_struct *)args;

	pixel *src = data->src;
	
	int displs  = data->displs;
	int xsize = data->xsize;
	int ysize = data->ysize;
	int radius = data->radius;
	double *w = data->w;



	printf("Displs: %d , xsize: %d, ysize: %d, radius: %d \n",displs,xsize,ysize,radius);


	pblurfilter(xsize,ysize, src, radius, w, displs/xsize);

}


int main (int argc, char ** argv) {

	if (argc != 5) {
		fprintf(stderr, "Usage: %s threads radius infile outfile\n", argv[0]);
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
 
	radius = atoi(argv[2]);
	if((radius > MAX_RAD) || (radius < 1)) {
		fprintf(stderr, "Radius (%d) must be greater than zero and less then %d\n", radius, MAX_RAD);
		return -1;
	}

    get_gauss_weights(radius, w);
	pthread_t tid[p];

	
	/* read file */
	if(read_ppm (argv[3], &xsize, &ysize, &colmax, (char *) src) != 0){
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
		data[i].radius = radius;
		data[i].w = w;
		data[i].xsize = xsize;
		data[i].ysize = sendcount[i]/xsize;
		data[i].displs = displs[i];
		
		pthread_create(&tid[i], NULL, myThreadFun, (void *)&data[i]);
	}

	
	for(i =0 ;i<p;i++){
		pthread_join(tid[i],NULL);
	}

	transpose_array(src,transpose,ysize,xsize);

	partion = (xsize/p)*ysize; // number of rows in each partition
	rest = xsize % p; // remaining number of rows
	offset = 0;

	/* calculate partition size and offset */
	for(i = 0; i<p; i++){ 
		displs[i]=offset;
		if(i<rest)
			sendcount[i] = partion+ysize;
		else
			sendcount[i] =partion;	
		offset += sendcount[i];	
	}
	

	for(i=0;i<p;i++){
		data[i].src=transpose;
		data[i].radius = radius;
		data[i].w = w;
		data[i].xsize = ysize;
		data[i].ysize = sendcount[i]/ysize;
		data[i].displs = displs[i];
		
		pthread_create(&tid[i], NULL, myThreadFun, (void *)&data[i]);
	}

	for(i =0 ;i<p;i++){
		pthread_join(tid[i],NULL);
	}

	transpose_array(transpose,src,xsize,ysize);


	clock_gettime(CLOCK_REALTIME, &etime);

	printf("Filtering took: %g secs\n", (etime.tv_sec  - stime.tv_sec) +
	   1e-9*(etime.tv_nsec  - stime.tv_nsec)) ;

	/* write result */
	printf("Writing output file\n");

	if(write_ppm (argv[4], xsize, ysize, (char *)src) != 0){
		return -1;
	}
    return(0);
}
