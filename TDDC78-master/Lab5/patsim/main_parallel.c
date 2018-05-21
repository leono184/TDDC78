#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <mpi.h>

#include "definitions.h"
#include "physics.h"

float rand1(){
	return (float)(rand()/(float) RAND_MAX);
}

void reset_collision(particle_node_t* particles) {
	particle_node_t* current_particle = particles;
	while (current_particle) {
		current_particle->collided = false;
		current_particle = current_particle->next;
	}
}

void particle_collisions(particle_node_t* particles) {
    particle_node_t* current_particle = particles;
    while (current_particle) {
        if (!current_particle->collided) {
            /* check for collisions */
            particle_node_t* colliding_particle = current_particle->next;
            while (colliding_particle) {
                if (!colliding_particle->collided) {
                    float t = collide(&(current_particle->particle), &(colliding_particle->particle));
                    if (t!=-1) { // collision
                        current_particle->collided = true;
                        colliding_particle->collided = true;
                        interact(&(current_particle->particle), &(colliding_particle->particle), t);
                        colliding_particle = colliding_particle->next;
                        break; // only check collision of two particles
                    }
                }
                colliding_particle = colliding_particle->next;
            }
        }
        current_particle = current_particle->next;
    }
}

void free_particles(particle_node_t* particles) {
    particle_node_t* current_particle = particles;
    particle_node_t* next_particle;
    while (current_particle) {
        next_particle = current_particle->next;
        free(current_particle);
        current_particle = next_particle;
    }
}

void init_mpi_particle(MPI_Datatype* mpi_particle) {
    // Element of the type
    int block_lengths[] = {1,1,1,1}; // Lengths of type elements
    MPI_Datatype block_types[] = {MPI_FLOAT,MPI_FLOAT,MPI_FLOAT,MPI_FLOAT}; //Set types
    MPI_Aint start, displ[4];

    particle_t item;
    MPI_Address(&item, &start);
    MPI_Address(&item.x, &displ[0]);
    MPI_Address(&item.y, &displ[1]);
    MPI_Address(&item.vx, &displ[2]);
    MPI_Address(&item.vy, &displ[3]);

    displ[0] -= start; // Displacement relative to address of start
    displ[1] -= start;
    displ[2] -= start;
    displ[3] -= start;
    MPI_Type_struct(4, block_lengths, displ, block_types, mpi_particle);
    MPI_Type_commit(mpi_particle);
}

void init_particles(particle_node_t* particles, particle_node_t* last_particle, wall_t wall) {
	float r, a;

	// Seed random generator
	srand(time(NULL) + 1234);

	particle_node_t* current_particle = particles;
	for(int i=0; i<INIT_NO_PARTICLES; i++){
		// initialize random position
		current_particle->particle.x = wall.x0 + rand1()*(wall.x1-wall.x0);
		current_particle->particle.y = wall.y0 + rand1()*(wall.y1-wall.y0);

		// initialize random velocity
		r = rand1()*MAX_INITIAL_VELOCITY;
		a = rand1()*2*PI;
		current_particle->particle.vx = r*cos(a);
		current_particle->particle.vy = r*sin(a);

		if (i == INIT_NO_PARTICLES-1) {
			current_particle->next = NULL;
		} else {
			current_particle->next = malloc(sizeof(particle_node_t));
			current_particle = current_particle->next;
		}
	}
    last_particle = current_particle;
}

wall_t create_wall(int rank, int world_size) {
	// Set the walls
	wall_t wall;
	wall.x0 = 0.0;
	wall.x1 = BOX_HORIZ_SIZE;
	wall.y0 = 0;
	wall.y1 = BOX_VERT_SIZE/world_size;

	wall.leftsolid = true;
	wall.rightsolid = true;
	wall.upsolid = (rank == 0);
	wall.downsolid = (rank == world_size-1);

    return wall;
}

int main(int argc, char** argv){
	unsigned int time_stamp = 0, time_max;
	float pressure;
    int rank, world_size;
    MPI_Status msg_status;
	particle_node_t* particles = malloc(sizeof(particle_node_t));
    particle_node_t* last_particle = NULL;
    wall_t wall;

	// parse arguments
	if(argc != 2) {
		fprintf(stderr, "Usage: %s simulation_time\n", argv[0]);
		fprintf(stderr, "For example: %s 10\n", argv[0]);
		exit(1);
	}

	time_max = atoi(argv[1]);

	/* Initialize */
	// Initialize MPI and create MPI_Datatpye
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Saves the number of processes in world_size
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Saves the rank in rank

    // Create a MPI datatype
    MPI_Datatype mpi_particle; // MPI type to commit
    init_mpi_particle(&mpi_particle);

    // Create wall
    wall = create_wall(rank,world_size);

    // Create NO_PARTICLES particles as a linked list
    init_particles(particles,last_particle,wall);

	/* Main loop */
	for (time_stamp=0; time_stamp<time_max; time_stamp++) { // for each time stamp
        reset_collision(particles);
        particle_collisions(particles);
        // move particles that has not collided with another
        particle_node_t* previous_particle = NULL;
        particle_node_t* current_particle = particles;
        while (current_particle) {
            if (!current_particle->collided) {
                // Move particles
                feuler(&(current_particle->particle), 1);

                /* check for wall interaction and add the momentum */
                pressure += wall_collide(&(current_particle->particle), wall);

            }
    		// Check what particles that is to be sent to neighbours
    		// Can only sent up or down since partitions is row wise
    	    if (!wall.upsolid && current_particle->particle.y < wall.y0) { // To neighbour above
    	    }
    	    if (!wall.downsolid && current_particle->particle.y > wall.y1) { // To neighbour below
    	    }
            previous_particle = current_particle;
            current_particle = current_particle->next;
        }
	}

	printf("Average pressure = %f, Rank: %d\n", pressure / (WALL_LENGTH*time_max), rank);

	MPI_Finalize();
	return 0;

}
