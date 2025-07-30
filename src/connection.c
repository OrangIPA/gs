#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "connection.h"
#include "state.h"
#include "net.h"

#define CLIENT_DISCONNECT    -2

void *handle_client(void *conn_param) {
    ConnectionArgs args         = *(ConnectionArgs *)conn_param;
    char           buffer[1024] = { 0 };

    free(conn_param);
    printf("new connection!\n");

    state_newplayer(args.state, args.socket_desc);

    while (1) {
        size_t   message_length;
        uint8_t *message;
        int      status = recv_message(args.socket_desc, &message_length, &message);
        if (status == CLIENT_DISCONNECT) {
            printf("Client Disconnect\n");
            break;
        }

        pthread_rwlock_wrlock(&args.state->lock);
        Player *player = NULL;
        for (int i = 0; i < args.state->p_count; i++) {
            if (args.state->players[i].fd == args.socket_desc) {
                player = &args.state->players[i];
            }
        }
        if (player == NULL) {
            perror("assertion failed, player is NULL");
            break;
        }

        switch (message[0]) {
        case OP_PMOVL:
            player->movement = LEFT;
            break;

        case OP_PMOVR:
            player->movement = RIGHT;
            break;

        case OP_PSTOP:
            player->movement = NONE;
            break;

        default:
            break;
        }
        pthread_rwlock_unlock(&args.state->lock);

        char *reply = malloc(message_length * 2 + 1);
        for (int i = 0; i < message_length; i++) {
            sprintf((reply + i * 2), "%02X", (int)message[i]);
        }
        reply[message_length * 2] = '\0';
        printf("%d: %s\n", args.socket_desc, reply);

        free(message);
        free(reply);
    }

    printf("close connection\n");
    int err = state_deleteplayer(args.state, args.socket_desc);
    if (err != 0) {
        printf("error occured while deleting player");
    }
    shutdown(args.socket_desc, SHUT_RDWR);
    close(args.socket_desc);

    return (void *)0;
}
