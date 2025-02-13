#include "update.h"
#include "state.h"

void game_update(State *state, long long delta) {
    int i;

    for (i = 0; i < state->p_count; i++) {
        Player *p = &state->players[i];
        *p->pos += *p->vel;
    }

    // print_state(state);
}
