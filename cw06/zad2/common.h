#ifndef COMMON_H
#define COMMON_H

#include <mqueue.h>
#include <stdlib.h>

#define TEXT_LEN 8192
#define FILENAME_LEN 16

#define STOP_SERVER 1L
#define STOP 2L
#define DISCONNECT 3L
#define LIST 4L
#define INIT 5L
#define CONNECT 6L
#define SEND 7L
#define TYPES_COUNT 7L

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
    mq_send(dest, buffer, length + 1, TYPES_COUNT - type + 1);
}

typedef struct {
    long type;
    char text[TEXT_LEN];
} message;

typedef struct {
    int id;
    int queue_id;
    char* queue_filename;
    int connected_client_id;
} client;

#endif