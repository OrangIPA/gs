#include "state.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

State create_state() {
    State state;

    state.p_count = 0;
    state.p_cap   = 1;
    state.players = malloc(sizeof(Player));

    pthread_rwlock_init(&state.lock, NULL);
}

void state_newplayer(State *state, int sockfd) {
    pthread_rwlock_wrlock(&state->lock);
    state->p_count += 1;

    /**
     * If p_count is bigger than capacity, allocate more capacity
     * Specifically, 2^n
     */
    if (state->p_count > state->p_cap) {
        unsigned int new_cap = 1;
        while (new_cap < state->p_count) {
            new_cap *= 2;
        }

        state->p_cap   = new_cap;
        state->players = realloc(state->players, sizeof(Player) * state->p_cap);
    }

    state->players[state->p_count].pfd     = 0;
    state->players[state->p_count].ppos[0] = 0;
    state->players[state->p_count].ppos[1] = 0;
    pthread_rwlock_unlock(&state->lock);
}

int state_deleteplayer(State *state, int sockfd) {
    pthread_rwlock_wrlock(&state->lock);
    state->p_count -= 1;
    int index = -1;
    for (int i = 0; i < state->p_count; i++) {
        if (state->players[i].pfd == sockfd) {
            index = i;
        }
    }
    if (index == -1) {
        return -1;
    }

    for (int i = index; i < state->p_count; i++) {
        if (i == state->p_count - 1) {
            state->players[state->p_count - 1].pfd     = 0;
            state->players[state->p_count - 1].ppos[0] = 0;
            state->players[state->p_count - 1].ppos[1] = 0;
            continue;
        }

        state->players[i] = state->players[i + 1];
    }

    pthread_rwlock_unlock(&state->lock);
    return 0;
}
