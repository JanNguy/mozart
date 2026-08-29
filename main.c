#include "mozart.h"

int main(int ac, char **av)
{
    moz_t moz[1] = {0};

    if (init_moz(moz) != 0)
        return (1);
    parse_args(ac, av, moz->pars);
    get_models(moz);
    run_interactive(moz);
    free_moz(moz);
    return (0);
}
