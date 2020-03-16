#define _GNU_SOURCE
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    int value;
    char op;
} arg_number;

int depth = -1;
arg_number* mtime = NULL;
arg_number* atime = NULL;

bool should_display(struct stat* stat) {
    int current_time = time(NULL);
    int mdays = (stat->st_mtim.tv_sec - current_time) / (60 * 60 * 24);
    int adays = (stat->st_atim.tv_sec - current_time) / (60 * 60 * 24);
    bool retval = true;

    if (mtime) {
        if (mtime->op == '=') {
            retval = retval && mdays == mtime->value;
        } else if (mtime->op == '-') {
            retval = retval && mdays < mtime->value;
        } else {
            retval = retval && mdays > mtime->value;
        }
    }
    if (atime) {
        if (atime->op == '=') {
            retval = retval && adays == atime->value;
        } else if (atime->op == '-') {
            retval = retval && adays < atime->value;
        } else {
            retval = retval && adays > atime->value;
        }
    }

    return retval;
}

char* get_file_type(struct stat* stat) {
    if (S_ISDIR(stat->st_mode)) return "dir";
    if (S_ISCHR(stat->st_mode)) return "char dev";
    if (S_ISBLK(stat->st_mode)) return "block dev";
    if (S_ISFIFO(stat->st_mode)) return "fifo";
    if (S_ISLNK(stat->st_mode)) return "slink";
    if (S_ISREG(stat->st_mode)) return "file";
    return "socket";
}

char* get_absolute_path(char* filename) { return realpath(filename, NULL); }

char* format_date(struct timespec date) { return ctime(&date.tv_sec); }

void explore_directory(char* directory_path, int max_depth) {
    if (max_depth == 0) return;
    chdir(directory_path);
    char* absolute_directory_path = get_absolute_path(".");

    DIR* directory_stream = opendir(".");
    struct dirent* dir;
    while ((dir = readdir(directory_stream)) != NULL) {
        if (strcmp(dir->d_name, "..") == 0 || strcmp(dir->d_name, ".") == 0) {
            continue;
        }

        char* absolute_path =
            calloc(strlen(absolute_directory_path) + strlen(dir->d_name) + 2,
                   sizeof(char));
        sprintf(absolute_path, "%s/%s", absolute_directory_path, dir->d_name);

        struct stat file_stat;
        lstat(dir->d_name, &file_stat);

        if (!should_display(&file_stat)) continue;

        printf("%s", absolute_path);
        if (S_ISLNK(file_stat.st_mode)) {
            char* tmp = realpath(absolute_path, NULL);
            printf(" -> %s", tmp);
            free(tmp);
        }
        printf("\ntype: %s", get_file_type(&file_stat));
        printf(", links: %ld", file_stat.st_nlink);
        printf(", size: %ld bytes\n", file_stat.st_size);
        printf("access: %s", format_date(file_stat.st_atim));
        printf("modify: %s\n", format_date(file_stat.st_mtim));

        if (S_ISDIR(file_stat.st_mode)) {
            explore_directory(absolute_path, max_depth - 1);
            chdir(directory_path);
        }
        free(absolute_path);
    }
    closedir(directory_stream);
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        fprintf(stderr, "You have to call this function with a path");
        return 1;
    }

    char* path = get_absolute_path(argv[1]);

    for (int i = 2; i < argc; i += 2) {
        if (strcmp(argv[i], "-mtime") == 0) {
            if (mtime) free(mtime);
            mtime = calloc(1, sizeof(arg_number));

            if ('0' <= argv[i + 1][0] && argv[i + 1][0] <= '9') {
                mtime->value = atoi(argv[i + 1]);
                mtime->op = '=';
            } else {
                mtime->value = atoi(argv[i + 1] + 1);
                mtime->op = argv[i + 1][0];
            }
        } else if (strcmp(argv[i], "-atime") == 0) {
            if (atime) free(atime);
            atime = calloc(1, sizeof(arg_number));

            if ('0' <= argv[i + 1][0] && argv[i + 1][0] <= '9') {
                atime->value = atoi(argv[i + 1]);
                atime->op = '=';
            } else {
                atime->value = atoi(argv[i + 1] + 1);
                atime->op = argv[i + 1][0];
            }
        } else {
            depth = atoi(argv[i + 1]);
        }
    }

    explore_directory(path, depth);
    free(path);
}