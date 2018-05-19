#include <sys/stat.h>

// EXISTS
#define LOVE_VALIDATE_FILE_EXISTS(path) do { \
	struct stat pathInfo; \
	if (stat(path, &pathInfo)) { \
		Love::RaiseError("Could not open file %s. Does not exist.", path); \
		return 0; \
	} \
} while (0)

#define LOVE_ERROR_INVALID_TYPE(what, got, expected) do { \
	const char * options = concat(expected, ", "); \
	return Love::RaiseError("Invalid %s '%s', expected one of: %s", what, got, options); \
} while (0)

// FILES
#define LOVE_VALIDATE_FILEMODE(mode) do { \
	if (!love_validate(FILE_MODES, mode)) { \
		LOVE_ERROR_INVALID_TYPE("file open mode", mode, FILE_MODES); \
		return 0; \
	} \
} while (0)

// SOURCES
#define LOVE_VALIDATE_SOURCE_TYPE(type) do { \
	if (!love_validate(SOURCE_TYPES, type)) { \
		LOVE_ERROR_INVALID_TYPE("source type", type, SOURCE_TYPES); \
		return 0; \
	} \
} while (0)