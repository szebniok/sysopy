#ifndef COMMON_H
#define COMMON_H

#define PACKAGES_COUNT 10
#define MAX_CREATED_COUNT 10

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

void print_memory(memory_t* mem) {
    for (int i = 0; i < PACKAGES_COUNT; i++) {
        printf("|");
        if (mem->index == i) {
            printf("A");
        } else if ((mem->index + mem->size - 1) % PACKAGES_COUNT == i) {
            printf("B");
        } else {
            printf(" ");
        }

        printf("%d %d", mem->packages[i].status, mem->packages[i].value);
        printf("| ");
    }
    printf("\n");
}

#endif
