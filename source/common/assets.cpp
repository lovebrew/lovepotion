#include "common/runtime.h"
#include "common/assets.h"

#include <unistd.h>
#include <sys/stat.h>

#define GAME_FOLDER "game"

void Assets::Initialize(const std::string & path)
{
    Location location = Assets::GetLocation(path);

    switch(location)
    {
        case LOCATION_SDMC:
            Assets::directory = GAME_FOLDER;
            break;
        case LOCATION_EXTERNAL:
            romfsMountFromFsdev(path.c_str(), 0, "romfs");
            break;
        default:
        case LOCATION_ROMFS:
            break;
    }

    chdir(Assets::directory.c_str());
}

bool Assets::IsFused()
{
    return Assets::fused;
}

Location Assets::GetLocation(const std::string & path)
{
    size_t extPosition = path.find_last_of(".");
    bool isROMFSGame = false;

    if (extPosition != std::string::npos)
        isROMFSGame = (path.substr(extPosition) == ".lpx");

    if (!isROMFSGame)
    {
        if (std::filesystem::is_directory(GAME_FOLDER))
            return LOCATION_SDMC;
    }

    Result rc = romfsInit();

    // load our external ROMFS game
    if (R_SUCCEEDED(rc) && isROMFSGame)
        return LOCATION_EXTERNAL;

    // load no game / fused
    return LOCATION_ROMFS;
}
