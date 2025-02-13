#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "connection.h"

#define CLIENT_DISCONNECT    -2

void *handle_client(void *conn_param) {
    ConnectionArgs args         = *(ConnectionArgs *)conn_param;
    char           buffer[1024] = { 0 };

    free(conn_param);
    printf("new connection!\n");

    while (1) {
        size_t   message_length;
        uint8_t *message;
        int      status = get_message(args.socket_desc, &message, &message_length);
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
        ssize_t bytes_written = write(args.socket_desc, reply, message_length * 2 + 1);
        if (bytes_written < 0) {
            perror("webserver (write)");
        }

        free(message);
        free(reply);
    }

    printf("close connection");
    shutdown(args.socket_desc, SHUT_RDWR);
    close(args.socket_desc);

    return (void *)0;
}

/**
 * @brief  Retrive tcp messages by TLV protocol without type
 *
 * The protocol is, each message have length, and body.
 * The first bytes is the length and the rest is the body.
 *
 * since max value of 8 bit uint is 255 (0xFF), message length can't exceed 248 (0xF8)
 * bytes 248 (0xF8) and above is reserved for futureproofing (i'm thinking of doing it utf-8 style)
 *
 * ## example
 * ```
 * 0    1    2    3    4    5    6    7    8    9    A    ..   F
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+
 * | 02 | 7F | 28 | 01 | 01 | A0 | 00 | 3F | 9A | 90 | 45 | .. | F4
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+
 * Len  Dat  Dat  Len  Dat  Len  Dat  Dat  Dat  Dat  Dat  ..   Dat
 * ```
 * Above stream contains 3 messages:
 * `0x7F28` with length 0x02, `0x01` with length 0x01, and `0x003F9A9045..F4` with length 0xA0
 *
 */
int get_message(int sockfd, uint8_t **out_message, size_t *len) {
    int valread = read(sockfd, len, 1);

    if (valread == 0) {
        return CLIENT_DISCONNECT;
    }
    if (valread < 0) {
        return -1;
    }

    *out_message = malloc(*len);

    int totalread = 0;
    int is_err    = 0;
    while (1) {
        uint8_t buf;
        int     valread = read(sockfd, &buf, 1);
        if (valread == 0) {
            continue;
        }
        (*out_message)[totalread] = buf;

        totalread += valread;
        if (totalread > (int)*len) {
            is_err = 1;
            break;
        }
        if (totalread == (int)*len) {
            break;
        }
    }

    if (is_err) {
        return -1;
    }

    return -1;
}
