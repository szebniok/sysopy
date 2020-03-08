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

    Container container = create_container(2);
    create_block(&container);

    delete_diff(&container, 1);
    delete_block(container.diffs[0], 1);

    system("rm .tmp");
    free(filenames);
}