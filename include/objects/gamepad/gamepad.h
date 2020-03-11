#pragma once

#include "objects/object.h"
#include "common/backend/input.h"

namespace love
{
    class Gamepad : public Object
    {
        public:
            Gamepad(size_t id);

            static inline love::Type type = love::Type("Joystick", &Object::type);

            float GetAxis(size_t axis);

            size_t GetAxisCount();

            size_t GetButtonCount();

            float GetGamepadAxis(const std::string & axis);

            size_t GetID();

            std::string GetName();

            std::pair<float, float> GetVibration();

            bool IsConnected();

            bool IsDown(size_t button);

            bool IsGamepad();

            bool IsGamepadDown(const std::string & button);

            bool IsVibrationSupported();

            bool SetVibration(float left, float right, float duration = 0.0f);

            virtual ~Gamepad() {}

        private:
            size_t id;
            StickPosition stick;

            std::pair<float, float> vibrations;
            float vibrationDuration;

            u32 vibrationHandles[2][2];
            VibrationValue vibrationValues[2];
    };
}
