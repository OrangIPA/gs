#ifndef STATE_H
#define STATE_H

#include <pthread.h>

typedef struct player {
    int   fd;
    float pos[2];
    float vel[2];
} Player;

typedef struct state {
    int              p_count;
    unsigned int     p_cap;
    Player *         players;

    pthread_rwlock_t lock;
} State;


State create_state();

void state_newplayer(State *state, int sockfd);
int state_deleteplayer(State *state, int sockfd);

#endif
