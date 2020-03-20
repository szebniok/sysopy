#define _GNU_SOURCE
#include <dirent.h>
#include <ftw.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int nftw_callback(const char* file_path, const struct stat* stat, int typeflag,
                  struct FTW* ftw) {
    if (typeflag != FTW_D) return 0;

    if (fork() == 0) {
        printf("pid: %d\n", getpid());
        puts(file_path);
        execlp("ls", "ls", "-l", file_path, NULL);
    }

    // ignore unused warning
    (void)ftw;
    (void)stat;

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        fprintf(stderr, "You have to call this function with a path");
        return 1;
    }

    char* path = realpath(argv[1], NULL);

    nftw(path, nftw_callback, 7, FTW_PHYS);
    free(path);
}