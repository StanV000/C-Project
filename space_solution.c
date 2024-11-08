#include "space_explorer.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    unsigned int *visited;
    double *mixer_distances;
    unsigned int visited_count;
    unsigned int visited_size;
    unsigned int best_planet_id;
    unsigned int *best_connections;
} ShipState;

void* init_ship_state() {
    ShipState *state = (ShipState *)malloc(sizeof(ShipState));
    if (!state) {
        return NULL;
    }
    state->visited_size = 10;
    state->visited = (unsigned int *)malloc(state->visited_size * sizeof(unsigned int));
    state->mixer_distances = (double *)malloc(state->visited_size * sizeof(double));
    if (!state->visited || !state->mixer_distances) { // Check if malloc succeeded
        free(state->visited);
        free(state->mixer_distances);
        free(state);
        return NULL;
    }
    state->visited_count = 0;
    state->best_planet_id = 0;
    return state;
}

int is_visited(ShipState *state, unsigned int planet_id) {
    for (unsigned int i = 0; i < state->visited_count; i++) {
        if (state->visited[i] == planet_id) {
            return 1;
        }
    }
    return 0;
}

void add_visited(ShipState *state, unsigned int planet_id, double mixer_distance) {
    if (is_visited(state, planet_id)) {
        return;
    }
    if (state->visited_count == state->visited_size) {
        unsigned int new_size = state->visited_size * 2;
        unsigned int *new_visited = realloc(state->visited, new_size * sizeof(unsigned int));
        double *new_mixer_distances = realloc(state->mixer_distances, new_size * sizeof(double));
        if (!new_visited || !new_mixer_distances) {
            return;
        }
        state->visited = new_visited;
        state->mixer_distances = new_mixer_distances;
        state->visited_size = new_size;
    }
    state->visited[state->visited_count] = planet_id;
    state->mixer_distances[state->visited_count++] = mixer_distance;
}

void remove_visited(ShipState *state, unsigned int planet_id) {
    int found_index = -1;
    for (unsigned int i = 0; i < state->visited_count; i++) {
        if (state->visited[i] == planet_id) {
            found_index = i;
            break;
        }
    }

    if (found_index != -1) {

        for (unsigned int i = found_index; i < state->visited_count - 1; i++) {
            state->visited[i] = state->visited[i + 1];
            state->mixer_distances[i] = state->mixer_distances[i + 1];
        }
        // Decrement the count of visited planets
        state->visited_count--;
    }
}

ShipAction space_hop(unsigned int crt_planet, unsigned int *connections, int num_connections, double distance_from_mixer, void *ship_state) {
    ShipState *state = (ShipState *) ship_state;
    if (!state) {
        state = init_ship_state();
        if (!state) return (ShipAction) {RAND_PLANET, NULL};
    }

    if (!is_visited(state, crt_planet)) {
        add_visited(state, crt_planet, distance_from_mixer);
    }


    if (distance_from_mixer > 5) {
        return (ShipAction) {RAND_PLANET, state};
    }

    double last_distance = 9999999999;
    unsigned int next_planet = RAND_PLANET;


    if (state->visited_count >= 2 && distance_from_mixer > state->mixer_distances[state->visited_count - 2]) {

        next_planet = state->visited[state->visited_count - 2];
        remove_visited(state, next_planet);
    }else {
        for (int i = 0; i < num_connections; i++) {
            if (!is_visited(state, connections[i])) {
                if (distance_from_mixer <= last_distance) {
                    next_planet = connections[i];
                    state->best_planet_id = crt_planet;
                    break;
                }
            }
        }

    }


    //BACKTRACKING if connections = 0 and visited
    if (num_connections == 0)  {
        next_planet = state->visited[state->visited_count - 2];
        remove_visited(state, state->visited[state->visited_count - 2]);
    }



    //PRINT FUNCTIONS----------------------------------------
    // Print status of connections
    printf("Distance To Mixer %f\n", distance_from_mixer);
    for(int i = 0; i < num_connections; i++) {
        printf("COnnections %u\n", connections[i]);

    }

    printf("\n");
    printf("Status of connections: ");
    for (int i = 0; i < num_connections; i++) {
        if (is_visited(state, connections[i])) {
            printf("Visited ");
        } else {
            printf("Unvisited ");
        }
    }
    printf("\n");

    printf("Current Planet %d\n", crt_planet);
    printf("How many Connections %d\n", num_connections);

    // Print all distances to the mixer for each visited planet
    printf("Visited mixer distances: ");
    for (unsigned int i = 0; i < state->visited_count; i++) {
        printf("%f ", state->mixer_distances[i]);
    }
    printf("\n");




    ShipAction action;
    action.next_planet = next_planet;
    action.ship_state = state;
    return action;
}
