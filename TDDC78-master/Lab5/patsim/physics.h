#ifndef _physics_h
#define _physics_h

#include "definitions.h"

int feuler(particle_t *a, float time_step);

float wall_collide(particle_t *p, wall_t wall);

float collide(particle_t *p1, particle_t *p2);

void interact(particle_t *p1, particle_t *p2, float t);

#endif
