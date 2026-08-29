#include "mozart.h"
#include <stdlib.h>

void free_models(moz_t *moz)
{
    if (moz->pars->models == NULL)
        return;
    for (int i = 0; moz->pars->models[i].name != NULL; i++) {
        free(moz->pars->models[i].name);
        free(moz->pars->models[i].model);
        free(moz->pars->models[i].digest);
        if (moz->pars->models[i].details != NULL) {
            free(moz->pars->models[i].details->parent_model);
            free(moz->pars->models[i].details->format);
            free(moz->pars->models[i].details->families);
            free(moz->pars->models[i].details->parameter_size);
            free(moz->pars->models[i].details->quatization_level);
            if (moz->pars->models[i].details->capabilities != NULL) {
                for (int j = 0; moz->pars->models[i].details->capabilities[j] != NULL; j++)
                    free(moz->pars->models[i].details->capabilities[j]);
                free(moz->pars->models[i].details->capabilities);
            }
            free(moz->pars->models[i].details);
        }
    }
    free(moz->pars->models);
}
