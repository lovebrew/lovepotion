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
    char buffer[SOCKET_BUFFERSIZE];
    char ip[0x41];
    int port;
} Datagram;

typedef struct
{
    int x;
    int y;
    int subWidth;
    int subHeight;

    int width;
    int height;
} Viewport;

typedef struct
{
    char * buffer;
    size_t size;
    int code;
    char * header;
} HTTPChunk;

extern std::vector<const char *> FILE_MODES;

extern std::vector<const char *> SOURCE_TYPES;

extern std::vector<const char *> DRAW_MODES;