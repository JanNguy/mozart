#include "mozart.h"
#include <stdlib.h>
#include <string.h>

int init_moz(moz_t *moz)
{
    const char *env_token = NULL;

    moz->pars = calloc(1, sizeof(*moz->pars));
    if (moz->pars == NULL)
        return (1);
    moz->pars->cloud_models = true;
    moz->pars->local_models = true;
    env_token = getenv("OLLAMA_API_KEY");
    moz->ollama_token = env_token != NULL ? strdup(env_token) : get_env_token(".env");
    return (0);
}
