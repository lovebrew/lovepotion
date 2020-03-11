/*
** common/assets.h
** @brief   : Loads game content
*/

#pragma once

enum Location
{
    LOCATION_SDMC = 0,
    LOCATION_ROMFS,
    LOCATION_EXTERNAL
};

namespace Assets
{
    /* Variables */

    inline std::string directory = "romfs:";
    inline std::string writePath = "";

    /* Functions */

    void Initialize(const std::string & path);

    Location GetLocation(const std::string & path);

    std::string GetWritePath();
};
