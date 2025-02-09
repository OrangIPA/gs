#include "state.h"

#include <stdlib.h>

State create_state() {
    State state;

    state.p_count = 0;
    state.p_cap = 1;
    state.pfd_list = malloc(sizeof(int));
    state.ppos_list = malloc(sizeof(float[2]));
}

void state_newplayer(State* state, int sockfd) {
    state->p_count += 1;

    // If p_count is bigger than capacity, allocate more capacity
    // Specifically, 2^n
    if (state->p_count > state->p_cap) {
        unsigned int new_cap = 1;
        while (new_cap < state->p_count) {
            new_cap *= 2;
        }

        state->p_cap = new_cap;
        state->pfd_list = realloc(state->pfd_list, sizeof(int) * new_cap);
        state->ppos_list = realloc(state->ppos_list, sizeof(int[2]) * new_cap);
    }

    state->pfd_list[state->p_count] = sockfd;
    *state->ppos_list[state->p_count] = *(float[2]){0.0, 0.0};
}