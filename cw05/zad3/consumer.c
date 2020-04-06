#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: ./consumer pipe file N\n");
        return 1;
    }

    FILE *pipe = fopen(argv[1], "r");
    FILE *file = fopen(argv[2], "w+");
    const int N = atoi(argv[3]);

    char *buffer = calloc(N + 1, sizeof(char));
    while (fread(buffer, 1, N, pipe) > 0) {
        fwrite(buffer, 1, N, file);
    }

    free(buffer);
    fclose(file);
    fclose(pipe);
}