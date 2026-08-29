#include "mozart.h"
#include <string.h>

type_t capability_to_type(const char *capability)
{
    if (capability == NULL)
        return (AUTRE);
    if (strcmp(capability, "completion") == 0)
        return (CONVERSATION);
    if (strcmp(capability, "tools") == 0)
        return (CODE);
    if (strcmp(capability, "thinking") == 0)
        return (MATH);
    if (strcmp(capability, "vision") == 0)
        return (RAG);
    if (strcmp(capability, "embed") == 0)
        return (RAG);
    return (AUTRE);
}
