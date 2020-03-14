#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
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

void print_time(struct tms* start) {
    struct tms end;
    times(&end);
    printf("user: %f, sys: %f\n\n",
           (double)(end.tms_utime - start->tms_utime) / sysconf(_SC_CLK_TCK),
           (double)(end.tms_stime - start->tms_stime) / sysconf(_SC_CLK_TCK));
}

int main(int argc, char* args[]) {
    struct tms cmd_start;

    for (int i = 1; i < argc; i++) {
        times(&cmd_start);

        char* cmd = args[i];

        if (strcmp(cmd, "generate") == 0) {
            char* filename = args[i + 1];
            int records_count = atoi(args[i + 2]);
            int record_size = atoi(args[i + 3]);
            i += 3;

            generate(filename, records_count, record_size);
        } else if (strcmp(cmd, "sort") == 0) {
            char* filename = args[i + 1];
            int records_count = atoi(args[i + 2]);
            int record_size = atoi(args[i + 3]);
            char* type = args[i + 4];
            i += 4;

            if (strcmp(type, "sys") == 0) {
                sys_sort(filename, records_count, record_size);
            } else {
                lib_sort(filename, records_count, record_size);
            }

            printf("sort %s %d %d %s:\n", filename, records_count, record_size,
                   type);
            print_time(&cmd_start);
        } else if (strcmp(cmd, "copy") == 0) {
            char* src_filename = args[i + 1];
            char* dst_filename = args[i + 2];
            int records_count = atoi(args[i + 3]);
            int record_size = atoi(args[i + 4]);
            char* type = args[i + 5];
            i += 5;

            if (strcmp(type, "sys") == 0) {
                sys_copy(src_filename, dst_filename, records_count,
                         record_size);
            } else {
                lib_copy(src_filename, dst_filename, records_count,
                         record_size);
            }

            printf("copy %s %s %d %d %s:\n", src_filename, dst_filename,
                   records_count, record_size, type);
            print_time(&cmd_start);
        } else {
            fprintf(stderr, "Invalid command: %s", cmd);
            return 1;
        }
    }

    return 0;
}