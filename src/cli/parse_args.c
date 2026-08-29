#include "mozart.h"
#include <string.h>

void parse_args(int ac, char **av, parsing_t *arg)
{
    for (int i = 1; i < ac; i++) {
        if (strcmp(av[i], "--nolocal") == 0)
            arg->local_models = false;
        if (strcmp(av[i], "--nocloud") == 0)
            arg->cloud_models = false;
    }
}
