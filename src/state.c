#include "state.h"

#include <stdlib.h>
#include <string.h>

State create_state() {
    State state;

    state.p_count = 0;
    state.p_cap = 1;
    state.pfd_list = malloc(sizeof(int));
    state.ppos_list = malloc(sizeof(float) * 2);
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
        state->ppos_list = realloc(state->ppos_list, sizeof(float) * 2 * new_cap);
    }

    state->pfd_list[state->p_count] = sockfd;
    state->ppos_list[state->p_count * 2] = 0;
    state->ppos_list[state->p_count * 2 + 1] = 0;
}

int state_deleteplayer(State* state, int sockfd) {
    state->p_count -= 1;
    int index = -1;
    for (int i = 0; i < state->p_count; i++) {
        if (state->pfd_list[i] == sockfd) {
            index = i;
        }
    }
    if (index == -1) {
        return -1;
    }

    for (int i = index; i < state->p_count; i++) {
        if (i == state->p_count - 1) {
            state->pfd_list[state->p_count - 1] = 0;
            state->ppos_list[(state->p_count * 2)] = 0;
            continue;
        }

        state->pfd_list[i]  = state->pfd_list[i + 1];
        state->ppos_list[i * 2] = state->ppos_list[(i + 1) * 2];
        state->ppos_list[i * 2 + 1] = state->ppos_list[(i + 1) * 2 + 1];
    }

    return 0;
}