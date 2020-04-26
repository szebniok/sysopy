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
    srand(getpid());
    key_t key = ftok("main", 1);

    sem_t* space = sem_open("/space", O_RDWR, 0666);
    sem_t* created = sem_open("/created", O_RDWR, 0666);
    sem_t* packed = sem_open("/packed", O_RDWR, 0666);
    sem_t* can_modify = sem_open("/can_modify", O_RDWR, 0666);

    int memory = shmget(key, sizeof(memory_t), 0);

    while (1) {
        sem_wait(space);
        sem_wait(can_modify);

        int n = rand() % MAX_CREATED_COUNT + 1;

        memory_t* m = shmat(memory, NULL, 0);

        int index;
        if (m->index == -1) {
            m->index = 0;
            index = 0;
        } else {
            index = (m->index + m->size) % PACKAGES_COUNT;
        }

        m->packages[index].status = CREATED;
        m->packages[index].value = n;
        m->size++;

        int created_count, packed_count;
        sem_getvalue(created, &created_count);
        sem_getvalue(packed, &packed_count);

        printf("(%d %lu) Dostalem liczbe %d. ", getpid(), time(NULL), n);
        printf("Liczba paczek do przygotowania: %d. ", created_count + 1);
        printf("Liczba paczek do wyslania: %d\n", packed_count);

        sem_post(created);
        sem_post(can_modify);

        shmdt(m);

        sleep(1);
    }
}