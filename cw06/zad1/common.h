#ifndef COMMON_H
#define COMMON_H

#define TEXT_LEN 128

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
#define INIT_ACK 2L
#define LIST 3L
#define CONNECT 4L
#define SEND 5L
#define DISCONNECT 6L
#define STOP 7L
#define STOP_SERVER 8L

#endif