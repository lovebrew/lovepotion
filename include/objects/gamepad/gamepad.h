#pragma once

#if defined (_3DS)
    inline constexpr float JOYSTICK_MAX = 150.0f;
    typedef circlePosition StickPosition;
#elif defined (__SWITCH__)
    typedef JoystickPosition StickPosition;
#endif

#include "objects/object.h"
#include "common/backend/input.h"
#include "objects/gamepad/handles/handles.h"

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

            LOVE_Vibration GetVibration();

            bool IsConnected();

            bool IsDown(size_t button);

            bool IsGamepad();

            bool IsGamepadDown(const std::string & button);

            bool IsVibrationSupported();

            void SetVibrationValues(const LOVE_Vibration & vibration);

            float GetVibrationDuration() const;

            bool SyncVibration(float duration);

            virtual ~Gamepad() {}

        private:
            size_t id;
            StickPosition stick;

            float duration;

            LOVE_Vibration vibration;
            std::unique_ptr<gamepad::Handles> handles;
    };
}
