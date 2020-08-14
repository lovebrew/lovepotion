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

            void RemoveGamepad(Gamepad * gamepad);

            bool Split(Gamepad * gamepad);

            bool Merge(std::pair<Gamepad *, Gamepad *> gamepads);

            static bool IsConnected(u32 id);

        private:
            std::vector<Gamepad *> gamepads;
    };
}
