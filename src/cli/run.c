#include "mozart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *type_name(type_t type)
{
    switch (type) {
    case CODE:
        return ("CODE");
    case MATH:
        return ("MATH");
    case RAG:
        return ("RAG");
    case CONVERSATION:
        return ("CONVERSATION");
    default:
        return ("AUTRE");
    }
}

static void model_specialties(model_t *model, char *buf, size_t size)
{
    bool seen[5] = {false};
    size_t len = 0;

    buf[0] = '\0';
    if (model->details == NULL || model->details->capabilities == NULL)
        return;
    for (int i = 0; model->details->capabilities[i] != NULL; i++) {
        type_t type = capability_to_type(model->details->capabilities[i]);

        if (seen[type])
            continue;
        seen[type] = true;
        if (len > 0)
            len += snprintf(buf + len, size - len, ", ");
        len += snprintf(buf + len, size - len, "%s", type_name(type));
    }
}

static model_t **all_models(moz_t *moz)
{
    int count = 0;
    model_t **list = NULL;

    for (int i = 0; moz->pars->models[i].name != NULL; i++)
        count++;
    list = calloc(count + 1, sizeof(model_t *));
    if (list == NULL)
        return (NULL);
    for (int i = 0; i < count; i++)
        list[i] = &moz->pars->models[i];
    return (list);
}

static void run_menu(moz_t *moz, model_t **candidates, const char *prompt)
{
    int n = 0;
    int choice = 0;

    for (int i = 0; candidates[i] != NULL; i++)
        n++;
    printf("Modèles capables de cette catégorie :\n");
    for (int i = 0; i < n; i++) {
        char specialties[128];

        model_specialties(candidates[i], specialties, sizeof(specialties));
        printf("  %d. %s - %s\n", i + 1, candidates[i]->name, specialties);
    }
    printf("Choix (0 = annuler) : ");
    fflush(stdout);
    if (scanf("%d", &choice) != 1)
        return;
    while (getchar() != '\n') {
    }
    if (choice < 1 || choice > n)
        return;
    printf("--- %s ---\n", candidates[choice - 1]->name);
    execute_model(moz, candidates[choice - 1], prompt);
}

void run_interactive(moz_t *moz)
{
    char *prompt = NULL;
    type_t type = AUTRE;
    model_t **candidates = NULL;

    if (moz->pars->models == NULL) {
        fprintf(stderr, "aucun modèle disponible\n");
        return;
    }
    while ((prompt = read_prompt()) != NULL) {
        type = predict_type(moz, prompt);
        printf("Type déduit : %s\n", type_name(type));
        candidates = find_models_for_type(moz, type);
        if (candidates == NULL) {
            printf("Aucun modèle ne couvre %s — voici tous les modèles :\n",
                type_name(type));
            candidates = all_models(moz);
            if (candidates == NULL) {
                free(prompt);
                continue;
            }
        }
        run_menu(moz, candidates, prompt);
        free(candidates);
        free(prompt);
    }
}
