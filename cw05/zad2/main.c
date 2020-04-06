#define _POSIX_C_SOURCE 2
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./main file\n");
        return 1;
    }

    char* cmd = calloc(strlen(argv[1]) + 10, sizeof(char));
    sprintf(cmd, "sort %s", argv[1]);

    FILE* output = popen(cmd, "w");
    free(cmd);

    char buffer[257];
    while (fgets(buffer, sizeof(buffer) - 1, output) != NULL) {
        printf("%s", buffer);
    }

    fclose(output);
}