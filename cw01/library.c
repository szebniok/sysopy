#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cIndex = 0;

const char* TMP_FILENAME = ".tmp";

EditingOps** create_container(int size) {
    EditingOps** retval = calloc(size, sizeof(EditingOps*));
    for (int i = 0; i < size; i++) {
        retval[i] = NULL;
    }
    return retval;
}

void compare_files(char** filenames, int size) {
    for (int i = 0; i < size; i += 2) {
        char* buffer = calloc(
            sizeof("diff  >>") + strlen(filenames[0]) + strlen(filenames[1]),
            sizeof(char));
        sprintf(buffer, "diff %s %s>>%s", filenames[0], filenames[1],
                TMP_FILENAME);
        system(buffer);
        sprintf(buffer, "printf \"n\\n\">>%s", TMP_FILENAME);
        system(buffer);
        free(buffer);
    }
}

int create_block(EditingOps** container) {
    FILE* tmp_file = fopen(TMP_FILENAME, "rw");
    char* line = NULL;
    size_t line_size = 0;
    char* ops = NULL;

    int block_count = 0;
    EditingOps* current_diff = calloc(0, sizeof(EditingOps));
    while (getline(&line, &line_size, tmp_file) != -1) {
        if (line[0] == 'n') {
            if (strlen(ops) > 0) {
                current_diff[block_count - 1].ops = ops;
            }
            container[cIndex++] = current_diff;
            current_diff = calloc(0, sizeof(EditingOps));
            block_count = 0;
        } else if ('0' <= line[0] && line[0] <= '9') {
            if (block_count > 0) {
                current_diff[block_count - 1].ops = ops;
            }
            block_count++;
            current_diff =
                realloc(current_diff, block_count * sizeof(EditingOps));
            ops = calloc(1000000, sizeof(char));
            strcpy(ops, line);
        } else {
            strcat(ops, line);
        }
    }

    free(line);
    fclose(tmp_file);

    return 0;
}