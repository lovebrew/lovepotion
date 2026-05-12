#include "modules/joystick/JoystickModule.hpp"

#include "modules/joystick/Joystick.hpp"

namespace love::joystick
{
    void init()
    {}

    int getJoystickCount()
    {
        return 1;
    }

    JoystickBase* openJoystick(int index)
    {
        return new Joystick(index);
    }
} // namespace love::joystick
