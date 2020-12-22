#pragma once

#include "objects/gamepad/gamepadc.h"
#include "common/enummap.h"

#include "common/bitalloc.h"

namespace love
{
    class Gamepad : public common::Gamepad
    {
        public:
            Gamepad(size_t id);

            Gamepad(size_t id, size_t index);

            ~Gamepad();

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

            std::string GetGUID() const override;

            size_t GetInstanceID() const override;

            size_t GetID() const override;

            bool IsVibrationSupported() override;

            bool SetVibration(float left, float right, float duration = -1.0f) override;

            bool SetVibration() override;

            void GetVibration(float & left, float & right) override;

            const Vibration & GetVibration() const;

            PadState & GetPadState();

            static bool GetConstant(int32_t in, GamepadButton & out);
            static bool GetConstant(GamepadButton in, int32_t & out);

        private:
            PadState pad;
            u32 style;

            std::unique_ptr<HidSixAxisSensorHandle[]> sixAxisHandles;

            std::unique_ptr<HidVibrationDeviceHandle[]> vibrationHandles;
            HidVibrationValue vibrationValues[2];

            const HidNpadStyleTag GetStyleTag() const;

            const HidNpadIdType GetNpadIdType() const;

            static EnumMap<GamepadButton, int32_t, GAMEPAD_BUTTON_MAX_ENUM>::Entry buttonEntries[];
            static EnumMap<GamepadButton, int32_t, GAMEPAD_BUTTON_MAX_ENUM> buttons;
    };
}