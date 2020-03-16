#define _GNU_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

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

    int max_depth = -1;

    for (int i = 2; i < argc; i += 2) {
        if (strcmp(argv[i], "-mtime") == 0) {
        } else if (strcmp(argv[i], "-atime") == 0) {
        } else {
            max_depth = atoi(argv[i + 1]);
        }
    }

    explore_directory(path, max_depth);
    free(path);
}