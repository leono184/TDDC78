#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "ppmio.h"
#include "blurfilter.h"
#include "gaussw.h"
#include <mpi.h>
#include "transpose.h"


int main (int argc, char ** argv) {

	if (argc != 4) {
		fprintf(stderr, "Usage: %s radius infile outfile\n", argv[0]);
		return -1;
	}

	#define MAX_RAD 3000
	int radius;
    int xsize, ysize, colmax;
   	pixel *src = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);
	pixel *transpose = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);	
    double w[MAX_RAD];
 
	radius = atoi(argv[1]);
	if((radius > MAX_RAD) || (radius < 1)) {
		fprintf(stderr, "Radius (%d) must be greater than zero and less then %d\n", radius, MAX_RAD);
		return -1;
	}

    int p;   // number of started MPI processes
    int me;  // my rank 
	pixel item;

   	MPI_Status status;

    MPI_Init(&argc, &argv);

  	MPI_Comm_size( MPI_COMM_WORLD, &p );
    MPI_Comm_rank( MPI_COMM_WORLD, &me );

	double starttime, endtime;
	starttime = MPI_Wtime();

	/* Create custom MPI datatype */
    MPI_Datatype ptype;

	int block_lengths [] = {1,1,1};
	MPI_Datatype block_types []= {MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR};
	MPI_Aint start,displ[3];

	MPI_Address(&item,&start);
	MPI_Address(&item.r,&displ[0]);
	MPI_Address(&item.g,&displ[1]);
	MPI_Address(&item.b,&displ[2]);
	
	displ[0] -= start;
	displ[1] -= start;
	displ[2] -= start;

	MPI_Type_struct(3,block_lengths,displ,block_types,&ptype);
	MPI_Type_commit (&ptype);

  	/* END custom MPI datatype */

	int displs[p];	
	int sendcount[p];

	get_gauss_weights(radius, w);

	if( me == 0 ){

	  /* Take care of the arguments */

		/* read file */
		if(read_ppm (argv[2], &xsize, &ysize, &colmax, (char *) src) != 0){
		    return -1;
		}

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
	}
	/* Cast variables to all processes */
	MPI_Bcast(&xsize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&ysize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(sendcount, p, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(displs, p, MPI_INT, 0, MPI_COMM_WORLD);

  //  printf(" %d recieved xsixe: %d ,ysize: %d,sendcount: %d,displs: %d, radius: %d \n", me,xsize,ysize,sendcount[me], displs[me], radius);

	pixel *rbuff = (pixel*) malloc(sizeof(pixel) * (sendcount[me]));  
	/*Scatter image*/
	MPI_Scatterv(src,sendcount,displs,ptype,rbuff,sendcount[me],ptype,0,MPI_COMM_WORLD);

	/*Blurfilter on partitions*/
   	blurfilter(xsize,sendcount[me]/xsize, rbuff, radius, w);

	/* Gather all partitions */
	MPI_Gatherv(rbuff,sendcount[me],ptype,src,sendcount,displs,ptype,0,MPI_COMM_WORLD);

	if(me==0){	
		transpose_array(src,transpose,ysize,xsize);

		int rows=xsize/p;
		int rest=xsize%p;
		int offset =0, i;
		for(i = 0; i<p; i++){ 
			displs[i]=offset;
			if(i<rest)
				sendcount[i] = (rows + 1)*ysize;
			else
				sendcount[i] =rows*ysize;	
			offset += sendcount[i];	
		}
	}

	MPI_Bcast(sendcount, p, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(displs, p, MPI_INT, 0, MPI_COMM_WORLD);

	rbuff = (pixel*) malloc(sizeof(pixel) * (sendcount[me]));  

	MPI_Scatterv(transpose,sendcount,displs,ptype,rbuff,sendcount[me],ptype,0,MPI_COMM_WORLD);

	/*Blurfilter on partitions*/

   	blurfilter(ysize,sendcount[me]/ysize, rbuff, radius, w);

	MPI_Gatherv(rbuff,sendcount[me],ptype,transpose,sendcount,displs,ptype,0,MPI_COMM_WORLD);

	if(me==0){

		transpose_array(transpose,src,xsize,ysize);

		/* write result */
		printf("Writing output file\n");

		if(write_ppm (argv[3], xsize, ysize, (char *)src) != 0){
			return -1;
		}
		
	}
	endtime = MPI_Wtime();

	printf("That took %f seconds on id:%d\n", endtime-starttime, me 		);

	MPI_Finalize();
    return(0);
}
