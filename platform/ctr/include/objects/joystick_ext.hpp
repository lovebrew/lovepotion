#pragma once

#include <objects/joystick/joystick.tcc>

#include <3ds.h>

namespace love
{
    template<>
    class Joystick<Console::CTR> : public Joystick<Console::ALL>
    {
      public:
        Joystick(int id);

        Joystick(int id, int index);

        virtual ~Joystick();

        bool Open(int index);

        void Close();

        bool IsConnected() const
        {
            return true;
        }

        bool IsDown(JoystickInput& result);

        bool IsUp(JoystickInput& result);

        void GetDeviceInfo(int& vendor, int& product, int& version);

        int GetAxisCount() const;

        int GetButtonCount() const;

        void Update();

        float GetAxis(int index);

        std::vector<float> GetAxes();

        bool IsDown(const std::vector<int>& buttons) const;

        void SetPlayerIndex(int index)
        {}

        int GetPlayerIndex() const
        {
            return 0;
        }

        bool IsGamepad() const
        {
            return true;
        }

        guid::GamepadType GetGamepadType() const
        {
            bool isN3DS = false;
            APT_CheckNew3DS(&isN3DS);

            if (isN3DS)
                return guid::GAMEPAD_TYPE_NEW_NINTENDO_3DS;

            return guid::GAMEPAD_TYPE_NINTENDO_3DS;
        }

        float GetGamepadAxis(GamepadAxis axis);

        bool IsGamepadDown(const std::vector<GamepadButton>& buttons) const;

        bool IsVibrationSupported()
        {
            return false;
        }

        bool SetVibration(float left, float right, float duration = -1.0f)
        {
            return false;
        }

        bool SetVibration()
        {
            return false;
        }

        void GetVibration(float& left, float& right) {};

      private:
        struct
        {
            uint32_t pressed;
            uint32_t released;
        } buttonStates;
    };
} // namespace love
