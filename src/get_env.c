#include "mozart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_env_token(const char *path)
{
    FILE *file = fopen(path, "r");
    char line[256];
    char *token = NULL;

    if (file == NULL)
        return (NULL);
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "OLLAMA_API_KEY=", 15) == 0) {
            char *value = line + 15;
            size_t vlen = strlen(value);

            while (vlen > 0 && (value[vlen - 1] == '\n' || value[vlen - 1] == '\r'))
                value[--vlen] = '\0';
            if (vlen >= 2 && value[0] == '"' && value[vlen - 1] == '"') {
                value[vlen - 1] = '\0';
                value++;
            }
            token = strdup(value);
            break;
        }
    }
    fclose(file);
    return (token);
}
