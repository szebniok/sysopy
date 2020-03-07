#ifndef LIBRARY_H
#define LIBRARY_H

typedef struct {
    char* ops;
} EditingOps;

EditingOps** create_container(int size);

void compare_files(char** filenames, int size);

int create_block();

#endif