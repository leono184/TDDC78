#ifndef _physics_h
#define _physics_h

#include "definitions.h"

int feuler(Particle&, float time_step);

float wall_collide(Particle&, Wall&);

float collide(Particle&, Particle&);

void interact(Particle&, Particle&, float t);

#endif
