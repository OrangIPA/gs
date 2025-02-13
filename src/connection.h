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

/**
 * @param sockfd socket file descriptor for the tcp connection
 * @param[out] out_message return owned uint8_t array pointer
 * @param[out] len return 1 byte length of the out_message
 */
int get_message(int sockfd, uint8_t **out_message, size_t *len);

#endif
