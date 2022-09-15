#pragma once

#include <objects/joystick/joystick.tcc>

#include <padscore/kpad.h>
#include <vpad/input.h>

namespace love
{
    template<>
    class Joystick<Console::CAFE> : public Joystick<Console::ALL>
    {
        Joystick(int id);

        Joystick(int id, int index);

        virtual ~Joystick();

        bool Open(int index);

        void Close();

        bool IsConnected() const;

        bool IsDown(JoystickInput& result);

        bool IsUp(JoystickInput& result);

        void GetDeviceInfo(int& vendor, int& product, int& version);

        int GetAxisCount() const;

        int GetButtonCount() const;

        void Update();

        float GetAxis(int index) const;

        std::vector<float> GetAxes() const;

        bool IsDown(const std::vector<int>& buttons) const;

        void SetPlayerIndex(int index);

        int GetPlayerIndex() const
        {
            return this->playerId;
        }

        bool IsGamepad() const
        {
            return true;
        }

        guid::GamepadType GetGamepadType() const;

        float GetGamepadAxis(GamepadAxis axis) const;

        bool IsGamepadDown(const std::vector<GamepadButton>& buttons) const;

        bool IsVibrationSupported()
        {
            return true;
        }

        bool SetVibration(float left, float right, float duration = -1.0f);

        bool SetVibration();

        void GetVibration(float& left, float& right);

      private:
        VPADStatus gamepad;
        KPADStatus kpad;

        int playerId;
    };
} // namespace love
