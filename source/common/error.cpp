#include "common/runtime.h"
#include <sys/stat.h>

void LOVE_VALIDATE_FILE_EXISTS(const char * path)
{
	struct stat pathInfo;
	if (stat(path, &pathInfo))
		Love::RaiseError("Could not open file %s. Does not exist.", path);
}

void LOVE_ERROR_INVALID_TYPE(const char * what,  const char * got, const std::vector<const char *> & expected)
{
	const char * options = concat(expected, ", ");
	Love::RaiseError("Invalid %s '%s', expected one of: %s", what, got, options);
}

void LOVE_VALIDATE_FILEMODE(const char * mode)
{
	if (!love_validate(FILE_MODES, mode))
		LOVE_ERROR_INVALID_TYPE("file open mode", mode, FILE_MODES);
}

void LOVE_VALIDATE_SOURCE_TYPE(const char * type)
{
	if (!love_validate(SOURCE_TYPES, type))
		LOVE_ERROR_INVALID_TYPE("source type", type, SOURCE_TYPES);
}