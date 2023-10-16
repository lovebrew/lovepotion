#pragma once

#include <objects/joystick_ext.hpp>

#include <padscore/kpad.h>
#include <padscore/wpad.h>

namespace love
{
    class ProController : public Joystick<Console::CAFE>
    {
      public:
        ProController()
        {}

        ProController(int id);

        ProController(int id, int index);

        virtual ~ProController()
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
            return guid::GAMEPAD_TYPE_WII_PRO;
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

      private:
        KPADStatus state;
        Vibration<Console::CAFE> vibration;
    };
} // namespace love
