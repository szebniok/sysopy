#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"

pid_t childs[CREATORS_COUNT + PACKERS_COUNT + SENDERS_COUNT];

void sigint_handler() {
    for (int i = 0; i < CREATORS_COUNT + PACKERS_COUNT + SENDERS_COUNT; i++) {
        kill(childs[i], SIGTERM);
    }
}

int main() {
    signal(SIGINT, sigint_handler);

    sem_t *space = sem_open("/space", O_CREAT | O_RDWR, 0666, PACKAGES_COUNT);
    sem_t *created = sem_open("/created", O_CREAT | O_RDWR, 0666, 0);
    sem_t *packed = sem_open("/packed", O_CREAT | O_RDWR, 0666, 0);
    sem_t *can_modify = sem_open("/can_modify", O_CREAT | O_RDWR, 0666, 1);

    int memory = shm_open("/memory", O_CREAT | O_RDWR, 0666);
    ftruncate(memory, sizeof(memory_t));

    memory_t *m =
        mmap(NULL, sizeof(memory_t), PROT_WRITE, MAP_SHARED, memory, 0);
    m->index = -1;
    m->size = 0;
    for (int i = 0; i < PACKAGES_COUNT; i++) {
        m->packages[i].status = SENT;
        m->packages[i].value = 0;
    }

    munmap(m, sizeof(memory_t));

    int j = 0;
    for (int i = 0; i < CREATORS_COUNT; i++) {
        childs[j] = fork();
        if (childs[j] == 0) {
            execlp("./creator", "./creator", NULL);
            return 1;
        }
        j++;
    }

    for (int i = 0; i < PACKERS_COUNT; i++) {
        childs[j] = fork();
        if (childs[j] == 0) {
            execlp("./packer", "./packer", NULL);
            perror("test");
            return 1;
        }
        j++;
    }

    for (int i = 0; i < SENDERS_COUNT; i++) {
        childs[j] = fork();
        if (childs[j] == 0) {
            execlp("./sender", "./sender", NULL);
            return 1;
        }
        j++;
    }

    for (int i = 0; i < CREATORS_COUNT + PACKERS_COUNT + SENDERS_COUNT; i++) {
        wait(0);
    }

    sem_close(space);
    sem_close(created);
    sem_close(packed);
    sem_close(can_modify);

    sem_unlink("/space");
    sem_unlink("/created");
    sem_unlink("/packed");
    sem_unlink("/can_modify");

    shm_unlink("/memory");

    return 0;
}