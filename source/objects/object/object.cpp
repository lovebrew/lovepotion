#include "common/runtime.h"

char * Object::ToString()
{
    char buffer[64];
    snprintf(buffer, 64, "%s: %p", this->type, this);

    return strdup(buffer);
}