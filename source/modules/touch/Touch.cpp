#include "modules/touch/Touch.hpp"

#include <algorithm>

namespace love
{
    Touch::Touch() : Module(Module::M_TOUCH, "love.touch")
    {}

    const std::vector<Finger>& Touch::getTouches()
    {
        return this->touches;
    }

    const Finger& Touch::getTouch(int64_t id)
    {
        for (const auto& touch : this->touches)
        {
            if (touch.id == id)
                return touch;
        }

        throw love::Exception("Invalid active touch ID: {:d}", id);
    }

    void Touch::onEvent(SubEventType type, const Finger& info)
    {
        auto compare = [&info](const Finger& touch) -> bool { return touch.id == info.id; };

        switch (type)
        {
            case SUBTYPE_TOUCHPRESS:
            {
                auto it = std::remove_if(this->touches.begin(), this->touches.end(), compare);
                this->touches.erase(it, this->touches.end());

                this->touches.push_back(info);

                break;
            }
            case SUBTYPE_TOUCHMOVED:
            {
                for (auto& touch : this->touches)
                {
                    if (touch.id == info.id)
                        touch = info;
                }
                break;
            }
            case SUBTYPE_TOUCHRELEASE:
            {
                auto it = std::remove_if(this->touches.begin(), this->touches.end(), compare);
                this->touches.erase(it, this->touches.end());

                break;
            }
            default:
                break;
        }
    }
} // namespace love
