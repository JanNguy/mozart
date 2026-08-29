#include "mozart.h"
#include <stdio.h>
#include <stdlib.h>

type_t predict_type(moz_t *moz, const char *prompt)
{
    char *output = NULL;
    int value = 0;

    if (moz == NULL || prompt == NULL)
        return (AUTRE);
    char *args[] = {
        "ml/.venv/bin/python",
        "ml/predict.py",
        (char *)prompt,
        NULL
    };
    output = run_command(moz, args);
    if (output == NULL)
        return (AUTRE);
    value = atoi(output);
    free(output);
    if (value < CODE || value > AUTRE)
        return (AUTRE);
    return ((type_t)value);
}
