#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "ppmio.h"
#include "blurfilter.h"
#include "gaussw.h"
void transpose_array (pixel *A, pixel *T,const int ysize,const int xsize) {

	int j,i;	
	for(i = 0; i<ysize;i++){
		for(j=0;j<xsize;j++){
			T[j*ysize + i]=A[j + i*xsize];		
		}
	}
}