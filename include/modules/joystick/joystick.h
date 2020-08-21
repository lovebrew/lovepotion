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

            size_t GetJoystickCount() const;

            Gamepad * AddGamepad(size_t index);

            void RemoveGamepad(Gamepad * gamepad);

            bool Split(size_t id, const std::string & holdType = "default");

            bool Merge(const std::array<size_t, 2> & gamepads);

            static bool IsConnected(u32 id);

        private:
            std::vector<Gamepad *> gamepads;

            void ClearGamepads();

            std::string GetDeviceGUID(u32 index) const;
    };
}
