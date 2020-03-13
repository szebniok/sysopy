#include <stdio.h>
#include "lib_records.h"
#include "sys_records.h"

int main() {
    sys_generate("tmp", 10, 2);
    sys_copy("tmp", "tmp2", 10, 2);
    sys_sort("tmp", 10, 2);
}