#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: ./producer pipe file N\n");
        return 1;
    }

    FILE *pipe = fopen(argv[1], "w");
    FILE *file = fopen(argv[2], "r");
    const int N = atoi(argv[3]);

    char *buffer = calloc(N + 1, sizeof(char));
    while (fread(buffer, 1, N, file) > 0) {
        sleep(rand() % 2 + 1);
        fprintf(pipe, "#%d#%s\n", getpid(), buffer);
        fflush(pipe);
    }

    free(buffer);
    fclose(file);
    fclose(pipe);
}