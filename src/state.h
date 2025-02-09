#ifndef STATE_H
#define STATE_H

// TODO: move pfd and ppos to array of player instead
typedef struct state
{
    int p_count;
    unsigned int p_cap;
    int *pfd_list;
    float *ppos_list;
} State;

State create_state();

void state_newplayer(State* state, int sockfd);
int state_deleteplayer(State* state, int sockfd);

#endif