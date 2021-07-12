#pragma once

namespace Version
{
    static constexpr const char* LOVE_POTION = "2.2.0";
    static constexpr const char* LOVE        = "11.3.0";

    static constexpr int MAJOR    = 11;
    static constexpr int MINOR    = 3;
    static constexpr int REVISION = 0;

    static constexpr int POTION_MAJOR    = 2;
    static constexpr int POTION_MINOR    = 1;
    static constexpr int POTION_REVISION = 0;

    static constexpr const char* CODENAME = "Mysterious Mysteries";

    static constexpr const char* COMPATABILITY[] = { LOVE_POTION, "2.1.2", "2.1.1", "2.1.0", 0 };
} // namespace Version
