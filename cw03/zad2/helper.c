#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int random_inclusive(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void generate_matrix(int rows, int cols, char* filename) {
    FILE* file = fopen(filename, "w+");

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (x > 0) {
                fprintf(file, " ");
            }
            fprintf(file, "%d", random_inclusive(0, 10));
        }
        fprintf(file, "\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: ./helper min max");
        return 1;
    }

    srand(time(NULL));

    int min = atoi(argv[1]);
    int max = atoi(argv[2]);

    int a_rows = random_inclusive(min, max);
    int a_cols = random_inclusive(min, max);
    int b_cols = random_inclusive(min, max);

    generate_matrix(a_rows, a_cols, "a.txt");
    generate_matrix(a_cols, b_cols, "b.txt");

    system("echo \"a.txt b.txt c.txt\" > lista");

    return 0;
}