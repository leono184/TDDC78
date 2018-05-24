#include "physics.h"

#ifndef _definitions_h
#define _definitions_h

#define PI 3.141592653

#define MAX_NO_PARTICLES 15000  /* Maximum number of particles/processor */
#define INIT_NO_PARTICLES 1000    /* Initial number of particles/processor */
#define MAX_INITIAL_VELOCITY 50


#define BOX_HORIZ_SIZE 10000.0
#define BOX_VERT_SIZE 10000.0
#define WALL_LENGTH (2.0*BOX_HORIZ_SIZE+2.0*BOX_VERT_SIZE)


struct Wall {
    Wall() {}
    Wall(float x0, float x1, float y0, float y1, bool us, bool ds, bool ls = true, bool rs = true)
        : x0{x0}, x1{x1}, y0{y0}, y1{y1}, upsolid{us}, downsolid{ds}, leftsolid{ls}, rightsolid{rs} {}

    float x0, x1, y0, y1;
    bool upsolid, downsolid, leftsolid, rightsolid;
};

// POD
struct POD_Particle {
    float x, y, vx, vy;
};

struct Particle {
    Particle() {}
    Particle(const POD_Particle& other) {
        x = other.x;
        y = other.y;
        vx = other.vx;
        vy = other.vy;
        collided = false;
    }
    Particle(float x, float y, float vx, float vy) : x{x}, y{y}, vx{vx}, vy{vy} {}

    float x, y, vx, vy;
    bool collided{false};
};
#endif
