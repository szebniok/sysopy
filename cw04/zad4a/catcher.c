#define _POSIX_C_SOURCE 199309L
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int sender_pid = 0;
int sigusr1_count = 0;

void sigusr1(int signum) {
    // ignore warning about unused parameter
    (void)signum;

    sigusr1_count++;
}

void sigusr2(int signum, siginfo_t *info, void *context) {
    // ignore warning about unused parameter
    (void)signum;
    (void)context;

    sender_pid = info->si_pid;
}

int main() {
    sigset_t block_mask;
    sigfillset(&block_mask);
    sigdelset(&block_mask, SIGUSR1);
    sigdelset(&block_mask, SIGUSR2);
    sigprocmask(SIG_SETMASK, &block_mask, NULL);

    struct sigaction sigusr1_action;
    sigusr1_action.sa_handler = sigusr1;
    sigusr1_action.sa_flags = 0;
    sigemptyset(&sigusr1_action.sa_mask);
    sigaction(SIGUSR1, &sigusr1_action, NULL);

    struct sigaction sigusr2_action;
    sigusr2_action.sa_sigaction = sigusr2;
    sigusr2_action.sa_flags = SA_SIGINFO;
    sigemptyset(&sigusr2_action.sa_mask);
    sigaction(SIGUSR2, &sigusr2_action, NULL);

    printf("%d\n", getpid());

    while (sender_pid == 0) {
    }

    printf("received %d signals\n", sigusr1_count);

    for (int i = 0; i < sigusr1_count; i++) {
        sigqueue(sender_pid, SIGUSR1, (union sigval){.sival_int = i});
    }
    kill(sender_pid, SIGUSR2);
}