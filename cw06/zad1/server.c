#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>

#include "common.h"
#define MAX_CLIENTS 10

int first_available_id = 0;
int clients_count = 0;
client* clients[MAX_CLIENTS] = {NULL};

int get_client_queue_id(int client_id) {
    for (int i = 0; i < clients_count; i++) {
        if (clients[i]->id == client_id) return clients[i]->queue_id;
    }

    return -1;
}

void init_handler(message* msg) {
    int queue_id = atoi(msg->text);

    client* new_client = calloc(1, sizeof(client));
    new_client->id = first_available_id++;
    new_client->queue_id = queue_id;
    new_client->connected_client_id = -1;

    clients[clients_count++] = new_client;

    message reply;
    reply.type = INIT_ACK;
    sprintf(reply.text, "%d", new_client->id);
    msgsnd(queue_id, &reply, TEXT_LEN, 0);
}

void list_handler(message* msg) {
    puts("list_handler");
    int client_id = atoi(msg->text);
    printf("client_id: %d\n", client_id);

    int queue_id = get_client_queue_id(client_id);

    puts("sending reply...");

    message reply;
    reply.type = LIST;
    for (int i = 0; i < clients_count; i++) {
        sprintf(reply.text + strlen(reply.text), "%d: %d\n", clients[i]->id,
                clients[i]->connected_client_id == -1);
    }
    msgsnd(queue_id, &reply, TEXT_LEN, 0);
    puts(reply.text);
}

int main() {
    key_t server_queue_key = ftok("server.c", 1);
    int server_queue = msgget(server_queue_key, IPC_CREAT | 0666);

    while (1) {
        message msg;
        msgrcv(server_queue, &msg, TEXT_LEN, 0, 0);
        printf("%ld: %s\n", msg.type, msg.text);

        switch (msg.type) {
            case INIT:
                init_handler(&msg);
                break;
            case LIST:
                list_handler(&msg);
                break;
        }
    }

    msgctl(server_queue, IPC_RMID, NULL);
}