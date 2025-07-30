#ifndef CONNECTION_H
#define CONNECTION_H

#include "state.h"

typedef struct connection_args {
    int    socket_desc;
    State *state;
} ConnectionArgs;

typedef enum {
    /**
     * PLAYER MOVEMENTS
     *
     * OP_PMOVL (0x01), OP_PMOVR (0x02), OP_PSTOP (0x03)
     * Player Move Left, Right, or Stop
     * Doesn't have any following bytes, ignored if exist
     */
    OP_PMOVL = 0x01,
    OP_PMOVR = 0x02,
    OP_PSTOP = 0x03,
} OpCode;

void *handle_client(void *socket_desc);

#endif
