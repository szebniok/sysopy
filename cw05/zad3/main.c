#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

void run_child(char* path, int producer) {
    if (fork() == 0) {
        if (producer) {
            execlp("./producer", "./producer", "pipe", path, "10", NULL);
        } else {
            execlp("./consumer", "./consumer", "pipe", path, "10", NULL);
        }
    }
}

int main() {
    mkfifo("pipe", S_IRUSR | S_IWUSR);

    run_child("1.txt", 1);
    run_child("A.txt", 1);
    run_child("B.txt", 1);
    run_child("p.txt", 1);
    run_child("d.txt", 1);
    run_child("out.txt", 0);

    for (int i = 0; i < 6; i++) {
        wait(NULL);
    }
}