#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>

#define ACCL_CONST         1
#define DECL_CONST         1.5
#define MAX_SPEED_CONST    10

#define MIN(a, b)    ((a) < (b) ? (a) : (b))
#define MAX(a, b)    ((a) > (b) ? (a) : (b))
#define PANIC(msg, ...) \
    do { \
        fprintf(stderr, "PANIC at %s:%d in %s(): " msg "\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
        abort(); \
    } while (0)

#endif
