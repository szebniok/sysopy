#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

#include "common.h"

int main() {
    srand(getpid());
    key_t key = ftok("main", 1);

    int semaphores = semget(key, 4, 0);
    int memory = shmget(key, sizeof(memory_t), 0);

    struct sembuf lock_memory = {CAN_MODIFY_INDEX, -1, 0};
    struct sembuf decrement_space = {SPACE_INDEX, -1, 0};

    struct sembuf unlock_memory = {CAN_MODIFY_INDEX, 1, 0};
    struct sembuf increment_created = {CREATED_INDEX, 1, 0};

    while (1) {
        struct sembuf ops_start[2] = {lock_memory, decrement_space};
        semop(semaphores, ops_start, 2);

        int n = rand() % MAX_CREATED_COUNT + 1;

        memory_t *m = shmat(memory, NULL, 0);

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

        int created_count = semctl(semaphores, CREATED_INDEX, GETVAL);
        int packed_count = semctl(semaphores, PACKED_INDEX, GETVAL);

        printf("(%d %lu) Dostalem liczbe %d. ", getpid(), time(NULL), n);
        printf("Liczba paczek do przygotowania: %d. ", created_count + 1);
        printf("Liczba paczek do wyslania: %d\n", packed_count);

        struct sembuf ops_end[2] = {unlock_memory, increment_created};
        semop(semaphores, ops_end, 2);
        shmdt(m);

        sleep(1);
    }
}