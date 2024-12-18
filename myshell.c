#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h> 

#define MAX_COMMANDS 100
#define MAX_PATHS 100
#define MAX_LENGTH 100

char *command_history[MAX_COMMANDS]; // storing the history of commands.
char *paths_args[MAX_PATHS]; // storing search paths.
int command_count = 0; // track of number of commands stored in the history.
int path_count = 0; // track of the number of paths from input.

// print the command history.
void print_history() {
    // iterate the command_history array and prints each command.
    for (int i = 0; i < command_count; i++) {
        printf("%s", command_history[i]);
    }
}

// change the current working directory to the specified path.
void cd(char *path) {
    // if the change fails it prints an error message.
    if (chdir(path) != 0) {
        perror("chdir failed");
    }
}

// print the current working directory.
void pwd() {
    char cwd[MAX_LENGTH];
    // if it fails print an error message.
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd failed");
    }
}

void execute_command(char *command) {
    char *args[MAX_LENGTH];
    char *part_of_command = strtok(command, " ");
    int part_of_command_count = 0;
    // split the command into arguments.
    while (part_of_command != NULL) {
        args[part_of_command_count++] = part_of_command;
        part_of_command = strtok(NULL, " ");
    }
    // end of array.
    args[part_of_command_count] = NULL;
    // if there is no command return.
    if (part_of_command_count == 0) {
        return;
    }
    // handle default commands.
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "cd") == 0) {
        cd(args[1]);
    } else if (strcmp(args[0], "pwd") == 0) {
        pwd();
    } else if (strcmp(args[0], "history") == 0) {
        print_history();
    } else if (strcmp(args[0], "cat") == 0) {
        // If no file name is provided, read from standard input.
        if (part_of_command_count < 2) {
            char buffer[1024];
            ssize_t bytes_read;
            while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
                if (write(STDOUT_FILENO, buffer, bytes_read) != bytes_read) {
                    perror("write failed");
                    break;
                }
            }
            if (bytes_read == -1) {
                perror("read failed");
            }
        } else {
            for (int i = 1; i < part_of_command_count; i++) {
                int fd = open(args[i], O_RDONLY);
                if (fd == -1) {
                    printf("cat: %s: No such file or directory\n", args[i]);
                    continue;
                }
                char buffer[1024];
                ssize_t bytes_read;
                while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
                    if (write(STDOUT_FILENO, buffer, bytes_read) != bytes_read) {
                        perror("write failed");
                        break;
                    }
                }
                if (bytes_read == -1) {
                    perror("read failed");
                }
                close(fd);
            }
        }
    } else {
        // execute external commands. create child process.
        pid_t pid = fork();
        // fork() fail.
        if (pid == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        // child process.
        } else if (pid == 0) {
            // try to execute the command directly.
            if (execvp(args[0], args) == -1) {
                // if exec failed try to find the command in the provided paths.
                for (int i = 0; i < path_count; i++) {
                    char full_path[MAX_LENGTH];
                    snprintf(full_path, sizeof(full_path), "%s/%s", paths_args[i], args[0]);
                    if (execv(full_path, args) == -1) {
                        continue; // try the next path if execv fails.
                    }
                }
                // if not in provided paths and exxec failed, print an error and exit the child process.
                perror("exec failed");
                exit(EXIT_FAILURE);
            }
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

// add a command to the command history.
void add_to_history(const char *command) {
    // if got to limit exit the program.
    if (command_count < MAX_COMMANDS) {
        command_history[command_count++] = strdup(command);
    } else {
        exit(EXIT_FAILURE);
    }
}

// add path to the list of paths.
void add_path(const char *path) {
    // if got to limit exit the program.
    if (path_count < MAX_PATHS) {
        paths_args[path_count++] = strdup(path);
    } else {
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    char command[MAX_LENGTH];
    // store the command-line path arguments.
    for (int i = 1; i < argc; i++) {
        add_path(argv[i]);
    }
    // infinite loop to get input from user and execute commands.
    while (1) {
        // print prompt for user.
        printf("$ ");
        // flush the output buffer.
        fflush(stdout);
        // read the command input and store it in array, exit if its null.
        if (fgets(command, sizeof(command), stdin) == NULL) {
            exit(EXIT_FAILURE);
        }
        // save command in history of commands.
        add_to_history(command);
        // remove the newline character to prevent issues with processing command.
        command[strcspn(command, "\n")] = 0;
        // execute the command.
        execute_command(command);
    }
    return 0;
}