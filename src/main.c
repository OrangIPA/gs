#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

#include "connection.h"
#include "state.h"

#define PORT          4000
#define LOOP_DELAY    1000000LL

void *game_loop(void *args);

int main() {
    signal(SIGPIPE, SIG_IGN);

    State state = create_state();

    pthread_t loop_thread;
    if (pthread_create(&loop_thread, NULL, game_loop, NULL) < 0) {
        perror("gameloop");
        return(1);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("websocket (socket)");
        return(1);
    }

    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);

    host_addr.sin_family      = AF_INET;
    host_addr.sin_port        = htons(PORT);
    host_addr.sin_addr.s_addr = htons(INADDR_ANY);

    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    if (bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) != 0) {
        perror("webserver (bind)");
        return(1);
    }

    if (listen(sockfd, SOMAXCONN) != 0) {
        perror("webserver (listen)");
        return(1);
    }
    printf("listening to port: %d\n", PORT);

    while (1) {
        int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
        if (newsockfd < 0) {
            perror("webserver (accept)");
            continue;
        }

        pthread_t thread_id;
        int *     client_sock = malloc(sizeof(int));
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
    return(0);
}

void *game_loop(void *args) {
    struct timeval tv;

    gettimeofday(&tv, NULL);
    long long last_update = tv.tv_sec * 1000000LL + tv.tv_usec;
    long long now         = last_update;
    long long delta       = 0;

    while (1) {
        // MAIN LOOP START
        // printf("main loop\n");
        usleep(1500000);

        // MAIN LOOP END


        // time delay for consisten fps
        gettimeofday(&tv, NULL);
        now = tv.tv_sec * 1000000LL + tv.tv_usec;

        long long i_delta = now - last_update;
        long long i_left  = LOOP_DELAY - i_delta;

        last_update = now;

        // printf("sleeping for %d\n", i_left);
        if (i_left > 0) {
            usleep(i_left);
        }

        gettimeofday(&tv, NULL);
        last_update = tv.tv_sec * 1000000LL + tv.tv_usec;
    }
}
