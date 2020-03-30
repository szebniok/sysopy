#ifndef COMMON_H
#define COMMON_H

#include <string.h>
#include <signal.h>

typedef enum { IGNORE, HANDLER, MASK, PENDING } mode_t;

mode_t get_mode(char* encoded_mode) {
    if (strcmp(encoded_mode, "ignore") == 0) return IGNORE;
    if (strcmp(encoded_mode, "handler") == 0) return HANDLER;
    if (strcmp(encoded_mode, "mask") == 0) return MASK;
    return PENDING;
}

void check_for_pending_signal() {
    sigset_t set;
    sigpending(&set);
    printf("Pending SIGUSR1 signal: %s\n",
           sigismember(&set, SIGUSR1) ? "true" : "false");
}

#endif