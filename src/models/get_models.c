#include "mozart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *default_ollama_url = "https://ollama.com/api/tags";
const char *default_show_url = "https://ollama.com/api/show";

static char *fetch_models(moz_t *moz)
{
    char auth_header[512];
    char *args[] = {
        "/usr/bin/curl",
        "-s",
        "-H", auth_header,
        (char *)default_ollama_url,
        NULL
    };

    snprintf(auth_header, sizeof(auth_header),
        "Authorization: Bearer %s", moz->ollama_token);
    return (run_command(moz, args));
}

static char *fetch_show(moz_t *moz, const char *model)
{
    char auth_header[512];
    char body[512];
    char *args[] = {
        "/usr/bin/curl",
        "-s",
        "-H", auth_header,
        "-H", "Content-Type: application/json",
        "-d", body,
        (char *)default_show_url,
        NULL
    };

    snprintf(auth_header, sizeof(auth_header),
        "Authorization: Bearer %s", moz->ollama_token);
    snprintf(body, sizeof(body), "{\"model\": \"%s\"}", model);
    return (run_command(moz, args));
}

static char **parse_capabilities(const char *response)
{
    cJSON *root = NULL;
    cJSON *caps = NULL;
    cJSON *item = NULL;
    char **list = NULL;
    int count = 0;
    int i = 0;

    if (response == NULL)
        return (NULL);
    root = cJSON_Parse(response);
    if (root == NULL)
        return (NULL);
    caps = cJSON_GetObjectItemCaseSensitive(root, "capabilities");
    if (caps == NULL || !cJSON_IsArray(caps)) {
        cJSON_Delete(root);
        return (NULL);
    }
    count = cJSON_GetArraySize(caps);
    list = calloc(count + 1, sizeof(char *));
    if (list == NULL) {
        cJSON_Delete(root);
        return (NULL);
    }
    cJSON_ArrayForEach(item, caps) {
        if (cJSON_IsString(item))
            list[i++] = strdup(item->valuestring);
    }
    cJSON_Delete(root);
    return (list);
}

static details_t *parse_details(cJSON *item)
{
    cJSON *details_obj = cJSON_GetObjectItemCaseSensitive(item, "details");
    cJSON *field = NULL;
    details_t *det = NULL;

    if (details_obj == NULL || !cJSON_IsObject(details_obj))
        return (NULL);
    det = calloc(1, sizeof(details_t));
    if (det == NULL)
        return (NULL);
    field = cJSON_GetObjectItemCaseSensitive(details_obj, "parent_model");
    if (field != NULL && cJSON_IsString(field))
        det->parent_model = strdup(field->valuestring);
    field = cJSON_GetObjectItemCaseSensitive(details_obj, "format");
    if (field != NULL && cJSON_IsString(field))
        det->format = strdup(field->valuestring);
    field = cJSON_GetObjectItemCaseSensitive(details_obj, "families");
    if (field != NULL && cJSON_IsString(field))
        det->families = strdup(field->valuestring);
    field = cJSON_GetObjectItemCaseSensitive(details_obj, "parameter_size");
    if (field != NULL && cJSON_IsString(field))
        det->parameter_size = strdup(field->valuestring);
    field = cJSON_GetObjectItemCaseSensitive(details_obj, "quantization_level");
    if (field != NULL && cJSON_IsString(field))
        det->quatization_level = strdup(field->valuestring);
    return (det);
}

static void parse_model(moz_t *moz, cJSON *item, model_t *model)
{
    char *show = NULL;

    model->name_nc = cJSON_GetObjectItemCaseSensitive(item, "name");
    model->model_nc = cJSON_GetObjectItemCaseSensitive(item, "model");
    model->modified_at_nc = cJSON_GetObjectItemCaseSensitive(item, "modified_at");
    model->size_nc = cJSON_GetObjectItemCaseSensitive(item, "size");
    model->digest_nc = cJSON_GetObjectItemCaseSensitive(item, "digest");
    model->details_nc = cJSON_GetObjectItemCaseSensitive(item, "details");

    if (model->name_nc != NULL && cJSON_IsString(model->name_nc))
        model->name = strdup(model->name_nc->valuestring);
    if (model->model_nc != NULL && cJSON_IsString(model->model_nc))
        model->model = strdup(model->model_nc->valuestring);
    if (model->digest_nc != NULL && cJSON_IsString(model->digest_nc))
        model->digest = strdup(model->digest_nc->valuestring);
    model->details = parse_details(item);
    if (model->name != NULL && model->details != NULL) {
        show = fetch_show(moz, model->name);
        model->details->capabilities = parse_capabilities(show);
        free(show);
    }
}

static void parse_models(moz_t *moz, const char *response)
{
    cJSON *root = NULL;
    cJSON *models = NULL;
    cJSON *item = NULL;
    model_t *arr = NULL;
    int count = 0;
    int i = 0;

    root = cJSON_Parse(response);
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
        parse_model(moz, item, &arr[i]);
        i++;
    }
    moz->pars->models = arr;
    moz->pars->root = root;
}

void get_models(moz_t *moz)
{
    char *response = NULL;

    if (moz == NULL || moz->pars == NULL || moz->ollama_token == NULL) {
        fprintf(stderr, "get_models: moz_t invalide (token requis)\n");
        return;
    }
    response = fetch_models(moz);
    if (response == NULL)
        return;
    parse_models(moz, response);
    free(response);
}
