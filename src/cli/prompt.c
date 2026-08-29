#include "mozart.h"
#include <stdio.h>
#include <stdlib.h>

char *read_prompt(void)
{
    char *line = NULL;
    size_t cap = 0;
    ssize_t len = 0;

    printf("> ");
    fflush(stdout);
    len = getline(&line, &cap, stdin);
    if (len < 0) {
        free(line);
        return (NULL);
    }
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
        line[--len] = '\0';
    if (len == 0) {
        free(line);
        return (NULL);
    }
    return (line);
}
