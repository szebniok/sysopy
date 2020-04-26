#ifndef COMMON_H
#define COMMON_H

#define PACKAGES_COUNT 10
#define MAX_CREATED_COUNT 10

#define CREATORS_COUNT 7
#define PACKERS_COUNT 5
#define SENDERS_COUNT 3

typedef enum { CREATED, PACKED, SENT } package_status;
typedef struct {
    package_status status;
    int value;
} package_t;

typedef struct {
    int index;
    int size;
    package_t packages[PACKAGES_COUNT];
} memory_t;

#endif
