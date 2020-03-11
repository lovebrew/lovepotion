#pragma once

#include "objects/gamepad/gamepad.h"
#include "objects/gamepad/wrap_gamepad.h"

namespace love
{
    class Joystick : public Module
    {
        public:
            Joystick();
            ~Joystick();

            ModuleType GetModuleType() const { return M_JOYSTICK; }

            const char * GetName() const override { return "love.joystick"; }

            Gamepad * GetJoystickFromID(size_t index);

            size_t GetJoystickCount();

            Gamepad * AddGamepad(size_t index);

        private:
            std::vector<Gamepad *> gamepads;
    };
}
