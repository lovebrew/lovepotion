#include "common/runtime.h"
#include <sys/stat.h>

std::string concat(const std::vector<const char *> & expected, const char * delimeter)
{
    std::string returnValue;
    int reservedCapacity = 0;

    // Find the reserve capacity
    // string + (' * 2) + delimeter size
    for (const char * value : expected)
    {
        std::string strValue = value;
        reservedCapacity += (strValue.length() + 2 + std::string(delimeter).length());
    }

    returnValue.reserve(reservedCapacity);
    for (const char * value : expected)
    {
        std::string strValue = value;
        returnValue += ("'" + strValue + "'" + ((strValue == std::string(expected.back()) ? "" : std::string(delimeter))));
    }

    return returnValue;
}

bool LOVE_VALIDATE(const std::vector<const char *> & expected, const char * type)
{
    bool isValid = false;
    for (const char * value : expected)
    {
        if (strncmp(value, type, strlen(value)) == 0)
        {
            isValid = true;
            break;
        }
    }

    return isValid;
}

bool LOVE_VALIDATE_FILE_EXISTS_CLEAN(const char * path)
{
    struct stat pathInfo;
    if (stat(path, &pathInfo) != 0)
        return false;
    return true && S_ISREG(pathInfo.st_mode);
}

void LOVE_VALIDATE_FILE_EXISTS(const char * path)
{
    struct stat pathInfo;
    if (stat(path, &pathInfo) != 0)
        Love::RaiseError("Could not open file %s. Does not exist.", path);
}

void LOVE_ERROR_INVALID_TYPE(const char * what,  const char * got, const std::vector<const char *> & expected)
{
    std::string options = concat(expected, ", ");
    Love::RaiseError("Invalid %s '%s', expected one of: %s", what, got, options.c_str());
}

void LOVE_VALIDATE_FILEMODE(const char * mode)
{
    if (!LOVE_VALIDATE(FILE_MODES, mode))
        LOVE_ERROR_INVALID_TYPE("file open mode", mode, FILE_MODES);
}

void LOVE_VALIDATE_SOURCE_TYPE(const char * type)
{
    if (!LOVE_VALIDATE(SOURCE_TYPES, type))
        LOVE_ERROR_INVALID_TYPE("source type", type, SOURCE_TYPES);
}

void LOVE_VALIDATE_DRAW_MODE(const char * mode)
{
    if (!LOVE_VALIDATE(DRAW_MODES, mode))
        LOVE_ERROR_INVALID_TYPE("draw mode", mode, SOURCE_TYPES);
}