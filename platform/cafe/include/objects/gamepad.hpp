#pragma once

#include <objects/joystick_ext.hpp>

#include <vpad/input.h>

namespace love
{
    class Gamepad : public Joystick<Console::CAFE>
    {
      public:
        Gamepad()
        {}

        Gamepad(int id);

        Gamepad(int id, int index);

        virtual ~Gamepad()
        {}

        bool Open(int index) override;

        bool IsConnected() const
        {
            return true;
        }

        virtual bool IsDown(JoystickInput& result) override;

        virtual bool IsDown(const std::vector<int>& buttons) const override;

        virtual bool IsGamepadDown(const std::vector<GamepadButton>& buttons) const override;

        virtual bool IsAxisChanged(GamepadAxis axis) override;

        virtual bool IsUp(JoystickInput& result) override;

        virtual float GetAxis(int index) override;

        virtual float GetGamepadAxis(GamepadAxis axis) override;

        virtual std::vector<float> GetAxes() override;

        virtual void Update() override;

        bool IsGamepad() const
        {
            return true;
        }

        guid::GamepadType GetGamepadType() const override
        {
            return guid::GAMEPAD_TYPE_WII_U_GAMEPAD;
        }

        bool IsVibrationSupported() override
        {
            return true;
        }

        bool SetVibration(float left, float right, float duration = -1.0f) override;

        bool SetVibration() override;

        void GetVibration(float& left, float& right) override;

        bool HasSensor(Sensor::SensorType type) const override;

        bool IsSensorEnabled(Sensor::SensorType type) override;

        void SetSensorEnabled(Sensor::SensorType type, bool enabled) override;

        std::vector<float> GetSensorData(Sensor::SensorType type) override;

        /* Wii U Gamepad Specifics */

        VPADStatus GetVPADStatus() const
        {
            return this->state;
        }

      private:
        VPADStatus state;
        Vibration<Console::CAFE> vibration;
    };
} // namespace love
