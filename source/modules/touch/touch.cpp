#include "common/runtime.h"
#include "modules/touch/touch.h"

#include "common/exception.h"
#include "modules/event/event.h"

using namespace love;

const std::vector<Touch::TouchInfo> & Touch::GetTouches() const
{
    return this->touches;
}

const Touch::TouchInfo & Touch::GetTouch(int64_t id) const
{
    for (const auto & touch : this->touches)
    {
        if (touch.id == id)
            return touch;
    }

    throw love::Exception("Invalid active touch ID: %d.", id);
}

void Touch::OnEvent(int type, const Touch::TouchInfo & info)
{
    auto compare = [&](const TouchInfo & touch) -> bool
    {
        return touch.id == info.id;
    };

    switch (type)
    {
        case LOVE_TOUCHPRESS:
        {
            this->touches.erase(std::remove_if(this->touches.begin(), this->touches.end(), compare), this->touches.end());
            this->touches.push_back(info);
            break;
        }
        case LOVE_TOUCHMOVED:
        {
            for (TouchInfo & touch : this->touches)
            {
                if (touch.id == info.id)
                    touch = info;
            }
            break;
        }
        case LOVE_TOUCHRELEASE:
        {
            this->touches.erase(std::remove_if(this->touches.begin(), this->touches.end(), compare), this->touches.end());
            break;
        }
        default:
            break;
    }
}
