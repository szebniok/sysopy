#include <stdio.h>
#include "lib_records.h"

int main() {
    lib_generate("tmp", 10, 2);
    lib_copy("tmp", "tmp2", 10, 2);
    lib_sort("tmp", 10, 2);
}