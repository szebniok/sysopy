#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "library.h"

int main() {
    char** filenames = calloc(4, sizeof(char*));
    filenames[0] = "a.txt";
    filenames[1] = "b.txt";
    filenames[2] = "a.txt";
    filenames[3] = "b.txt";
    compare_files(filenames, 4);

    EditingOps** container = create_container(2);
    create_block(container);

    system("rm .tmp");
    free(filenames);
    free(container);
}