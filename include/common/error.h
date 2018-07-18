//CHECK
std::string concat(const std::vector<const char *> & expected, const char * delimeter);

bool LOVE_VALIDATE(const std::vector<const char *> & expected, const char * value);

// EXISTS
bool LOVE_VALIDATE_FILE_EXISTS_CLEAN(const char * path);

void LOVE_VALIDATE_FILE_EXISTS(const char * path);

//TYPES
void LOVE_ERROR_INVALID_TYPE(const char * what,  const char * got, const std::vector<const char *> & expected);

// FILES
void LOVE_VALIDATE_FILEMODE(const char * mode);

// SOURCES
void LOVE_VALIDATE_SOURCE_TYPE(const char * type);

//DRAW MODES
void LOVE_VALIDATE_DRAW_MODE(const char * mode);