//CHECK
const char * concat(const std::vector<const char *> & expected, const char * delimeter);

bool LOVE_VALIDATE(const std::vector<const char *> & expected, const char * value);

// EXISTS
void LOVE_VALIDATE_FILE_EXISTS(const char * path);

void LOVE_ERROR_INVALID_TYPE(const char * what,  const char * got, const std::vector<const char *> & expected);

// FILES
void LOVE_VALIDATE_FILEMODE(const char * mode);

// SOURCES
void LOVE_VALIDATE_SOURCE_TYPE(const char * type);