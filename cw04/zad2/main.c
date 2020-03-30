#define _POSIX_C_SOURCE 1
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "common.h"

void handler(int signum) {
    // ignore warning about unused variable
    (void)signum;

    puts("Hello from SIGUSR1 handler.");
}


int main(int argc, char* argv[]) {
    if (argc < 2 && argc > 3) {
        fprintf(stderr, "Usage: ./main mode child\n");
        return 1;
    }

    mode_t mode = get_mode(argv[1]);

    int is_exec = argc == 3 && strcmp(argv[2], "exec") == 0;

    if (mode == IGNORE) {
        struct sigaction action;
        action.sa_flags = 0;
        sigemptyset(&action.sa_mask);
        action.sa_handler = SIG_IGN;
        sigaction(SIGUSR1, &action, NULL);
    } else if (mode == HANDLER) {
        struct sigaction action;
        action.sa_flags = 0;
        sigemptyset(&action.sa_mask);
        action.sa_handler = handler;
        sigaction(SIGUSR1, &action, NULL);
    } else {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &mask, NULL);
    }

    puts("Raising inside parent...");
    raise(SIGUSR1);

    if (mode == MASK || mode == PENDING) {
        check_for_pending_signal();
    }

    if (is_exec) {
        char encoded_mode[2];
        sprintf(encoded_mode, "%d", mode);
        execl("./child", "./child", encoded_mode, NULL);
    }

    if (fork() == 0) {
        if (mode != PENDING) {
            puts("Raising inside child...");
            raise(SIGUSR1);
        }

        if (mode == MASK || mode == PENDING) {
            check_for_pending_signal();
        }

        return 0;
    }

    return 0;
}