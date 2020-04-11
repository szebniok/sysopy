#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"

int main() {
    key_t server_queue_key = ftok("server.c", 1);
    int server_queue = msgget(server_queue_key, 0666);

    key_t client_queue_key = ftok("client.c", getpid());
    int client_queue = msgget(client_queue_key, IPC_CREAT | 0666);

    int own_id;

    message init;
    init.type = INIT;
    sprintf(init.text, "%d", client_queue);
    msgsnd(server_queue, &init, TEXT_LEN, 0);

    message init_ack;
    msgrcv(client_queue, &init_ack, TEXT_LEN, INIT_ACK, 0);
    own_id = atoi(init_ack.text);

    printf("own_id: %d\n", own_id);
    (void)getchar();

    message list;
    list.type = LIST;
    sprintf(list.text, "%d", own_id);
    msgsnd(server_queue, &list, TEXT_LEN, 0);

    message list_reply;
    msgrcv(client_queue, &list_reply, TEXT_LEN, LIST, 0);
    puts(list_reply.text);
}