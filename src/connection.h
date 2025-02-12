#ifndef CONNECTION_H
#define CONNECTION_H

#include "state.h"

typedef struct connection_args {
    int socket_desc;
    State *state;
} ConnectionArgs;

void *handle_client(void *socket_desc);

/**
 * @param sockfd socket file descriptor for the tcp connection
 * @param[out] out_message return owned uint8_t array pointer
 * @param[out] len return 1 byte length of the out_message
 */
int get_message(int sockfd, uint8_t **out_message, size_t *len);

#endif
