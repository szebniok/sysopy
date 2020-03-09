#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include "library.h"

#ifdef DYNAMIC
#include <dlfcn.h>
#endif

double time_in_seconds(clock_t start, clock_t end) {
    return (double)(end - start) / sysconf(_SC_CLK_TCK);
}

int main(int argc, char* argv[]) {
    struct tms start_tms;
    struct tms end_tms;

    clock_t start_time = times(&start_tms);

    if (argc < 2) return 1;

#ifdef DYNAMIC
    void* lib_handle = dlopen("./liblibrary.so", RTLD_LAZY);
    Container (*create_container)(int) = dlsym(lib_handle, "create_container");
    void (*compare_files)(char**, int) = dlsym(lib_handle, "compare_files");
    int (*create_block)(Container*) = dlsym(lib_handle, "create_block");
    void (*delete_diff)(Container*, int) = dlsym(lib_handle, "delete_diff");
    void (*delete_block)(FileDiff*, int) = dlsym(lib_handle, "delete_block");
#endif

    int main_table_count = atoi(argv[1]);

    Container container = create_container(main_table_count);

    int i = 2;
    while (i < argc) {
        char* instruction = argv[i++];
        if (strcmp(instruction, "create_table") == 0) {
            int size = atoi(argv[i++]);
            container = create_container(size);
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

                char* filenames[] = {filenameA, filenameB};
                compare_files(filenames, 2);
            } while (i < argc && strchr(argv[i], ':') != NULL);
        } else if (strcmp(instruction, "create_block") == 0) {
            create_block(&container);
        } else if (strcmp(instruction, "remove_block") == 0) {
            int block_index = atoi(argv[i++]);
            delete_diff(&container, block_index);
        } else if (strcmp(instruction, "remove_operation") == 0) {
            int block_index = atoi(argv[i++]);
            int operation_index = atoi(argv[i++]);
            delete_block(container.diffs[block_index], operation_index);
        } else {
            printf("invalid operation: %s\n", instruction);
            return 1;
        }
    }

    clock_t end_time = times(&end_tms);

    printf("real: %f\n", time_in_seconds(start_time, end_time));
    printf("user: %f\n",
           time_in_seconds(start_tms.tms_utime, end_tms.tms_utime));
    printf("sys:  %f\n",
           time_in_seconds(start_tms.tms_stime, end_tms.tms_stime));

#ifdef DYNAMIC
    dlclose(lib_handle);
#endif

    return 0;
}