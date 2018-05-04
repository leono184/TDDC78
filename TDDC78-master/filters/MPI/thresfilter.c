#include "thresfilter.h"
#include "stdio.h"

void thresfilter(const int xsize, const int ysize, pixel* src){
#define uint unsigned int 

  uint sum, i, psum, nump;

  nump = xsize * ysize;

  for(i = 0, sum = 0; i < nump; i++) {
    sum += (uint)src[i].r + (uint)src[i].g + (uint)src[i].b;
  }
  printf("Sum: %d \n",sum);
  printf("nump: %d \n",nump);
  sum /= nump;
  printf("Sum/=nump: %d \n",sum);

  for(i = 0; i < nump; i++) {
    psum = (uint)src[i].r + (uint)src[i].g + (uint)src[i].b;
    if(sum > psum) {
      src[i].r = src[i].g = src[i].b = 0;
    }
    else {
      src[i].r = src[i].g = src[i].b = 255;
    }
  }
}

int threshsum(const int xsize, const int ysize, pixel* src){
#define uint unsigned int 

  uint sum, i, nump;

  nump = xsize * ysize;

  for(i = 0, sum = 0; i < nump; i++) {
    sum += (uint)src[i].r + (uint)src[i].g + (uint)src[i].b;
  }
  return sum;
}

int pthreadsthreshsum(const int xsize, const int ysize, pixel* src, const int startindex){
#define uint unsigned int 

  uint sum, i, nump;

  nump = xsize * ysize;

  for(i = startindex, sum = 0; i < nump+startindex; i++) {
    sum += (uint)src[i].r + (uint)src[i].g + (uint)src[i].b;
  }
  return sum;
}




void filter(const int xsize, const int ysize, pixel* src, const int sum){
#define uint unsigned  int 

  uint i, psum, nump;
	

  nump = xsize * ysize;

  for(i = 0; i < nump; i++) {
    psum = (uint)src[i].r + (uint)src[i].g + (uint)src[i].b;
    if(sum > psum) {
      src[i].r = src[i].g = src[i].b = 0;
    }
    else {
      src[i].r = src[i].g = src[i].b = 255;
    }
  }
}

void pthreadsfilter(const int xsize, const int ysize, pixel* src, const int sum, const int startindex){
#define uint unsigned  int 

  uint i, psum, nump;
	

  nump = xsize * ysize;

  for(i = startindex; i < nump+startindex; i++) {
    psum = (uint)src[i].r + (uint)src[i].g + (uint)src[i].b;
    if(sum > psum) {
      src[i].r = src[i].g = src[i].b = 0;
    }
    else {
      src[i].r = src[i].g = src[i].b = 255;
    }
  }
}
