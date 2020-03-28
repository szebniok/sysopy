#ifndef COMMON_H
#define COMMON_H

#include <string.h>

typedef enum { KILL, SIGQUEUE, SIGRT } mode_t;

mode_t get_mode(char* encoded_mode) {
    if (strcmp(encoded_mode, "KILL") == 0) return KILL;
    if (strcmp(encoded_mode, "SIGQUEUE") == 0) return SIGQUEUE;
    return SIGRT;
}

#define SIG1(X) (X == SIGRT ? SIGRTMIN : SIGUSR1)
#define SIG2(X) (X == SIGRT ? SIGRTMIN + 1 : SIGUSR2)

#endif