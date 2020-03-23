#define _XOPEN_SOURCE 500
#define MAX_COLS_NUMBER 1000
#define MAX_LINE_LENGTH (MAX_COLS_NUMBER * 5)

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    int** values;
    int rows;
    int cols;
} matrix;

void set_limits(int cpu_limit, int as_limit) {
    struct rlimit cpu = {cpu_limit, cpu_limit};
    struct rlimit as = {as_limit * 1000000, as_limit * 1000000};

    setrlimit(RLIMIT_CPU, &cpu);
    setrlimit(RLIMIT_AS, &as);
}

int get_elapsed_time(clock_t start_time) { return time(NULL) - start_time; }

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
    char line[MAX_LINE_LENGTH];
    *rows = 0;
    *cols = 0;

    while (fgets(line, MAX_LINE_LENGTH, matrix_file) != NULL) {
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
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, matrix_file) != NULL) {
        x_curr = 0;
        char* encoded_number = strtok(line, " ");
        while (encoded_number != NULL) {
            values[y_curr][x_curr++] = atoi(encoded_number);
            encoded_number = strtok(NULL, " ");
        }

        y_curr++;
    }

    fclose(matrix_file);

    matrix retval;
    retval.values = values;
    retval.rows = rows;
    retval.cols = cols;

    return retval;
}

void free_matrix(matrix* m) {
    for (int y = 0; y < m->rows; y++) {
        free(m->values[y]);
    }
    free(m->values);
}

int get_task(int pair_index, int tasks_count) {
    char buffer[PATH_MAX + 1];
    sprintf(buffer, ".tmp/tasks%03d", pair_index);
    FILE* tasks_file = fopen(buffer, "r+");
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

    free(tasks);

    flock(fd, LOCK_UN);
    fclose(tasks_file);

    return task_index;
}

void multiply_column(matrix* a, matrix* b, int current_pair_index,
                     int col_index) {
    char filename[PATH_MAX + 1];
    sprintf(filename, ".tmp/part_%03d_%04d", current_pair_index, col_index);
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

int worker_callback(matrix* a, matrix* b, int pairs_number, clock_t start_time,
                    int timeout, int cpu_limit, int as_limit) {
    set_limits(cpu_limit, as_limit);
    int multiplies_count = 0;
    int current_pair_index = 0;
    while (1) {
        if (get_elapsed_time(start_time) >= timeout) {
            break;
        }

        if (current_pair_index == pairs_number) {
            break;
        }

        int col_index =
            get_task(current_pair_index, b[current_pair_index].cols);
        if (col_index == -1) {
            current_pair_index++;
            continue;
        }

        multiply_column(&a[current_pair_index], &b[current_pair_index],
                        current_pair_index, col_index);
        multiplies_count++;
    }

    return multiplies_count;
}

int number_of_lines(FILE* f) {
    fseek(f, 0, SEEK_SET);

    int retval = 0;
    char buffer[MAX_LINE_LENGTH];
    while (fgets(buffer, MAX_LINE_LENGTH, f) != NULL) {
        retval++;
    }

    fseek(f, 0, SEEK_SET);
    return retval;
}

void print_usage(struct rusage* before, struct rusage* after) {
    long user_sec = abs(after->ru_utime.tv_sec - before->ru_utime.tv_sec);
    long user_micro = abs(after->ru_utime.tv_usec - before->ru_utime.tv_usec);

    long system_sec = abs(after->ru_stime.tv_sec - before->ru_stime.tv_sec);
    long system_micro = abs(after->ru_stime.tv_usec - before->ru_stime.tv_usec);

    printf("user time: %ld.%06ld\n", user_sec, user_micro);
    printf("system time: %ld.%06ld\n\n", system_sec, system_micro);
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: ./macierz path workers_count timeout");
        return 1;
    }

    FILE* input_file = fopen(argv[1], "r");
    int workers_count = atoi(argv[2]);
    int timeout = atoi(argv[3]);
    int cpu_limit = atoi(argv[4]);
    int as_limit = atoi(argv[5]);

    int pairs_number = number_of_lines(input_file);

    matrix* a_matrices = calloc(pairs_number, sizeof(matrix));
    matrix* b_matrices = calloc(pairs_number, sizeof(matrix));
    char** c_filenames = calloc(pairs_number, sizeof(char*));
    for (int i = 0; i < pairs_number; i++) {
        c_filenames[i] = calloc(PATH_MAX + 1, sizeof(char*));
    }

    char input_line[PATH_MAX * 3 + 3];
    int i = 0;
    while (fgets(input_line, PATH_MAX * 3 + 3, input_file) != NULL) {
        char a_filename[PATH_MAX + 1];
        char b_filename[PATH_MAX + 1];

        strcpy(a_filename, strtok(input_line, " "));
        strcpy(b_filename, strtok(NULL, " "));
        strcpy(c_filenames[i], strtok(NULL, " "));

        a_matrices[i] = load_matrix(a_filename);
        b_matrices[i] = load_matrix(b_filename);

        system("mkdir -p .tmp");

        char tasks_filename_buffer[PATH_MAX + 1];
        sprintf(tasks_filename_buffer, ".tmp/tasks%03d", i);
        FILE* tasks_file = fopen(tasks_filename_buffer, "w+");
        char* encoded_tasks = calloc(b_matrices[i].cols + 1, sizeof(char));
        sprintf(encoded_tasks, "%0*d", b_matrices[i].cols, 0);
        fwrite(encoded_tasks, 1, b_matrices[i].cols, tasks_file);
        fflush(tasks_file);
        free(encoded_tasks);
        fclose(tasks_file);
        i++;
    }

    fclose(input_file);
    time_t start_time = time(NULL);

    pid_t* workers = calloc(workers_count, sizeof(pid_t));
    for (int i = 0; i < workers_count; i++) {
        pid_t spawned_worker = fork();
        if (spawned_worker == 0) {
            return worker_callback(a_matrices, b_matrices, pairs_number,
                                   start_time, timeout, cpu_limit, as_limit);
        } else {
            workers[i] = spawned_worker;
        }
    }

    struct rusage before;
    struct rusage after;
    for (int i = 0; i < workers_count; i++) {
        int status;
        getrusage(RUSAGE_CHILDREN, &before);
        waitpid(workers[i], &status, 0);
        getrusage(RUSAGE_CHILDREN, &after);
        printf("Proces %d wykonal %d mnozen macierzy\n", workers[i],
               WEXITSTATUS(status));
        print_usage(&before, &after);
    }
    free(workers);

    for (int i = 0; i < pairs_number; i++) {
        char buffer[256];
        sprintf(buffer, "paste .tmp/part_%03d_* > %s", i, c_filenames[i]);
        system(buffer);

        free_matrix(&a_matrices[i]);
        free_matrix(&b_matrices[i]);
        free(c_filenames[i]);
    }
    free(a_matrices);
    free(b_matrices);
    free(c_filenames);

    system("rm -rf .tmp");

    return 0;
}