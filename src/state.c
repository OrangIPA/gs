#include "state.h"

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

State create_state() {
    State state;

    state.p_count = 0;
    state.p_cap   = 1;
    state.players = malloc(sizeof(Player));

    pthread_rwlockattr_t attr;
    pthread_rwlockattr_init(&attr);
    pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
    pthread_rwlock_init(&state.lock, &attr);

    return state;
}

void print_state(State *state) {
    pthread_rwlock_rdlock(&state->lock);
    printf("player count: %d\n", state->p_count);
    printf("player capacity: %d\n", state->p_cap);
    for (int i = 0; i < state->p_count; i++) {
        Player *p = &state->players[i];
        printf("sockfd %d [pos: %f, %f] [vel: %f, %f]", p->fd, p->pos[0], p->pos[1], p->vel[0], p->vel[1]);
    }

    pthread_rwlock_unlock(&state->lock);
}

void state_newplayer(State *state, int sockfd) {
    pthread_rwlock_wrlock(&state->lock);
    state->p_count += 1;

    //  If p_count is bigger than capacity, allocate more capacity
    // Specifically, 2^n
    if (state->p_count > state->p_cap) {
        unsigned int new_cap = 1;
        while (new_cap < state->p_count) {
            new_cap *= 2;
        }

        state->p_cap   = new_cap;
        state->players = realloc(state->players, sizeof(Player) * state->p_cap);
    }

    state->players[state->p_count].fd   = 0;
    *state->players[state->p_count].pos = *(float[]){ 0, 0 };
    *state->players[state->p_count].vel = *(float[]){ 0, 0 };

    pthread_rwlock_unlock(&state->lock);
}

int state_deleteplayer(State *state, int sockfd) {
    pthread_rwlock_wrlock(&state->lock);
    state->p_count -= 1;
    int index = -1;
    for (int i = 0; i < state->p_count; i++) {
        if (state->players[i].fd == sockfd) {
            index = i;
        }
    }
    if (index == -1) {
        return -1;
    }

    for (int i = index; i < state->p_count; i++) {
        if (i == state->p_count - 1) {
            state->players[state->p_count - 1].fd   = 0;
            *state->players[state->p_count - 1].pos = *(float[2]){ 0, 0 };
            *state->players[state->p_count - 1].vel = *(float[2]){ 0, 0 };
            continue;
        }

        state->players[i] = state->players[i + 1];
    }

    pthread_rwlock_unlock(&state->lock);
    return 0;
}
