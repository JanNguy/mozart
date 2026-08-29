#ifndef MOZART_H
    #define MOZART_H
    #include "struct.h"

char *get_env_token(const char *path);
void parse_args(int ac, char **av, parsing_t *arg);
int init_moz(moz_t *moz);
void free_moz(moz_t *moz);
void get_models(moz_t *moz);
void print_models(moz_t *moz);
void print_capabilities(moz_t *moz);
type_t capability_to_type(const char *capability);
void free_models(moz_t *moz);
char *run_command(moz_t *moz, char **args);
char *read_prompt(void);
void run_interactive(moz_t *moz);
type_t predict_type(moz_t *moz, const char *prompt);
model_t **find_models_for_type(moz_t *moz, type_t type);
void execute_model(moz_t *moz, model_t *model, const char *prompt);

#endif /*MOZART_H*/
