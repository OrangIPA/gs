#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "connection.h"

#define CLIENT_DISCONNECT -2

void *handle_client(void *conn_param) {
    ConnectionArgs  args  = *(ConnectionArgs *)conn_param;
    char buffer[1024] = { 0 };

    free(conn_param);
    printf("new connection!\n");

    while (1) {
        size_t message_length;
        uint8_t *message;
        int status = get_message(args.socket_desc, &message, &message_length);
        if (status == CLIENT_DISCONNECT) {
            printf("Client Disconnect\n");
            break;
        }

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

    return((void *)0);
}

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
    int is_err = 0;
    while (1) {
        uint8_t buf;
        int valread = read(sockfd, &buf, 1);
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