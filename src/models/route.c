#include "mozart.h"
#include <stdlib.h>

static int model_supports_type(model_t *model, type_t type)
{
    if (model->details == NULL || model->details->capabilities == NULL)
        return (0);
    for (int i = 0; model->details->capabilities[i] != NULL; i++) {
        if (capability_to_type(model->details->capabilities[i]) == type)
            return (1);
    }
    return (0);
}

model_t **find_models_for_type(moz_t *moz, type_t type)
{
    model_t **list = NULL;
    int count = 0;
    int j = 0;

    if (moz == NULL || moz->pars->models == NULL)
        return (NULL);
    for (int i = 0; moz->pars->models[i].name != NULL; i++) {
        if (model_supports_type(&moz->pars->models[i], type))
            count++;
    }
    if (count == 0)
        return (NULL);
    list = calloc(count + 1, sizeof(model_t *));
    if (list == NULL)
        return (NULL);
    for (int i = 0; moz->pars->models[i].name != NULL; i++) {
        if (model_supports_type(&moz->pars->models[i], type))
            list[j++] = &moz->pars->models[i];
    }
    return (list);
}
