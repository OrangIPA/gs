#ifndef STATE_H
#define STATE_H

#include <pthread.h>

typedef enum player_movement { NONE, LEFT, RIGHT } PMovement;

typedef struct player {
    int       fd;
    float     pos[2];
    float     vel[2];
    PMovement movement;
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
