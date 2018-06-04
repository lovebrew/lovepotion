#include "common/runtime.h"

char * Object::ToString(const char * type)
{
	char buffer[64];
	snprintf(buffer, 64, "%s: %p", type, this);

	return strdup(buffer);
}