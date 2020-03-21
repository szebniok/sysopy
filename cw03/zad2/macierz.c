#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int** values;
    int rows;
    int cols;
} matrix;

int get_cols_number(char* row) {
    int cols = 0;
    char* tmp = row;
    while ((tmp = strstr(tmp, " ")) != NULL) {
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

int main(int argc, char* argv[]) {
    if (argc == 1) {
        fprintf(stderr, "You have to run this program with a path");
        return 1;
    }

    FILE* input_file = fopen(argv[1], "r");
    char input_line[PATH_MAX * 3 + 3];
    fgets(input_line, PATH_MAX * 3 + 3, input_file);

    char a_filename[PATH_MAX + 1];
    char b_filename[PATH_MAX + 1];
    char c_filename[PATH_MAX + 1];

    strcpy(a_filename, strtok(input_line, " "));
    strcpy(b_filename, strtok(NULL, " "));
    strcpy(c_filename, strtok(NULL, " "));

    matrix a = load_matrix(a_filename);
    printf("%dx%d\n", a.rows, a.cols);
    for (int y = 0; y < a.rows; y++) {
        for (int x = 0; x < a.cols; x++) {
            printf("%d ", a.values[y][x]);
        }
        puts("");
    }

    puts("\n");
}