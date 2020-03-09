#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cIndex = 0;

const char* TMP_FILENAME = ".tmp";

Container create_container(int size) {
    FileDiff** diffs = calloc(size, sizeof(FileDiff*));
    for (int i = 0; i < size; i++) {
        diffs[i] = NULL;
    }

    Container retval;
    retval.diffs = diffs;
    retval.size = 0;
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

int create_block(Container* container) {
    int first_created_diff_index = container->size;

    FILE* tmp_file = fopen(TMP_FILENAME, "rw");
    char* line = NULL;
    size_t line_size = 0;
    char* ops = NULL;

    int block_count = 0;
    EditingOps* current_file_blocks = calloc(0, sizeof(EditingOps));

    while (getline(&line, &line_size, tmp_file) != -1) {
        if (line[0] == 'n') {
            if (strlen(ops) > 0) {
                current_file_blocks[block_count - 1].ops = ops;
            }
            FileDiff* file_diff = calloc(1, sizeof(FileDiff));
            file_diff->blocks = current_file_blocks;
            file_diff->size = block_count;
            container->diffs[container->size++] = file_diff;

            current_file_blocks = calloc(0, sizeof(EditingOps));
            block_count = 0;
        } else if ('0' <= line[0] && line[0] <= '9') {
            if (block_count > 0) {
                current_file_blocks[block_count - 1].ops = ops;
            }

            block_count++;
            current_file_blocks =
                realloc(current_file_blocks, block_count * sizeof(EditingOps));
            ops = calloc(1000000, sizeof(char));
            strcpy(ops, line);
        } else {
            strcat(ops, line);
        }
    }

    free(line);

    fclose(tmp_file);
    char* buffer = calloc(100, sizeof(char));
    sprintf(buffer, "rm %s", TMP_FILENAME);
    system(buffer);
    free(buffer);

    return first_created_diff_index;
}

int block_count(FileDiff* file_diff) { return file_diff->size; }

void delete_block(FileDiff* file_diff, int index) {
    if (index < 0 || index > file_diff->size - 1) return;

    EditingOps block_to_be_deleted = file_diff->blocks[index];

    for (int i = index + 1; i < file_diff->size; i++) {
        file_diff->blocks[i - 1] = file_diff->blocks[i];
    }

    free(block_to_be_deleted.ops);

    file_diff->size--;
    file_diff->blocks =
        realloc(file_diff->blocks, file_diff->size * sizeof(EditingOps));
}

void delete_diff(Container* container, int index) {
    if (index < 0 || index > container->size - 1) return;

    FileDiff* diff_to_be_deleted = container->diffs[index];

    for (int i = index + 1; i < container->size; i++) {
        container->diffs[i - 1] = container->diffs[i];
    }

    container->diffs[container->size - 1] = NULL;

    while (diff_to_be_deleted->size > 0) {
        delete_block(diff_to_be_deleted, 0);
    }
    free(diff_to_be_deleted);

    container->size--;
}