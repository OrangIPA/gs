#ifndef STATE_H
#define STATE_H

typedef struct state
{
    int p_count;
    unsigned int p_cap;
    int *pfd_list;
    float (*ppos_list)[2];
} State;

State create_state();

void state_newplayer(State* state, int sockfd);

#endif