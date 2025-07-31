#ifndef STATE_H
#define STATE_H

#include "common.h"

#define _POSIX_C_SOURCE 200809L
#include <pthread.h>

typedef enum player_movement { NONE, LEFT, RIGHT } PMovement;

typedef struct player {
    int             fd;
    pthread_mutex_t fd_lock;
    float           pos[2];
    float           vel[2];
    PMovement       movement;
} Player;

typedef struct state {
    int              p_count;
    unsigned int     p_cap;
    Player *         players;

    pthread_rwlock_t lock;
} State;


State create_state();
void print_state(State *state);

void state_newplayer(State *state, int sockfd);
int state_deleteplayer(State *state, int sockfd);

#endif
