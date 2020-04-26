#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

#include "common.h"

int main() {
    key_t key = ftok("main", 1);

    int semaphores = semget(key, 4, 0);
    int memory = shmget(key, sizeof(memory_t), 0);

    struct sembuf lock_memory = {CAN_MODIFY_INDEX, -1, 0};
    struct sembuf decrement_packed = {PACKED_INDEX, -1, 0};

    struct sembuf unlock_memory = {CAN_MODIFY_INDEX, 1, 0};
    struct sembuf increment_space = {SPACE_INDEX, 1, 0};

    while (1) {
        struct sembuf ops_start[2] = {lock_memory, decrement_packed};
        semop(semaphores, ops_start, 2);

        memory_t *m = shmat(memory, NULL, 0);

        m->packages[m->index].status = SENT;
        m->packages[m->index].value /= 2;
        m->packages[m->index].value *= 3;
        int n = m->packages[m->index].value;
        m->index = (m->index + 1) % PACKAGES_COUNT;
        m->size--;

        int created_count = semctl(semaphores, CREATED_INDEX, GETVAL);
        int packed_count = semctl(semaphores, PACKED_INDEX, GETVAL);

        printf("(%d %lu) Wyslalem zamowienie o wielkosci %d. ", getpid(),
               time(NULL), n);
        printf("Liczba paczek do przygotowania: %d. ", created_count);
        printf("Liczba paczek do wyslania: %d\n", packed_count);

        struct sembuf ops_end[2] = {unlock_memory, increment_space};
        semop(semaphores, ops_end, 2);

        shmdt(m);

        sleep(1);
    }
}