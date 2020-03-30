#define _POSIX_C_SOURCE 199309L
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "common.h"

int got_last_reply = 0;
int replies_count = 0;

void sigusr1(int signum, siginfo_t *info, void *context) {
    // ignore warning about unused parameter
    (void)signum;
    (void)info;
    (void)context;

    replies_count++;
}

void sigusr1_sigqueue(int signum, siginfo_t *info, void *context) {
    // ignore warning about unused parameter
    (void)signum;
    (void)context;

    printf("Got signal no. %d\n", info->si_value.sival_int);
    replies_count++;
}

void sigusr2(int signum, siginfo_t *info, void *context) {
    // ignore warning about unused parameter
    (void)signum;
    (void)info;
    (void)context;

    got_last_reply = 1;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: ./sender pid count mode\n");
        return 1;
    }

    int pid = atoi(argv[1]);
    int count = atoi(argv[2]);
    mode_t mode = get_mode(argv[3]);

    sigset_t block_mask;
    sigfillset(&block_mask);
    sigdelset(&block_mask, SIG1(mode));
    sigdelset(&block_mask, SIG2(mode));
    sigprocmask(SIG_SETMASK, &block_mask, NULL);

    struct sigaction sigusr1_action;
    sigusr1_action.sa_sigaction = mode == SIGQUEUE ? sigusr1_sigqueue : sigusr1;
    sigusr1_action.sa_flags = SA_SIGINFO;
    sigemptyset(&sigusr1_action.sa_mask);
    sigaction(SIG1(mode), &sigusr1_action, NULL);

    struct sigaction sigusr2_action;
    sigusr2_action.sa_sigaction = sigusr2;
    sigusr2_action.sa_flags = SA_SIGINFO;
    sigemptyset(&sigusr2_action.sa_mask);
    sigaction(SIG2(mode), &sigusr2_action, NULL);

    for (int i = 0; i < count; i++) {
        if (mode == SIGQUEUE) {
            sigqueue(pid, SIG1(mode), (union sigval){.sival_int = 0});
        } else {
            kill(pid, SIG1(mode));
        }
    }
    if (mode == SIGQUEUE) {
        sigqueue(pid, SIG2(mode), (union sigval){.sival_int = 0});
    } else {
        kill(pid, SIG2(mode));
    }

    while (!got_last_reply) {
    }

    printf("Sent %d signals, got back %d\n", count, replies_count);
}