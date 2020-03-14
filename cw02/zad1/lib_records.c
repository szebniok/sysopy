#include "lib_records.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNUSED(x) \
    if (x) {      \
    };

void lib_quicksort(int low, int high, FILE* file, int record_size);

void lib_sort(char* filename, int records_count, int record_size) {
    FILE* file = fopen(filename, "rwb+");
    lib_quicksort(0, records_count - 1, file, record_size);
    fclose(file);
}

void lib_copy(char* src_filename, char* dst_filename, int records_count,
              int buffer_size) {
    FILE* src = fopen(src_filename, "r");
    FILE* dst = fopen(dst_filename, "w");

    char* buffer = calloc(buffer_size, sizeof(char));

    int read_bytes_count = 0, read_records_count = 0;
    do {
        read_bytes_count = fread(buffer, 1, buffer_size, src);

        fwrite(buffer, 1, buffer_size, dst);
        read_records_count++;
    } while (read_bytes_count == buffer_size &&
             read_records_count < records_count);

    fclose(src);
    fclose(dst);

    free(buffer);
}

void lib_swap(int a, int b, FILE* file, int record_size) {
    char* a_content = calloc(record_size, sizeof(char));
    char* b_content = calloc(record_size, sizeof(char));

    fseek(file, a * record_size, 0);
    fread(a_content, 1, record_size, file);
    fseek(file, b * record_size, 0);
    fread(b_content, 1, record_size, file);

    fseek(file, a * record_size, 0);
    fwrite(b_content, 1, record_size, file);
    fseek(file, b * record_size, 0);
    fwrite(a_content, 1, record_size, file);

    free(a_content);
    free(b_content);
}

int lib_partition(int low, int high, FILE* file, int record_size) {
    int pivot = high;
    char* pivot_content = NULL;

    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (pivot_content == NULL) {
            pivot_content = calloc(record_size, sizeof(char));
            fseek(file, pivot * record_size, 0);
            fread(pivot_content, 1, record_size, file);
        }

        char* j_content = calloc(record_size, sizeof(char));
        fseek(file, j * record_size, 0);
        fread(j_content, 1, record_size, file);

        if (strcmp(j_content, pivot_content) < 0) {
            free(pivot_content);
            pivot_content = NULL;

            free(j_content);
            j_content = NULL;

            i++;
            lib_swap(i, j, file, record_size);
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
    lib_swap(i + 1, high, file, record_size);
    return i + 1;
}

void lib_quicksort(int low, int high, FILE* file, int record_size) {
    if (low < high) {
        int pi = lib_partition(low, high, file, record_size);

        lib_quicksort(low, pi - 1, file, record_size);
        lib_quicksort(pi + 1, high, file, record_size);
    }
}