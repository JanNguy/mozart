#include "mozart.h"
#include <stdio.h>

void print_models(moz_t *moz)
{
    if (moz->pars->models == NULL)
        return;
    for (int i = 0; moz->pars->models[i].name != NULL; i++)
        printf("%s\n", moz->pars->models[i].name);
}

void print_capabilities(moz_t *moz)
{
    if (moz->pars->models == NULL)
        return;
    for (int i = 0; moz->pars->models[i].name != NULL; i++) {
        if (moz->pars->models[i].details == NULL ||
            moz->pars->models[i].details->capabilities == NULL)
            continue;
        for (int j = 0; moz->pars->models[i].details->capabilities[j] != NULL; j++)
            printf("%s\n", moz->pars->models[i].details->capabilities[j]);
    }
}
