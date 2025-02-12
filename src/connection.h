#ifndef CONNECTION_H
#define CONNECTION_H

#include "state.h"

typedef struct connection_args {
    int socket_desc;
    State *state;
} ConnectionArgs;

void *handle_client(void *socket_desc);

#endif
