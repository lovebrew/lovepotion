#pragma once

#include <modules/joystick/joystickmodule.tcc>

#include <switch.h>
#include <vector>

namespace love
{
    template<>
    class JoystickModule<Console::HAC> : public JoystickModule<Console::ALL>
    {
      public:
        JoystickModule();

        virtual ~JoystickModule();

        /* out pointer to whatever was added/removed */
        int GetCurrentJoystickCount(std::vector<HidNpadIdType>* out = nullptr);

        std::vector<HidNpadIdType> GetActiveIds() const
        {
            return this->activeIds;
        }

        ::Joystick* AddJoystick(int index);

      private:
        static constexpr size_t MAX_JOYSTICKS = 0x08;
        std::vector<HidNpadIdType> activeIds;
    };
} // namespace love
