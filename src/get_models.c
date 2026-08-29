#include "mozart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

const char *default_ollama_url = "https://ollama.com/api/tags";

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

void get_models(moz_t *moz)
{
    if (moz == NULL || moz->pars == NULL || moz->ollama_token == NULL) {
        fprintf(stderr, "get_models: moz_t invalide (token requis)\n");
        return;
    }

    char auth_header[512];
    int pipefd[2];
    char *response = NULL;
    cJSON *root = NULL;
    cJSON *models = NULL;
    cJSON *item = NULL;
    model_t *arr = NULL;
    int count = 0;
    int i = 0;

    snprintf(auth_header, sizeof(auth_header),
        "Authorization: Bearer %s", moz->ollama_token);

    char *args[] = {
        "/usr/bin/curl",
        "-s",
        "-H", auth_header,
        (char *)default_ollama_url,
        NULL
    };

    if (pipe(pipefd) < 0) {
        perror("get_models: pipe");
        return;
    }
    moz->pid = fork();
    if (moz->pid < 0) {
        perror("get_models: fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return;
    }
    if (moz->pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execve(args[0], args, environ);
        perror("get_models: execve");
        exit(127);
    }
    close(pipefd[1]);
    response = read_pipe(pipefd[0]);
    close(pipefd[0]);
    if (waitpid(moz->pid, &moz->status, 0) < 0)
        perror("get_models: waitpid");
    if (response == NULL)
        return;

    root = cJSON_Parse(response);
    free(response);
    if (root == NULL) {
        fprintf(stderr, "get_models: cJSON_Parse failed\n");
        return;
    }
    models = cJSON_GetObjectItemCaseSensitive(root, "models");
    if (models == NULL || !cJSON_IsArray(models)) {
        cJSON_Delete(root);
        return;
    }
    count = cJSON_GetArraySize(models);
    arr = calloc(count + 1, sizeof(model_t));
    if (arr == NULL) {
        cJSON_Delete(root);
        return;
    }
    cJSON_ArrayForEach(item, models) {
        arr[i].name_nc = cJSON_GetObjectItemCaseSensitive(item, "name");
        arr[i].model_nc = cJSON_GetObjectItemCaseSensitive(item, "model");
        arr[i].modified_at_nc = cJSON_GetObjectItemCaseSensitive(item, "modified_at");
        arr[i].size_nc = cJSON_GetObjectItemCaseSensitive(item, "size");
        arr[i].digest_nc = cJSON_GetObjectItemCaseSensitive(item, "digest");
        arr[i].details_nc = cJSON_GetObjectItemCaseSensitive(item, "details");

        if (arr[i].name_nc != NULL && cJSON_IsString(arr[i].name_nc))
            arr[i].name = strdup(arr[i].name_nc->valuestring);
        if (arr[i].model_nc != NULL && cJSON_IsString(arr[i].model_nc))
            arr[i].model = strdup(arr[i].model_nc->valuestring);
        if (arr[i].digest_nc != NULL && cJSON_IsString(arr[i].digest_nc))
            arr[i].digest = strdup(arr[i].digest_nc->valuestring);
        i++;
    }
    moz->pars->models = arr;
    moz->pars->root = root;
}
