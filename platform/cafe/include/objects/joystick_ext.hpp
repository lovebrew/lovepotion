#pragma once

#include <objects/joystick/joystick.tcc>

#include <padscore/kpad.h>
#include <vpad/input.h>

namespace love
{
    template<>
    class Joystick<Console::CAFE> : public Joystick<Console::ALL>
    {
      public:
        Joystick(int id);

        Joystick(int id, int index);

        virtual ~Joystick();

        bool Open(int index);

        void Close();

        bool IsConnected() const;

        void Update(const VPADStatus& status);

        bool IsDown(JoystickInput& result);

        bool IsUp(JoystickInput& result);

        void GetDeviceInfo(int& vendor, int& product, int& version);

        int GetAxisCount() const;

        int GetButtonCount() const;

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

        /* VPAD */
        static bool GetConstant(GamepadButton in, VPADButtons& out);
        static bool GetConstant(VPADButtons in, GamepadButton& out);

        static bool GetConstant(GamepadAxis in, int32_t& out);
        static bool GetConstant(int32_t in, GamepadAxis& out);

        /* WPAD */
        static bool GetConstant(GamepadButton in, WPADButton& out);
        static bool GetConstant(WPADButton in, GamepadButton& out);

        /* NUNCHUCK */
        static bool GetConstant(GamepadButton in, WPADNunchukButton& out);
        static bool GetConstant(WPADNunchukButton in, GamepadButton& out);

      private:
        KPADStatus kpad;

        int playerId;
        bool isGamepad;

        WPADExtensionType extension;

        struct StickAxis
        {
            float x;
            float y;
        };

        struct
        {
            uint32_t pressed;
            uint32_t released;

            struct
            {
                uint32_t pressed;
                uint32_t released;
            } extension;

            StickAxis leftStick;
            StickAxis rightStick;

            int32_t leftTrigger;
            int32_t rightTrigger;
        } buttonStates;
    };
} // namespace love
