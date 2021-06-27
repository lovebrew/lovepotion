#pragma once

#include "objects/gamepad/gamepadc.h"

#include "common/bitalloc.h"

#include <memory>
#include <switch.h>

enum class love::common::Gamepad::GamepadAxis : uint64_t
{
    GAMEPAD_AXIS_LEFTX        = HidNpadButton_StickLLeft | HidNpadButton_StickLRight,
    GAMEPAD_AXIS_LEFTY        = HidNpadButton_StickLUp | HidNpadButton_StickLDown,
    GAMEPAD_AXIS_RIGHTX       = HidNpadButton_StickRLeft | HidNpadButton_StickRRight,
    GAMEPAD_AXIS_RIGHTY       = HidNpadButton_StickRUp | HidNpadButton_StickRDown,
    GAMEPAD_AXIS_TRIGGERLEFT  = HidNpadButton_ZL,
    GAMEPAD_AXIS_TRIGGERRIGHT = HidNpadButton_ZR,
};

enum class love::common::Gamepad::GamepadButton : uint64_t
{
    GAMEPAD_BUTTON_A              = HidNpadButton_A,
    GAMEPAD_BUTTON_B              = HidNpadButton_B,
    GAMEPAD_BUTTON_X              = HidNpadButton_X,
    GAMEPAD_BUTTON_Y              = HidNpadButton_Y,
    GAMEPAD_BUTTON_BACK           = HidNpadButton_Minus,
    GAMEPAD_BUTTON_START          = HidNpadButton_Plus,
    GAMEPAD_BUTTON_LEFTSTICK      = HidNpadButton_StickL,
    GAMEPAD_BUTTON_RIGHTSTICK     = HidNpadButton_StickR,
    GAMEPAD_BUTTON_LEFT_SHOULDER  = HidNpadButton_L,
    GAMEPAD_BUTTON_RIGHT_SHOULDER = HidNpadButton_R,
    GAMEPAD_BUTTON_DPAD_UP        = HidNpadButton_Up,
    GAMEPAD_BUTTON_DPAD_RIGHT     = HidNpadButton_Right,
    GAMEPAD_BUTTON_DPAD_DOWN      = HidNpadButton_Down,
    GAMEPAD_BUTTON_DPAD_LEFT      = HidNpadButton_Left
};

namespace love
{
    class Gamepad : public common::Gamepad
    {
      public:
        Gamepad(size_t id);

        Gamepad(size_t id, size_t index);

        virtual ~Gamepad();

        bool Open(size_t id) override;

        void Close() override;

        bool IsConnected() const override;

        const char* GetName() const override;

        size_t GetAxisCount() const override;

        size_t GetButtonCount() const override;

        float GetAxis(size_t axis) const override;

        std::vector<float> GetAxes() const override;

        bool IsDown(const std::vector<size_t>& buttons) const override;

        float GetGamepadAxis(GamepadAxis axis) const override;

        bool IsGamepadDown(const std::vector<GamepadButton>& buttons) const override;

        bool IsVibrationSupported() override;

        bool SetVibration(float left, float right, float duration = -1.0f) override;

        bool SetVibration() override;

        void GetVibration(float& left, float& right) override;

        const Vibration& GetVibration() const;

        void UpdatePadState();

        bool IsDown(size_t index, ButtonMapping& mapping) override;

        bool IsHeld(size_t index, ButtonMapping& mapping) const override;

        bool IsUp(size_t index, ButtonMapping& mapping) override;

        static constexpr uint8_t GAMEPAD_MAX_BUTTONS = 15;

        static bool GetConstant(const char* in, GamepadAxis& out);
        static bool GetConstant(GamepadAxis in, const char*& out);

        static bool GetConstant(const char* in, GamepadButton& out);
        static bool GetConstant(GamepadButton in, const char*& out);

        static constexpr uint8_t MAX_BUTTONS = 15;
        static constexpr uint8_t MAX_AXES    = 6;

        const static StringMap<GamepadButton, MAX_BUTTONS> GetButtonMapping()
        {
            return Gamepad::buttons;
        }

      private:
        PadState pad;
        u32 style;

        std::unique_ptr<HidSixAxisSensorHandle[]> sixAxisHandles;

        std::unique_ptr<HidVibrationDeviceHandle[]> vibrationHandles;
        HidVibrationValue vibrationValues[2];

        HidNpadStyleTag GetStyleTag();

        HidNpadIdType GetNpadIdType();

        const static StringMap<GamepadAxis, MAX_AXES> axes;
        const static StringMap<GamepadButton, MAX_BUTTONS> buttons;

        struct
        {
            uint64_t pressed;
            uint64_t released;
        } buttonStates;
    };
} // namespace love
