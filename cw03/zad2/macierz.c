#define _XOPEN_SOURCE 500
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
    int** values;
    int rows;
    int cols;
} matrix;

int get_cols_number(char* row) {
    int cols = 0;
    char* tmp = row;
    while ((tmp = strchr(tmp, ' ')) != NULL) {
        cols++;
        tmp++;
    }
    return cols + 1;
}

void get_dimensions(FILE* matrix_file, int* rows, int* cols) {
    char line[256];
    *rows = 0;
    *cols = 0;

    while (fgets(line, 256, matrix_file) != NULL) {
        if (*cols == 0) {
            *cols = get_cols_number(line);
        }

        *rows = *rows + 1;
    }

    fseek(matrix_file, 0, SEEK_SET);
}

matrix load_matrix(char* filename) {
    FILE* matrix_file = fopen(filename, "r");

    int rows, cols;
    get_dimensions(matrix_file, &rows, &cols);

    int** values = calloc(rows, sizeof(int*));
    for (int y = 0; y < rows; y++) {
        values[y] = calloc(cols, sizeof(int));
    }

    int x_curr, y_curr = 0;
    char line[256];
    while (fgets(line, 256, matrix_file) != NULL) {
        x_curr = 0;
        char* encoded_number = strtok(line, " ");
        while (encoded_number != NULL) {
            values[y_curr][x_curr++] = atoi(encoded_number);
            encoded_number = strtok(NULL, " ");
        }

        y_curr++;
    }

    matrix retval;
    retval.values = values;
    retval.rows = rows;
    retval.cols = cols;

    return retval;
}

int get_task(FILE* tasks_file, int tasks_count) {
    int fd = fileno(tasks_file);
    flock(fd, LOCK_EX);
    char* tasks = calloc(tasks_count + 1, sizeof(char));
    fseek(tasks_file, 0, SEEK_SET);
    fread(tasks, 1, tasks_count, tasks_file);

    char* task_ptr_offset = strchr(tasks, '0');

    int task_index = task_ptr_offset != NULL ? task_ptr_offset - tasks : -1;

    if (task_index >= 0) {
        tasks[task_index] = '1';
        fseek(tasks_file, 0, SEEK_SET);
        fwrite(tasks, 1, tasks_count, tasks_file);
        fflush(tasks_file);
    }

    flock(fd, LOCK_UN);
    return task_index;
}

void multiply_column(matrix* a, matrix* b, int col_index) {
    char* filename = calloc(20, sizeof(char));
    sprintf(filename, ".tmp/part%d", col_index);
    FILE* part_file = fopen(filename, "w+");

    for (int y = 0; y < a->rows; y++) {
        int result = 0;

        for (int x = 0; x < a->cols; x++) {
            result += a->values[y][x] * b->values[x][col_index];
        }

        fprintf(part_file, "%d \n", result);
    }
    fclose(part_file);
}

void worker_callback(matrix* a, matrix* b, FILE* tasks_file) {
    while (1) {
        int col_index = get_task(tasks_file, b->cols);
        if (col_index == -1) {
            return;
        }

        multiply_column(a, b, col_index);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: ./macierz path workers_count timeout");
        return 1;
    }

    FILE* input_file = fopen(argv[1], "r");
    char input_line[PATH_MAX * 3 + 3];
    fgets(input_line, PATH_MAX * 3 + 3, input_file);
    fclose(input_file);

    char a_filename[PATH_MAX + 1];
    char b_filename[PATH_MAX + 1];
    char c_filename[PATH_MAX + 1];

    strcpy(a_filename, strtok(input_line, " "));
    strcpy(b_filename, strtok(NULL, " "));
    strcpy(c_filename, strtok(NULL, " "));

    int workers_count = atoi(argv[2]);
    // int timeout = atoi(argv[3]);

    matrix a = load_matrix(a_filename);
    matrix b = load_matrix(b_filename);

    system("rm -rf .tmp");
    system("mkdir -p .tmp");

    FILE* tasks_file = fopen(".tmp/tasks", "w+");
    char* encoded_tasks = calloc(b.cols + 1, sizeof(char));
    sprintf(encoded_tasks, "%0*d", b.cols, 0);
    fwrite(encoded_tasks, 1, b.cols, tasks_file);
    fflush(tasks_file);

    pid_t* workers = calloc(workers_count, sizeof(int));
    for (int i = 0; i < workers_count; i++) {
        pid_t spawned_worker = fork();
        if (spawned_worker == 0) {
            worker_callback(&a, &b, tasks_file);
            return 0;
        } else {
            workers[i] = spawned_worker;
        }
    }

    char buffer[256];
    sprintf(buffer, "paste .tmp/part* > %s", c_filename);
    system(buffer);

    return 0;
}