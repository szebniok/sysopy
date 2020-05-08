#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define CLIENT_RETRY_TIME 5
#define CLIENT_CREATION_TIME 3
#define CUT_TIME 5

int K, N;

int barber_sleeping;
int* seats;
pthread_mutex_t seats_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t barber_ready = PTHREAD_COND_INITIALIZER;

int free_seats() {
    int retval = 0;

    for (int i = 0; i < K; i++) {
        if (seats[i] == 0) {
            retval++;
        }
    }

    return retval;
}

void barber() {
    int cut_clients = 0;

    while (cut_clients < N) {
        pthread_mutex_lock(&seats_mutex);

        while (free_seats() == K) {
            puts("Golibroda: ide spac");
            barber_sleeping = 1;
            pthread_cond_wait(&barber_ready, &seats_mutex);
        }
        barber_sleeping = 0;

        int client_index;
        for (int i = 0; i < K; i++) {
            if (seats[i] != 0) {
                client_index = i;
                break;
            }
        }

        printf("Golibroda: czeka %d klientow, gole klienta %d\n",
               K - free_seats() - 1, seats[client_index]);
        seats[client_index] = 0;
        cut_clients++;
        pthread_mutex_unlock(&seats_mutex);

        sleep(rand() % CUT_TIME + 1);
    }
    puts("Golibroda: koniec pracy");
}

void client(int* id) {
    pthread_mutex_lock(&seats_mutex);

    int free_seats_count = free_seats();
    if (free_seats_count == 0) {
        printf("Zajete; %d\n", *id);
        pthread_mutex_unlock(&seats_mutex);
        sleep(rand() % CLIENT_RETRY_TIME + 1);
        client(id);
        return;
    }

    for (int i = 0; i < K; i++) {
        if (seats[i] == 0) {
            seats[i] = *id;
            break;
        }
    }

    printf("Poczekalnia, wolne miejsca: %d; %d\n", free_seats_count - 1, *id);

    if (free_seats_count == K && barber_sleeping) {
        printf("Budze golibrode; %d\n", *id);
        pthread_cond_broadcast(&barber_ready);
    }
    pthread_mutex_unlock(&seats_mutex);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: ./main seats clients\n");
        return 1;
    }

    srand(time(NULL));

    K = atoi(argv[1]);
    N = atoi(argv[2]);
    seats = calloc(K, sizeof(int));

    pthread_t barber_thread;
    pthread_create(&barber_thread, NULL, (void* (*)(void*))barber, NULL);

    int* ids = calloc(N, sizeof(int));
    pthread_t* client_threads = calloc(N, sizeof(pthread_t));

    for (int i = 0; i < N; i++) {
        sleep(rand() % CLIENT_CREATION_TIME + 1);
        ids[i] = i + 1;
        pthread_create(&client_threads[i], NULL, (void* (*)(void*))client,
                       ids + i);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(client_threads[i], NULL);
    }
    pthread_join(barber_thread, NULL);

    free(ids);
    free(client_threads);
    free(seats);
}
