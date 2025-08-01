#include "state.h"

#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <stdlib.h>
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
        printf("sockfd %d [pos: %f, %f] [vel: %f, %f]\n", p->fd, p->pos[0], p->pos[1], p->vel[0], p->vel[1]);
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

    state->players[state->p_count - 1].fd   = sockfd;
    pthread_mutex_init(&state->players[state->p_count - 1].fd_lock, NULL);
    *state->players[state->p_count - 1].pos = *(float[]){ 0, 0 };
    *state->players[state->p_count - 1].vel = *(float[]){ 0, 0 };

    pthread_rwlock_unlock(&state->lock);
}

void player_clear(Player *p) {
    p->fd = 0;
    pthread_mutex_destroy(&p->fd_lock);
    *p->pos = *(float[2]){0, 0};
    *p->vel = *(float[2]){0, 0};
}

int state_deleteplayer(State *state, int sockfd) {
    pthread_rwlock_wrlock(&state->lock);
    int index = -1;
    for (int i = 0; i < state->p_count; i++) {
        if (state->players[i].fd == sockfd) {
            index = i;
        }
    }
    if (index == -1) {
        pthread_rwlock_unlock(&state->lock);
        return -1;
    }

    for (int i = index; i < state->p_count; i++) {
        if (i == state->p_count - 1) {
            player_clear(&state->players[state->p_count - 1]);
            continue;
        }

        state->players[i] = state->players[i + 1];
    }

    state->p_count -= 1;
    pthread_rwlock_unlock(&state->lock);
    return 0;
}
