#pragma once

namespace Love
{
    #define LOVE_VERSION "11.2"
    #define LOVE_POTION_VERSION "1.1.0"

    static const int VERSION_MAJOR = 11;
    static const int VERSION_MINOR = 2;
    static const int VERSION_REVISION = 0; 

    static const std::string VERSION = LOVE_VERSION;
    static const std::string CODENAME = "Mysterious Mysteries";
}

#if defined (_3DS)
    #define WINDOW_WIDTH 400
    #define WINDOW_HEIGHT 240
#elif defined (__SWITCH__)
    #define WINDOW_WIDTH 1280
    #define WINDOW_HEIGHT 720
#endif