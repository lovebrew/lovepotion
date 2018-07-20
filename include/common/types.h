#pragma once

#include <string>
#include <vector>
#include <map>

#include "socket/common.h"

typedef struct
{
    double ox;
    double oy;

    double r;

    double sx;
    double sy;

    double kx;
    double ky;
} StackMatrix;

typedef enum
{
    BOOLEAN,
    LUSERDATA,
    NIL,
    NUMBER,
    STRING,
    TABLE,
    UNKNOWN,
    USERDATA
} VARIANTS;

typedef enum
{
    OBJECTS
} REGISTRY;

typedef struct
{
    char * buffer;
    char * ip;
    int port;
} Datagram;

extern std::vector<const char *> FILE_MODES;

extern std::vector<const char *> SOURCE_TYPES;

extern std::vector<const char *> DRAW_MODES;