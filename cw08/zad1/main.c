#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_LENGTH 70
#define WHITESPACE " \t\r\n"

int height;
int width;
unsigned char **image;
int histogram[256] = {0};

void read_uncommented_line(char *buffer, FILE *input) {
    do {
        fgets(buffer, MAX_LINE_LENGTH, input);
    } while (buffer[0] == '#' || buffer[0] == '\n');
}

void load_image(char *image_filename) {
    FILE *input = fopen(image_filename, "r");

    char buffer[MAX_LINE_LENGTH + 1] = {0};

    // ignore P2
    read_uncommented_line(buffer, input);

    // read dimensions
    read_uncommented_line(buffer, input);
    width = atoi(strtok(buffer, WHITESPACE));
    height = atoi(strtok(NULL, WHITESPACE));

    image = calloc(height, sizeof(char *));
    for (int i = 0; i < height; i++) {
        image[i] = calloc(width, sizeof(char));
    }

    // ignore maximum gray value
    read_uncommented_line(buffer, input);

    // read gray values
    read_uncommented_line(buffer, input);
    char *encoded_gray_value = strtok(buffer, WHITESPACE);
    for (int i = 0; i < width * height; i++) {
        if (encoded_gray_value == NULL) {
            read_uncommented_line(buffer, input);
            encoded_gray_value = strtok(buffer, WHITESPACE);
        }

        image[i / width][i % width] = atoi(encoded_gray_value);

        encoded_gray_value = strtok(NULL, WHITESPACE);
    }

    fclose(input);
}

void save_histogram(char *histogram_filename) {
    FILE *output = fopen(histogram_filename, "w+");

    float max_occurence = histogram[0];
    for (int i = 1; i < 256; i++) {
        if (max_occurence < histogram[i]) {
            max_occurence = histogram[i];
        }
    }

    fputs("P2\n", output);
    fputs("256 50\n", output);
    fputs("255\n", output);

    for (int y = 0; y < 50; y++) {
        for (int x = 0; x < 256; x++) {
            if (50 - y > 50 * (histogram[x] / max_occurence)) {
                fputs("0\n", output);
            } else {
                fprintf(output, "255\n");
            }
        }
    }

    fclose(output);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr,
                "Usage: ./main threads_count mode input_file output_file");
        return 1;
    }

    int threads_count = atoi(argv[1]);
    char *encoded_mode = argv[2];
    char *input_filename = argv[3];
    char *output_filename = argv[4];

    (void)threads_count;

    load_image(input_filename);

    if (strcmp(encoded_mode, "sign") == 0) {
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            histogram[image[y][x]]++;
        }
    }

    save_histogram(output_filename);

    for (int y = 0; y < height; y++) {
        free(image[y]);
    }
    free(image);
}