#pragma once

#include <objects/joystick/joystick.tcc>

#include <padscore/kpad.h>
#include <vpad/input.h>

#include <utilities/haptics/sixaxis_ext.hpp>
#include <utilities/haptics/vibration_ext.hpp>

using SixAxis   = love::SixAxis<love::Console::Which>;
using Vibration = love::Vibration<love::Console::Which>;

namespace love
{
    template<>
    class Joystick<Console::CAFE> : public Joystick<Console::ALL>
    {
      public:
        struct StickAxis
        {
            float x;
            float y;
        };

        Joystick(int id);

        Joystick(int id, int index);

        virtual ~Joystick();

        bool Open(int index);

        void Close();

        bool IsConnected() const;

        void Update();

        bool IsDown(JoystickInput& result);

        bool IsUp(JoystickInput& result);

        void GetDeviceInfo(int& vendor, int& product, int& version);

        int GetAxisCount() const;

        int GetButtonCount() const;

        float GetAxis(int index);

        std::vector<float> GetAxes();

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

        float GetGamepadAxis(GamepadAxis axis);

        bool IsGamepadDown(const std::vector<GamepadButton>& buttons) const;

        bool IsVibrationSupported()
        {
            return true;
        }

        bool SetVibration(float left, float right, float duration = -1.0f);

        bool SetVibration();

        void GetVibration(float& left, float& right);

        VPADTouchData GetTouchData() const;

      private:
        VPADStatus vpad;
        KPADStatus kpad;

        int playerId;
        bool isGamepad;

        WPADExtensionType extension;

        struct
        {
            int32_t pressed;
            int32_t released;
            int32_t held;

            struct
            {
                int32_t pressed;
                int32_t released;
                int32_t held;
            } extension;

            StickAxis leftStick;
            StickAxis rightStick;
        } buttonStates;

        int32_t leftTrigger;
        int32_t rightTrigger;

        ::SixAxis sixAxis;
        ::SixAxis sixAxisExt;

        ::Vibration vibration;
    };
} // namespace love
