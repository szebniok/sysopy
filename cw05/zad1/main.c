#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 10000
#define MAX_ARGS 5
#define MAX_CMDS 5

typedef struct {
    char** args;
} command;

void strip_ending_whitespace(char* s) {
    int i = strlen(s) - 1;
    while (i >= 0 && isspace(s[i])) {
        s[i] = '\0';
        i--;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./main file\n");
        return 1;
    }

    FILE* input_file = fopen(argv[1], "r");
    char line_buffer[MAX_LINE_LENGTH + 1];

    while (fgets(line_buffer, MAX_LINE_LENGTH, input_file) != NULL) {
        strip_ending_whitespace(line_buffer);
        printf("%s :\n", line_buffer);

        command* commands[MAX_CMDS] = {NULL};
        int command_index = 0;

        char* encoded_cmd = strtok(line_buffer, "|");
        while (encoded_cmd != NULL) {
            char** args = calloc(MAX_ARGS + 2, sizeof(char*));
            char* tok_buffer;
            char* token = strtok_r(encoded_cmd, " ", &tok_buffer);
            args[0] = strdup(token);
            token = strtok_r(NULL, " ", &tok_buffer);
            int i = 1;
            while (token != NULL) {
                args[i++] = strdup(token);

                token = strtok_r(NULL, " ", &tok_buffer);
            }

            command* cmd = calloc(1, sizeof(command));
            cmd->args = args;
            commands[command_index++] = cmd;

            encoded_cmd = strtok(NULL, "|");
        }

        int i;
        int pipes[2], prev_pipes[2];
        for (i = 0; commands[i] != NULL; i++) {
            pipe(pipes);

            pid_t pid = fork();
            if (pid == 0) {
                if (i != 0) {
                    dup2(prev_pipes[0], STDIN_FILENO);
                    close(prev_pipes[1]);
                }
                dup2(pipes[1], STDOUT_FILENO);
                execvp(commands[i]->args[0], commands[i]->args);
            }

            close(pipes[1]);

            prev_pipes[0] = pipes[0];
            prev_pipes[1] = pipes[1];
        }

        while (i-- > 0) {
            wait(NULL);
        }

        char buffer[257] = {0};
        while (read(pipes[0], buffer, sizeof(buffer) - 1) > 0) {
            printf("%s", buffer);
            memset(buffer, 0, sizeof(buffer));
        }
        puts("");
    }
}