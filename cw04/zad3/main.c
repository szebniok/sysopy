#define _POSIX_C_SOURCE 200809L
#include <fenv.h>
#include <limits.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void sigchld(int signum, siginfo_t *info, void *context) {
    (void)signum;
    (void)context;

    puts("Scenario 1: si_status");
    printf("Child process %d exited with return code %d\n\n", info->si_pid,
           info->si_status);
}

void sigvalue(int signum, siginfo_t *info, void *context) {
    (void)signum;
    (void)context;

    puts("Scenario 2: si_value");
    if (info->si_code == SI_QUEUE) {
        puts("Signal was sent using sigqueue");
    }
    printf("Attached value: %d\n\n", info->si_value.sival_int);
}

void sigtimer(int signum, siginfo_t *info, void *context) {
    (void)signum;
    (void)context;

    puts("Scenario 3: si_code");
    if (info->si_code == SI_TIMER) {
        int *timer_id = (int *)info->si_value.sival_ptr;
        printf("Signal was sent by timer with id of %d\n\n", *timer_id);
    } else if (info->si_code == SI_USER) {
        puts("SIGALRM was triggered by user\n");
    }
}

int main() {
    // scenario 1
    struct sigaction sigchld_action;
    sigchld_action.sa_sigaction = sigchld;
    sigchld_action.sa_flags = SA_SIGINFO;
    sigemptyset(&sigchld_action.sa_mask);
    sigaction(SIGCHLD, &sigchld_action, NULL);

    if (fork() == 0) {
        return 77;
    }

    // scenario 2
    struct sigaction sigvalue_action;
    sigvalue_action.sa_sigaction = sigvalue;
    sigvalue_action.sa_flags = SA_SIGINFO;
    sigemptyset(&sigvalue_action.sa_mask);
    sigaction(SIGUSR1, &sigvalue_action, NULL);

    sigqueue(getpid(), SIGUSR1, (union sigval){.sival_int = 1998});

    // scenario 3
    struct sigaction sigtimer_action;
    sigtimer_action.sa_sigaction = sigtimer;
    sigtimer_action.sa_flags = SA_SIGINFO;
    sigemptyset(&sigtimer_action.sa_mask);
    sigaction(SIGALRM, &sigtimer_action, NULL);

    struct timespec second = {1, 0};
    struct timespec zero = {0, 0};
    const struct itimerspec timer_value = {zero, second};

    timer_t timer;
    timer_create(CLOCK_REALTIME, NULL, &timer);
    timer_settime(timer, 0, &timer_value, NULL);

    kill(getpid(), SIGALRM);

    struct timespec nanosleep_value = {2, 0};
    while (nanosleep(&nanosleep_value, &nanosleep_value)) {
    }
}