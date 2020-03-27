#define _POSIX_C_SOURCE 199309L
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
    char *mode = argv[3];

    struct sigaction sigusr1_action;
    if (strcmp(mode, "KILL") == 0) {
        sigusr1_action.sa_sigaction = sigusr1;
    } else {
        sigusr1_action.sa_sigaction = sigusr1_sigqueue;
    }
    sigusr1_action.sa_flags = SA_SIGINFO;
    sigfillset(&sigusr1_action.sa_mask);
    sigdelset(&sigusr1_action.sa_mask, SIGUSR1);
    sigdelset(&sigusr1_action.sa_mask, SIGUSR2);

    struct sigaction sigusr2_action;
    sigusr2_action.sa_sigaction = sigusr2;
    sigusr2_action.sa_flags = SA_SIGINFO;
    sigfillset(&sigusr2_action.sa_mask);
    sigdelset(&sigusr2_action.sa_mask, SIGUSR1);
    sigdelset(&sigusr2_action.sa_mask, SIGUSR2);

    sigaction(SIGUSR1, &sigusr1_action, NULL);
    sigaction(SIGUSR2, &sigusr2_action, NULL);

    for (int i = 0; i < count; i++) {
        kill(pid, SIGUSR1);
    }
    kill(pid, SIGUSR2);

    while (!got_last_reply) {
    }

    printf("%d Sent %d signals, got back %d\n", getpid(), count, replies_count);
}