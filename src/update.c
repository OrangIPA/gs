#include <math.h>
#include <stdio.h>
#include <pthread.h>

#include "update.h"
#include "state.h"
#include "common.h"

void game_update(State *state, long long delta) {
    pthread_rwlock_rdlock(&state->lock);
    printf("Connected clients: %d\n", state->p_count);
    pthread_rwlock_unlock(&state->lock);
    
    int i;
    for (i = 0; i < state->p_count; i++) {
        pthread_rwlock_wrlock(&state->lock);
        Player *p = &state->players[i];
        switch (p->movement) {
        case NONE:
            p->vel[1] = MAX(0, p->vel[1] - DECL_CONST * delta);
            break;

        case LEFT:
            p->vel[1] = MAX(-MAX_SPEED_CONST, p->vel[1] - ACCL_CONST * delta);
            break;

        case RIGHT:
            p->vel[1] = MIN(MAX_SPEED_CONST, p->vel[1] + ACCL_CONST * delta);
            break;

        default:
            perror("Invalid player movement state");
        }

        *p->pos += *p->vel;

        int isChanged = p->vel[0] != 0 || p->vel[1] != 0;
        pthread_rwlock_unlock(&state->lock);

        if (isChanged) {
            // TODO: send position changes to all connected clients
            print_state(state);
        }
    }

}
