#pragma once

#include "objects/gamepad/gamepadc.h"
#include "common/enummap.h"

namespace love
{
    class Gamepad : public common::Gamepad
    {
        public:
            static constexpr float JOYSTICK_MAX = 150.0f;

            Gamepad(size_t id);

            Gamepad(size_t id, size_t index);

            virtual ~Gamepad();

            bool Open(size_t id) override;

            void Close() override;

            bool IsConnected() const override;

            const char * GetName() const override;

            size_t GetAxisCount() const override;

            size_t GetButtonCount() const override;

            float GetAxis(size_t axis) const override;

            std::vector<float> GetAxes() const override;

            bool IsDown(const std::vector<size_t> & buttons) const override;

            float GetGamepadAxis(GamepadAxis axis) const override;

            bool IsGamepadDown(const std::vector<GamepadButton> & buttons) const override;

            bool IsVibrationSupported() override;

            bool SetVibration(float left, float right, float duration = -1.0f) override;

            bool SetVibration() override;

            void GetVibration(float & left, float & right) override;

            static bool GetConstant(int32_t in, GamepadButton & out);
            static bool GetConstant(GamepadButton in, int32_t & out);

        private:
            static EnumMap<GamepadButton, int32_t, GAMEPAD_BUTTON_MAX_ENUM>::Entry buttonEntries[];
            static EnumMap<GamepadButton, int32_t, GAMEPAD_BUTTON_MAX_ENUM> buttons;
    };
}