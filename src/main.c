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
#include "update.h"

#define PORT          4000
#define LOOP_DELAY    40000LL

void *game_loop(void *state);

int main() {
    signal(SIGPIPE, SIG_IGN);

    State state = create_state();

    pthread_t loop_thread;
    if (pthread_create(&loop_thread, NULL, game_loop, (void *)&state) < 0) {
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
        ConnectionArgs *param = malloc(sizeof(ConnectionArgs));
        param->socket_desc = newsockfd;
        param->state = &state;

        if (pthread_create(&thread_id, NULL, handle_client, (void *)param) < 0) {
            perror("couldn't create thread");
            close(newsockfd);
            free(param);
            continue;
        }

        pthread_detach(thread_id);
    }
    printf("process exited!");
    return(0);
}

void *game_loop(void *state) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long last_update = tv.tv_sec * 1000000LL + tv.tv_usec;
    long long now;
    
    while (1) {
        // MAIN LOOP START
        gettimeofday(&tv, NULL);
        now = tv.tv_sec * 1000000LL + tv.tv_usec;
        long long delta = now - last_update;  // This is your delta time in microseconds
        
        game_update((State *)state, delta);
        
        // MAIN LOOP END
        
        // Calculate sleep time for consistent frame rate
        long long time_left = LOOP_DELAY - delta;
        if (time_left > 0) {
            usleep(time_left);
        }
        
        last_update = now;
    }
}