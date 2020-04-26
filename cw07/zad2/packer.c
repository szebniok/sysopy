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

sem_t* created;
sem_t* packed;
sem_t* can_modify;

void sigterm_handler() {
    sem_close(created);
    sem_close(packed);
    sem_close(can_modify);

    shm_unlink("/memory");

    exit(0);
}

int main() {
    signal(SIGTERM, sigterm_handler);

    created = sem_open("/created", O_RDWR, 0666);
    packed = sem_open("/packed", O_RDWR, 0666);
    can_modify = sem_open("/can_modify", O_RDWR, 0666);

    int memory = shm_open("/memory", O_RDWR, 0666);

    while (1) {
        sem_wait(created);
        sem_wait(can_modify);

        memory_t* m =
            mmap(NULL, sizeof(memory_t), PROT_WRITE, MAP_SHARED, memory, 0);

        int index = m->index;
        while (m->packages[index].status != CREATED) {
            index = (index + 1) % PACKAGES_COUNT;
        }

        m->packages[index].status = PACKED;
        m->packages[index].value *= 2;
        int n = m->packages[index].value;

        int created_count, packed_count;
        sem_getvalue(created, &created_count);
        sem_getvalue(packed, &packed_count);

        printf("(%d %lu) Przygotowalem zamowienie o wielkosci %d. ", getpid(),
               time(NULL), n);
        printf("Liczba paczek do przygotowania: %d. ", created_count);
        printf("Liczba paczek do wyslania: %d\n", packed_count + 1);

        sem_post(packed);
        sem_post(can_modify);

        munmap(m, sizeof(memory_t));

        sleep(1);
    }
}