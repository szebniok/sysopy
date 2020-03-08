#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "library.h"

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;

    int main_table_count = atoi(argv[1]);
    Container container = create_container(main_table_count);

    int i = 2;
    while (i < argc) {
        char* instruction = argv[i++];
        if (strcmp(instruction, "create_table") == 0) {
            int size = atoi(argv[i++]);
            printf("create_table: %d\n", size);
        } else if (strcmp(instruction, "compare_pairs") == 0) {
            do {
                char* filenames_pair = argv[i++];
                char* filenameA = calloc(strlen(filenames_pair), sizeof(char));
                char* filenameB = calloc(strlen(filenames_pair), sizeof(char));
                int colonPos = strchr(filenames_pair, ':') - filenames_pair;
                for (int i = 0; i < colonPos; i++) {
                    filenameA[i] = filenames_pair[i];
                }
                for (int i = 0; i + colonPos + 1 < strlen(filenames_pair);
                     i++) {
                    filenameB[i] = filenames_pair[i + colonPos + 1];
                }

                printf("compare_pairs: %s, %s\n", filenameA, filenameB);
            } while (i < argc && strchr(argv[i], ':') != NULL);
        } else if (strcmp(instruction, "remove_block") == 0) {
            int block_index = atoi(argv[i++]);
            printf("remove_block: %d\n", block_index);
        } else if (strcmp(instruction, "remove_operation") == 0) {
            int block_index = atoi(argv[i++]);
            int operation_index = atoi(argv[i++]);
            printf("remove_operation: %d, %d\n", block_index, operation_index);
        } else {
            printf("invalid operation: %s\n", instruction);
            return 1;
        }
    }

    char** filenames = calloc(4, sizeof(char*));
    filenames[0] = "a.txt";
    filenames[1] = "b.txt";
    filenames[2] = "a.txt";
    filenames[3] = "b.txt";
    compare_files(filenames, 4);

    create_block(&container);

    delete_diff(&container, 1);
    delete_block(container.diffs[0], 1);

    system("rm .tmp");
    free(filenames);

    return 0;
}