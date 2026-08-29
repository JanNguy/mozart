#include "mozart.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

static char *read_pipe(int fd)
{
    size_t cap = 4096;
    size_t len = 0;
    char *buf = malloc(cap);
    ssize_t n = 0;

    if (buf == NULL)
        return (NULL);
    while ((n = read(fd, buf + len, cap - len)) > 0) {
        len += n;
        if (len == cap) {
            cap *= 2;
            char *tmp = realloc(buf, cap);

            if (tmp == NULL) {
                free(buf);
                return (NULL);
            }
            buf = tmp;
        }
    }
    if (n < 0) {
        free(buf);
        return (NULL);
    }
    buf[len] = '\0';
    return (buf);
}

char *run_command(moz_t *moz, char **args)
{
    int pipefd[2];
    char *response = NULL;

    if (pipe(pipefd) < 0) {
        perror("run_command: pipe");
        return (NULL);
    }
    moz->pid = fork();
    if (moz->pid < 0) {
        perror("run_command: fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return (NULL);
    }
    if (moz->pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execve(args[0], args, environ);
        perror("run_command: execve");
        exit(127);
    }
    close(pipefd[1]);
    response = read_pipe(pipefd[0]);
    close(pipefd[0]);
    if (waitpid(moz->pid, &moz->status, 0) < 0)
        perror("run_command: waitpid");
    return (response);
}
