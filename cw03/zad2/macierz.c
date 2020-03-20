#include <linux/limits.h>
#include <stdio.h>
#include <string.h>

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

    int first_space_index = -1;
    for (int i = 0; input_line[i] != 0; i++) {
        if (input_line[i] == ' ') {
            if (first_space_index == -1) {
                first_space_index = i;
                strncpy(a_filename, input_line, first_space_index);
            } else {
                strncpy(b_filename, input_line + first_space_index + 1,
                        i - first_space_index - 1);
                strcpy(c_filename, input_line + i + 1);
                break;
            }
        }
    }

    puts(a_filename);
    puts(b_filename);
    puts(c_filename);
}