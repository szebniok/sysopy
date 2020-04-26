#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "common.h"

sem_t* space;
sem_t* created;
sem_t* packed;
sem_t* can_modify;

void sigterm_handler() {
    sem_close(space);
    sem_close(created);
    sem_close(packed);
    sem_close(can_modify);

    shm_unlink("/memory");

    exit(0);
}

int main() {
    signal(SIGTERM, sigterm_handler);

    space = sem_open("/space", O_RDWR, 0666);
    created = sem_open("/created", O_RDWR, 0666);
    packed = sem_open("/packed", O_RDWR, 0666);
    can_modify = sem_open("/can_modify", O_RDWR, 0666);

    int memory = shm_open("/memory", O_RDWR, 0666);

    while (1) {
        sem_wait(packed);
        sem_wait(can_modify);

        memory_t* m =
            mmap(NULL, sizeof(memory_t), PROT_WRITE, MAP_SHARED, memory, 0);

        m->packages[m->index].status = SENT;
        m->packages[m->index].value /= 2;
        m->packages[m->index].value *= 3;
        int n = m->packages[m->index].value;
        m->index = (m->index + 1) % PACKAGES_COUNT;
        m->size--;

        int created_count, packed_count;
        sem_getvalue(created, &created_count);
        sem_getvalue(packed, &packed_count);

        printf("(%d %lu) Wyslalem zamowienie o wielkosci %d. ", getpid(),
               time(NULL), n);
        printf("Liczba paczek do przygotowania: %d. ", created_count);
        printf("Liczba paczek do wyslania: %d\n", packed_count);

        sem_post(space);
        sem_post(can_modify);

        munmap(m, sizeof(memory_t));

        sleep(1);
    }
}