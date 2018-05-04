#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
#include "ppmio.h"
#include "thresfilter.h"


int main (int argc, char ** argv) {

    /* Take care of the arguments */

    if (argc != 3) {
	fprintf(stderr, "Usage: %s infile outfile\n", argv[0]);
	return -1;
    }

    int xsize, ysize, colmax;
    pixel *src = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);
    struct timespec stime, etime;

    int p;   // number of started MPI processes
    int me;  // my rank 
	pixel item;

   	MPI_Status status;

    MPI_Init(&argc, &argv);

  	MPI_Comm_size( MPI_COMM_WORLD, &p );
    MPI_Comm_rank( MPI_COMM_WORLD, &me );

	double starttime, endtime;
	starttime = MPI_Wtime();

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


	int displs[p];	
	int sendcount[p];
	int *filtersums = (int *)malloc(p*sizeof(int)); 

	if( me == 0 ){

	  /* Take care of the arguments */

		/* read file */
		if(read_ppm (argv[1], &xsize, &ysize, &colmax, (char *) src) != 0){
		    return -1;
		}

		// partition image 
		int partion = (ysize/p)*xsize; // number of rows in each partition
		int rest = ysize % p; // remaining number of rows
		int i;
		int offset =0;
		
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

 //   printf(" %d recieved xsixe: %d ,ysize: %d,sendcount: %d,displs: %d\n", me,xsize,ysize,sendcount[me], displs[me]);

	pixel *rbuff = (pixel*) malloc(sizeof(pixel) * (sendcount[me]));  
	/*Scatter image*/

	MPI_Scatterv(src,sendcount,displs,ptype,rbuff,sendcount[me],ptype,0,MPI_COMM_WORLD);

	int sum = threshsum(xsize,sendcount[me]/xsize,rbuff);

	/* Gather all partitions */

	MPI_Gather(&sum,1,MPI_INT,filtersums,1,MPI_INT,0,MPI_COMM_WORLD);

	if(me==0){
		int i;
		long int totalsum=0;
		for(i=0;i<p;i++){
			totalsum+=filtersums[i];
		//	printf("Filtersum[%d]= %d \n",i ,filtersums[i]);
		}
		sum=totalsum/(xsize*ysize);
	//	printf("sum= %d \n",sum);
	}
	MPI_Bcast(&sum, 1, MPI_INT, 0, MPI_COMM_WORLD);

	filter(xsize,sendcount[me]/xsize,rbuff,sum);

	MPI_Gatherv(rbuff,sendcount[me],ptype,src,sendcount,displs,ptype,0,MPI_COMM_WORLD);

	if(me==0){

		/* write result */
		printf("Writing output file\n");
		
		if(write_ppm (argv[2], xsize, ysize, (char *)src) != 0){
		  return -1;
		}
	}
	endtime = MPI_Wtime();

	printf("That took %f seconds on id:%d\n", endtime-starttime, me 		);
	MPI_Finalize();

    return(0);
}
