#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>

void *handle_client(void *socket_desc) {
    int client_sock = *(int*)socket_desc;
    char buffer[1024] = {0};
    free(socket_desc);
    printf("new connection!\n");

    while (1)
    {
        uint8_t message_length;
        int valread = read(client_sock, &message_length, 1);
        if (valread == 0) {
            printf("disconnect!\n");
            break;
        }
        if (valread < 0) {
            perror("what?");
            continue;
        }
        uint8_t* message = malloc(message_length);
        printf("buf_length: %d\n", message_length);

        int totalread = 0;
        int err = 0;
        while (1) {
            uint8_t buf;
            int valread = read(client_sock, &buf, 1);
            if (valread == 0) 
                continue;
            message[totalread] = buf;

            totalread += valread;
            if (totalread > (int)message_length) {
                err = 1;
                break;
            }
            if (totalread == (int)message_length) 
                break;
        }

        if (err) {
            perror("tcp read error");
            break;
        }

        char* reply = malloc(message_length * 2 + 1);
        for (int i = 0; i < message_length; i++) {
            sprintf((reply + i*2),"%02X", (int)message[i]);
        }
        reply[message_length * 2] = '\0';
        ssize_t bytes_written = write(client_sock, reply, message_length * 2 + 1);
        if (bytes_written < 0) {
            perror("webserver (write)");
        }

        free(message);
        free(reply);
    }

    printf("close connection");
    shutdown(client_sock, SHUT_RDWR);
    close(client_sock);

    return (void*)0;
}