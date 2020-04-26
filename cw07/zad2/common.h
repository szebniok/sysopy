#ifndef COMMON_H
#define COMMON_H

#define PACKAGES_COUNT 10
#define MAX_CREATED_COUNT 10

#define CREATORS_COUNT 1
#define PACKERS_COUNT 5
#define SENDERS_COUNT 3

typedef enum { CREATED, PACKED, SENT } package_status;
typedef struct {
    package_status status;
    int value;
} package_t;

#define SPACE_INDEX 0
#define CREATED_INDEX 1
#define PACKED_INDEX 2
#define CAN_MODIFY_INDEX 3

typedef struct {
    int index;
    int size;
    package_t packages[PACKAGES_COUNT];
} memory_t;

#endif
