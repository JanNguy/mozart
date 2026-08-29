#include "mozart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *default_chat_url = "https://ollama.com/api/chat";

static char *json_escape(const char *s)
{
    size_t cap = strlen(s) * 2 + 1;
    char *out = malloc(cap);
    size_t j = 0;

    if (out == NULL)
        return (NULL);
    for (size_t i = 0; s[i] != '\0'; i++) {
        if (s[i] == '"' || s[i] == '\\') {
            out[j++] = '\\';
            out[j++] = s[i];
        } else if (s[i] == '\n') {
            out[j++] = '\\';
            out[j++] = 'n';
        } else if (s[i] == '\t') {
            out[j++] = '\\';
            out[j++] = 't';
        } else {
            out[j++] = s[i];
        }
    }
    out[j] = '\0';
    return (out);
}

void execute_model(moz_t *moz, model_t *model, const char *prompt)
{
    char auth_header[512];
    char *escaped = NULL;
    char *body = NULL;
    char *response = NULL;
    cJSON *root = NULL;
    cJSON *msg = NULL;
    cJSON *content = NULL;
    char *args[10];

    if (moz == NULL || model == NULL || prompt == NULL)
        return;
    snprintf(auth_header, sizeof(auth_header),
        "Authorization: Bearer %s", moz->ollama_token);
    escaped = json_escape(prompt);
    if (escaped == NULL)
        return;
    if (asprintf(&body,
            "{\"model\":\"%s\",\"messages\":[{\"role\":\"user\",\"content\":\"%s\"}],\"stream\":false}",
            model->name, escaped) < 0) {
        free(escaped);
        return;
    }
    args[0] = "/usr/bin/curl";
    args[1] = "-s";
    args[2] = "-H";
    args[3] = auth_header;
    args[4] = "-H";
    args[5] = "Content-Type: application/json";
    args[6] = "-d";
    args[7] = body;
    args[8] = (char *)default_chat_url;
    args[9] = NULL;
    response = run_command(moz, args);
    if (response != NULL) {
        root = cJSON_Parse(response);
        if (root != NULL) {
            msg = cJSON_GetObjectItemCaseSensitive(root, "message");
            if (msg != NULL) {
                content = cJSON_GetObjectItemCaseSensitive(msg, "content");
                if (content != NULL && cJSON_IsString(content))
                    printf("%s\n", content->valuestring);
            }
            cJSON_Delete(root);
        }
        free(response);
    }
    free(body);
    free(escaped);
}
