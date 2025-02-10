#ifndef STATE_H
#define STATE_H

#include <pthread.h>
#include <stdio.h>

typedef struct Player;

typedef struct state {
    int              p_count;
    unsigned int     p_cap;
    Player *         players;

    pthread_rwlock_t lock;
} State;

typedef struct player {
    int   pfd;
    float ppos[2];
} Player;

State create_state();

void state_newplayer(State *state, int sockfd);
int state_deleteplayer(State *state, int sockfd);

#endif
