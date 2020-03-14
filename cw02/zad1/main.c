#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib_records.h"
#include "sys_records.h"

#define UNUSED(x) \
    if (x) {      \
    };

void generate(char* filename, int records_count, int record_size) {
    char* buffer = calloc(strlen(filename) + 100, sizeof(char));
    sprintf(buffer, "head -c %d /dev/random > %s", records_count * record_size,
            filename);
    UNUSED(system(buffer));
    free(buffer);
}

int main(int argc, char* args[]) {
    generate("tmp", 10, 2);
    sys_copy("tmp", "tmp2", 10, 2);
    sys_sort("tmp", 10, 2);
}