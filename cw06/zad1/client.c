#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"

int other_queue = -1;

void get_replies(int client_queue) {
    message reply;
    while (msgrcv(client_queue, &reply, TEXT_LEN, 0, IPC_NOWAIT) != -1) {
        if (reply.type == CONNECT) {
            other_queue = atoi(reply.text);
        } else if (reply.type == SEND) {
            printf("MESSAGE: %s", reply.text);
        } else {
            puts(reply.text);
        }
    }
}

int starts_with(char *s, char *prefix) {
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

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

    char line[128];
    while (fgets(line, sizeof(line), stdin)) {
        message msg;
        int is_msg_to_client = 0;

        if (strcmp(line, "LIST")) {
            msg.type = LIST;
            sprintf(msg.text, "%d", own_id);
        }

        if (starts_with(line, "CONNECT")) {
            msg.type = CONNECT;

            (void)strtok(line, " ");
            int second_id = atoi(strtok(NULL, " "));
            sprintf(msg.text, "%d %d", own_id, second_id);
        }

        if (starts_with(line, "SEND") && other_queue != -1) {
            msg.type = SEND;

            sprintf(msg.text, "%s", strchr(line, ' ') + 1);
            is_msg_to_client = 1;
        }

        int destination = is_msg_to_client ? other_queue : server_queue;
        msgsnd(destination, &msg, TEXT_LEN, 0);

        sleep(1);

        get_replies(client_queue);
    }
}