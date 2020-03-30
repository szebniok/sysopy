#define _POSIX_C_SOURCE 1
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "common.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./child mode\n");
        return 1;
    }

    mode_t mode = atoi(argv[1]);

    if (mode != PENDING) {
        puts("Raising inside child...");
        raise(SIGUSR1);
    }

    if (mode == MASK || mode == PENDING) {
        check_for_pending_signal();
    }
}