#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "net.h"

int send_message(int fd, size_t len, uint8_t *message) {
    if (len > 0xFB) {
        PANIC("message length greater than 251 not implemented");
    }

    uint8_t *buf = malloc(len + 1);
    buf[0] = len;
    memcpy(buf + 1, message, len);

    if (send(fd, buf, len + 1, 0) < 0) {
        perror("send");
    }

    return 0;
}

int recv_message(int fd, size_t *len, uint8_t **out_message) {
    int valread = read(fd, len, 1);

    if (*len > 0xFB) {
        PANIC("message length greater than 251 not implemented");
    }

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
        int     valread = read(fd, &buf, 1);
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