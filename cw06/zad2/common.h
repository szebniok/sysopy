#ifndef COMMON_H
#define COMMON_H

#include <mqueue.h>
#include <stdlib.h>

#define TEXT_LEN 8192
#define FILENAME_LEN 10
#define SERVER_KEY_ID 1

char* read_message(mqd_t src, char* type) {
    char from_queue[TEXT_LEN + 2] = {0};

    int success = mq_receive(src, from_queue, TEXT_LEN + 1, NULL);
    if (success == -1) return NULL;

    if (type) {
        *type = from_queue[0];
    }
    char* retval = calloc(TEXT_LEN + 1, sizeof(char));
    sprintf(retval, "%s", from_queue + 1);

    return retval;
}

void send_message(mqd_t dest, char type, char* message) {
    int length = strlen(message);
    char* buffer = calloc(2 + length, sizeof(char));
    buffer[0] = type;
    sprintf(buffer + 1, "%s", message);
    mq_send(dest, buffer, length + 1, 1);
}

typedef struct {
    long type;
    char text[TEXT_LEN];
} message;

typedef struct {
    int id;
    int queue_id;
    int connected_client_id;
} client;

#define INIT 1L
#define LIST 2L
#define CONNECT 3L
#define SEND 4L
#define DISCONNECT 5L
#define STOP 6L
#define STOP_SERVER 7L

#endif