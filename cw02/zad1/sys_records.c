#include "sys_records.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define UNUSED(x) \
    if (x) {      \
    };

void sys_quicksort(int low, int high, int file, int record_size);

void sys_generate(char* filename, int records_count, int record_size) {
    char* buffer = calloc(strlen(filename) + 100, sizeof(char));
    sprintf(buffer, "head -c %d /dev/random > %s", records_count * record_size,
            filename);
    UNUSED(system(buffer));
    free(buffer);
}

void sys_sort(char* filename, int records_count, int record_size) {
    int file = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    sys_quicksort(0, records_count - 1, file, record_size);
    close(file);
}

void sys_copy(char* src_filename, char* dst_filename, int records_count,
              int buffer_size) {
    int src = open(src_filename, O_RDONLY);
    int dst = open(dst_filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    char* buffer = calloc(buffer_size, sizeof(char));

    int read_bytes_count = 0, read_records_count = 0;
    do {
        read_bytes_count = read(src, buffer, buffer_size);

        write(dst, buffer, buffer_size);
        read_records_count++;
    } while (read_bytes_count == buffer_size &&
             read_records_count < records_count);

    close(src);
    close(dst);

    free(buffer);
}

void sys_swap(int a, int b, int file, int record_size) {
    char* a_content = calloc(record_size, sizeof(char));
    char* b_content = calloc(record_size, sizeof(char));

    lseek(file, a * record_size, SEEK_SET);
    read(file, a_content, record_size);
    lseek(file, b * record_size, SEEK_SET);
    read(file, b_content, record_size);

    lseek(file, a * record_size, SEEK_SET);
    write(file, b_content, record_size);
    lseek(file, b * record_size, SEEK_SET);
    write(file, a_content, record_size);

    free(a_content);
    free(b_content);
}

int sys_partition(int low, int high, int file, int record_size) {
    int pivot = high;
    char* pivot_content = NULL;

    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (pivot_content == NULL) {
            pivot_content = calloc(record_size, sizeof(char));
            lseek(file, pivot * record_size, SEEK_SET);
            read(file, pivot_content, record_size);
        }

        char* j_content = calloc(record_size, sizeof(char));
        lseek(file, j * record_size, SEEK_SET);
        read(file, j_content, record_size);

        if (strcmp(j_content, pivot_content) < 0) {
            free(pivot_content);
            pivot_content = NULL;

            free(j_content);
            j_content = NULL;

            i++;
            sys_swap(i, j, file, record_size);
        }

        if (pivot_content != NULL) {
            free(pivot_content);
            pivot_content = NULL;
        }

        if (j_content != NULL) {
            free(j_content);
            j_content = NULL;
        }
    }
    sys_swap(i + 1, high, file, record_size);
    return i + 1;
}

void sys_quicksort(int low, int high, int file, int record_size) {
    if (low < high) {
        int pi = sys_partition(low, high, file, record_size);

        sys_quicksort(low, pi - 1, file, record_size);
        sys_quicksort(pi + 1, high, file, record_size);
    }
}