#ifndef COMMON_H
#define COMMON_H

#define TEXT_LEN 128
#define SERVER_KEY_ID 1

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