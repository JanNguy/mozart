#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mozart.h"
#include <cjson/cJSON.h>

static void get_arg(char **av, int ac, parsing_t *arg)
{
    for (int i = 1; i < ac; i++) {
        if (strcmp(av[i], "--nolocal") == 0)
            arg->local_models = false;
        if (strcmp(av[i], "--nocloud") == 0)
            arg->cloud_models = false;
    }
}

int main(int ac, char **av)
{
    moz_t moz[1] = {0};
    const char *env_token = NULL;

    moz->pars = calloc(1, sizeof(*moz->pars));
    if (moz->pars == NULL)
        return (1);
    moz->pars->cloud_models = true;
    moz->pars->local_models = true;
    env_token = getenv("OLLAMA_API_KEY");
    moz->ollama_token = env_token != NULL ? strdup(env_token) : get_env_token(".env");
    get_arg(av, ac, moz->pars);
    get_models(moz);
    if (moz->pars->models != NULL) {
        for (int i = 0; moz->pars->models[i].name != NULL; i++)
            printf("%s\n", moz->pars->models[i].name);
    }
    if (moz->pars->models != NULL) {
        for (int i = 0; moz->pars->models[i].name != NULL; i++) {
            free(moz->pars->models[i].name);
            free(moz->pars->models[i].model);
            free(moz->pars->models[i].digest);
        }
        free(moz->pars->models);
    }
    if (moz->pars->root != NULL)
        cJSON_Delete(moz->pars->root);
    free(moz->ollama_token);
    free(moz->pars);
    return (0);
}
