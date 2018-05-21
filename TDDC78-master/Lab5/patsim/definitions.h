#include<stdlib.h>
#include<math.h>
#include <stdbool.h>

#include "physics.h"

#ifndef _definitions_h
#define _definitions_h

#define PI 3.141592653

#define MAX_NO_PARTICLES  15000  /* Maximum number of particles/processor */
#define INIT_NO_PARTICLES 500    /* Initial number of particles/processor */
#define MAX_INITIAL_VELOCITY 50


#define BOX_HORIZ_SIZE 10000.0
#define BOX_VERT_SIZE 10000.0
#define WALL_LENGTH (2.0*BOX_HORIZ_SIZE+2.0*BOX_VERT_SIZE)


typedef struct wall {
    float x0;
    float x1;
    float y0;
    float y1;
    bool leftsolid;
    bool rightsolid;
    bool upsolid;
    bool downsolid;
} wall_t;

typedef struct particle {
    float x;
    float y;
    float vx;
    float vy;
} particle_t;

typedef struct particle_node {
  particle_t particle;
  bool collided;
  struct particle_node* next;
} particle_node_t;

#endif
