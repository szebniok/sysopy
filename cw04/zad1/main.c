#define _POSIX_C_SOURCE 1
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int paused = 0;

void sigtstp_handler(int signum) {
    // ignore warning about unused parameter
    (void)signum;
    if (!paused) {
        puts(
            "Oczekuje na CTRL+Z - kontynuacja albo CTR+C - zakonczenie "
            "programu");
    }

    paused = !paused;
}

void sigint_handler(int signum) {
    // ignore warning about unused parameter
    (void)signum;
    puts("Odebrano sygnał SIGINT");
    exit(0);
}

int main() {
    struct sigaction sigtstp_action;
    sigtstp_action.sa_handler = sigtstp_handler;
    sigtstp_action.sa_flags = 0;
    sigemptyset(&sigtstp_action.sa_mask);

    sigaction(SIGTSTP, &sigtstp_action, NULL);
    signal(SIGINT, sigint_handler);
    while (1) {
        if (!paused) {
            system("ls -l");
        }
        sleep(1);
    }
}