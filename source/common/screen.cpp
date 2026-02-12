#include "common/screen.hpp"
#include "common/Exception.hpp"

namespace love
{

    const ScreenInfo& getScreenInfo(Screen id)
    {
        const auto info = love::getScreenInfo();

        if (info.empty())
            throw love::Exception("No screens available.");

        const auto index = (Screen)std::clamp<size_t>(id, 0, info.size() - 1);

        return info[index];
    }

    Screen getScreenId(const std::string& name)
    {
        const auto info = love::getScreenInfo();

        for (size_t i = 0; i < info.size(); ++i)
        {
            if (info[i].name == name)
                return Screen(i);
        }

        return INVALID_SCREEN;
    }
} // namespace love
