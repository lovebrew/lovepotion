#include <modules/touch/touch.hpp>

#include <algorithm>

using namespace love;

const std::vector<Finger>& Touch::GetTouches() const
{
    return this->touches;
}

const Finger& Touch::GetTouch(int64_t id) const
{
    for (const auto& touch : this->touches)
    {
        if (touch.id == id)
            return touch;
    }

    throw love::Exception("Invalid active touch ID: %d.", id);
}

static std::vector<Finger>::iterator onCompare(std::vector<Finger>)
{}

void Touch::OnEvent(SubEventType type, const Finger& info)
{
    auto compare = [&](const Finger& touch) -> bool { return touch.id == info.id; };

    switch (type)
    {
        case SUBTYPE_TOUCHPRESS:
        {
            const auto iter = std::remove_if(this->touches.begin(), this->touches.end(), compare);

            this->touches.erase(iter, this->touches.end());
            this->touches.push_back(info);

            break;
        }
        case SUBTYPE_TOUCHMOVED:
        {
            for (Finger& touch : this->touches)
            {
                if (touch.id == info.id)
                    touch = info;
            }

            break;
        }
        case SUBTYPE_TOUCHRELEASE:
        {
            const auto iter = std::remove_if(this->touches.begin(), this->touches.end(), compare);
            this->touches.erase(iter, this->touches.end());

            break;
        }
        default:
            break;
    }
}
