#include "common/runtime.h"
#include "common/assets.h"

void Assets::Initialize(char * path)
{
    uint gameAssets = Assets::GetLocation(path);

    switch(gameAssets)
    {
        case 0:
        {
            struct stat pathInfo;

            stat("game", &pathInfo);

            if (S_ISDIR(pathInfo.st_mode))
                location = "game";

            break;
        }
        case 1:
            #if defined (__SWITCH__)
                if (romfsMountFromFsdev(path, 0, "romfs") == 0)
                    isRomfsInitialized = true;
            #endif

            break;
        default:
            break;
    }

    chdir(location.c_str());
}

uint Assets::GetLocation(char * path)
{
    const char * ext = strrchr(path, '.');

    if (strncmp(ext, ".lpx", 4) != 0)
    {
        Result rc = romfsInit();

        if (rc != 0)
            return 0;

        return 1;
    }

    return 3;
}
