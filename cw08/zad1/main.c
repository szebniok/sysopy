#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_LENGTH 70
#define WHITESPACE " \t\r\n"

int height;
int width;
unsigned char **image;

int threads_count;
int **histogram_pieces;

int calculate_time(struct timespec *start) {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    int retval = (end.tv_sec - start->tv_sec) * 1000000;
    retval += (end.tv_nsec - start->tv_nsec) / 1000.0;
    return retval;
}

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

    int histogram[256] = {0};
    for (int i = 0; i < threads_count; i++) {
        for (int x = 0; x < 256; x++) {
            histogram[x] += histogram_pieces[i][x];
        }
    }

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

int sign_worker(int *thread_index) {
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int chunk_size = 256 / threads_count;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (image[y][x] / chunk_size == *thread_index) {
                histogram_pieces[0][image[y][x]]++;
            }
        }
    }
    return calculate_time(&start);
}

int block_worker(int *thread_index) {
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int k = *thread_index;
    int chunk_size = width / threads_count;
    for (int x = k * chunk_size; x < (k + 1) * chunk_size; x++) {
        for (int y = 0; y < height; y++) {
            histogram_pieces[k][image[y][x]]++;
        }
    }

    return calculate_time(&start);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr,
                "Usage: ./main threads_count mode input_file output_file");
        return 1;
    }

    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    threads_count = atoi(argv[1]);
    char *encoded_mode = argv[2];
    char *input_filename = argv[3];
    char *output_filename = argv[4];

    load_image(input_filename);

    histogram_pieces = calloc(threads_count, sizeof(int *));
    for (int i = 0; i < threads_count; i++) {
        histogram_pieces[i] = calloc(256, sizeof(int));
    }

    pthread_t *threads = calloc(threads_count, sizeof(pthread_t));
    int *args = calloc(threads_count, sizeof(int));

    for (int i = 0; i < threads_count; i++) {
        int (*start)(int *);
        if (strcmp(encoded_mode, "sign") == 0) {
            start = sign_worker;
        }
        if (strcmp(encoded_mode, "block") == 0) {
            start = block_worker;
        }

        args[i] = i;
        pthread_create(&threads[i], NULL, (void *(*)(void *))start, args + i);
    }

    for (int i = 0; i < threads_count; i++) {
        int time;
        pthread_join(threads[i], (void *)&time);
        printf("thread %d took %d microseconds\n", i, time);
    }

    save_histogram(output_filename);

    free(threads);
    free(args);

    for (int i = 0; i < threads_count; i++) {
        free(histogram_pieces[i]);
    }
    free(histogram_pieces);
    for (int y = 0; y < height; y++) {
        free(image[y]);
    }
    free(image);

    printf("total time: %d microseconds\n", calculate_time(&start));
}