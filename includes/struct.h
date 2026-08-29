#ifndef STRUCT_H
    #define STRUCT_H
    #include <stdbool.h>
    #include <sys/types.h>
    #include <cjson/cJSON.h>

typedef enum type_s {
    CODE,
    MATH,
    RAG,
    CONVERSATION,
    AUTRE
} type_t;

typedef struct date_s {
    char heure[2];
    char min[2];
    char sec[2];
    char jour[2];
    char mois[2];
    char anneee[2];
} date_t;

typedef struct details_s {
    char *parent_model;
    char *format;
    char *families;
    char *parameter_size;
    char *quatization_level;
} details_t;

typedef struct model_s {
    /*not converted csjon raw format. nc -> not converted*/
    cJSON *name_nc;
    cJSON *model_nc;
    cJSON *modified_at_nc;
    cJSON *size_nc;
    cJSON *digest_nc;
    cJSON *details_nc;

    /*cjson converted to C varaibles*/
    char *name;
    char *model;
    date_t *modified_at;
    char *digest;
    details_t *details;
} model_t;

typedef struct parsing_s {
    bool cloud_models;
    bool local_models;

    /*Ollama models*/
    model_t *models;
    cJSON *root;
} parsing_t;

typedef struct moz_s {
    char *ollama_token;

    parsing_t *pars;

    /*process curl lance dans get_models*/
    pid_t pid;
    int status;
} moz_t;

#endif /*struct_H*/