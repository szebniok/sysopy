#ifndef LIBRARY_H
#define LIBRARY_H

typedef struct {
    char* ops;
} EditingOps;

typedef struct {
    EditingOps* blocks;
    int size;
} FileDiff;

typedef struct {
    FileDiff** diffs;
    int size;
} Container;

Container create_container(int size);

void compare_files(char** filenames, int size);

int create_block();

int block_count(FileDiff* file_diff);

void delete_block(FileDiff* file_diff, int index);

void delete_diff(Container* container, int index);

#endif