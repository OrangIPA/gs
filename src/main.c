#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>

#define PORT 4000
#define LOOP_DELAY 1000000LL

void *handle_client(void *socket_desc);
void *game_loop(void *args);

int main() {
    signal(SIGPIPE, SIG_IGN);

    pthread_t loop_thread;
    if (pthread_create(&loop_thread, NULL, game_loop, NULL) < 0) {
        perror("gameloop");
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("websocket (socket)");
        return 1;
    }

    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htons(INADDR_ANY);

    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    if (bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) != 0) {
        perror("webserver (bind)");
        return 1;
    }

    if (listen(sockfd, SOMAXCONN) != 0) {
        perror("webserver (listen)");
        return 1;
    }
    printf("listening to port: %d\n", PORT);

    while (1)
    {
        int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
        if (newsockfd < 0) {
            perror("webserver (accept)");
            continue;
        }

        pthread_t thread_id;
        int *client_sock = malloc(sizeof(int));
        *client_sock = newsockfd;

        if (pthread_create(&thread_id, NULL, handle_client, (void *)client_sock) < 0) {
            perror("couldn't create thread");
            close(newsockfd);
            free(client_sock);
            continue;
        }

        pthread_detach(thread_id);
    }
    printf("process exited!");
    return 0;
}

void *handle_client(void *socket_desc) {
    int client_sock = *(int*)socket_desc;
    char buffer[1024] = {0};
    free(socket_desc);
    printf("new connection!\n");

    while (1)
    {        
        uint8_t message_length;
        int valread = read(client_sock, &message_length, 1);
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

clean_up:
        free(message);
        free(reply);
    }

    shutdown(client_sock, SHUT_RDWR);
    close(client_sock);
}

void *game_loop(void *args) {
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    long long last_update = tv.tv_sec * 1000000LL + tv.tv_usec;
    long long now = last_update;
    long long delta = 0;

    while(1) {
        

        // MAIN LOOP START
        // printf("main loop\n");
        usleep(1500000);

        // MAIN LOOP END


        // time delay for consisten fps
        gettimeofday(&tv, NULL);
        now = tv.tv_sec * 1000000LL + tv.tv_usec;

        long long i_delta = now - last_update;
        long long i_left = LOOP_DELAY - i_delta;

        last_update = now;

        // printf("sleeping for %d\n", i_left);
        if (i_left > 0) {
            usleep(i_left);
        }

        gettimeofday(&tv, NULL);
        last_update = tv.tv_sec * 1000000LL + tv.tv_usec;
    }
}