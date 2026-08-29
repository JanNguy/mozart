#include "mozart.h"
#include <stdlib.h>

void free_moz(moz_t *moz)
{
    free_models(moz);
    if (moz->pars->root != NULL)
        cJSON_Delete(moz->pars->root);
    free(moz->ollama_token);
    free(moz->pars);
}
