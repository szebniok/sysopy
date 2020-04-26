#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "common.h"

int main() {
    key_t key = ftok("main", 1);

    sem_t* created = sem_open("/created", O_RDWR, 0666);
    sem_t* packed = sem_open("/packed", O_RDWR, 0666);
    sem_t* can_modify = sem_open("/can_modify", O_RDWR, 0666);

    int memory = shmget(key, sizeof(memory_t), 0);

    while (1) {
        sem_wait(created);
        sem_wait(can_modify);

        memory_t* m = shmat(memory, NULL, 0);

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

        shmdt(m);

        sleep(1);
    }
}