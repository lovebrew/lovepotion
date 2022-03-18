#pragma once

#include "objects/gamepad/gamepadc.h"

#include "common/bidirectionalmap.h"

enum class love::common::Gamepad::GamepadAxis : uint64_t
{
    GAMEPAD_AXIS_LEFTX        = KEY_CPAD_LEFT | KEY_CPAD_RIGHT,
    GAMEPAD_AXIS_LEFTY        = KEY_CPAD_UP | KEY_CPAD_DOWN,
    GAMEPAD_AXIS_RIGHTX       = KEY_CSTICK_LEFT | KEY_CSTICK_RIGHT,
    GAMEPAD_AXIS_RIGHTY       = KEY_CSTICK_UP | KEY_CSTICK_DOWN,
    GAMEPAD_AXIS_TRIGGERLEFT  = KEY_ZL,
    GAMEPAD_AXIS_TRIGGERRIGHT = KEY_ZR
};

enum class love::common::Gamepad::GamepadButton : uint64_t
{
    GAMEPAD_BUTTON_A              = KEY_A,
    GAMEPAD_BUTTON_B              = KEY_B,
    GAMEPAD_BUTTON_X              = KEY_X,
    GAMEPAD_BUTTON_Y              = KEY_Y,
    GAMEPAD_BUTTON_BACK           = KEY_SELECT,
    GAMEPAD_BUTTON_START          = KEY_START,
    GAMEPAD_BUTTON_LEFT_SHOULDER  = KEY_L,
    GAMEPAD_BUTTON_RIGHT_SHOULDER = KEY_R,
    GAMEPAD_BUTTON_DPAD_UP        = KEY_DUP,
    GAMEPAD_BUTTON_DPAD_RIGHT     = KEY_DRIGHT,
    GAMEPAD_BUTTON_DPAD_DOWN      = KEY_DDOWN,
    GAMEPAD_BUTTON_DPAD_LEFT      = KEY_DLEFT
};

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

        const char* GetName() const override;

        size_t GetAxisCount() const override;

        size_t GetButtonCount() const override;

        float GetAxis(size_t axis) const override;

        std::vector<float> GetAxes() const override;

        void Update();

        bool IsDown(size_t index, ButtonMapping& button) override;

        bool IsHeld(size_t index, ButtonMapping& button) const override;

        bool IsUp(size_t index, ButtonMapping& button) override;

        bool IsDown(const std::vector<size_t>& buttons) const override;

        float GetGamepadAxis(GamepadAxis axis) const override;

        bool IsGamepadDown(const std::vector<GamepadButton>& buttons) const override;

        bool IsVibrationSupported() override;

        bool SetVibration(float left, float right, float duration = -1.0f) override;

        bool SetVibration() override;

        void GetVibration(float& left, float& right) override;

        static bool GetConstant(const char* in, GamepadAxis& out);
        static bool GetConstant(GamepadAxis in, const char*& out);

        static bool GetConstant(const char* in, GamepadButton& out);
        static bool GetConstant(GamepadButton in, const char*& out);

        static constexpr uint8_t MAX_BUTTONS = 12;
        static constexpr uint8_t MAX_AXES    = 6;

        const static auto& GetButtonMapping()
        {
            return Gamepad::buttons;
        }

        struct
        {
            uint32_t pressed;
            uint32_t released;
        } buttonStates;

      private:
        // clang-format off
        static constexpr auto axes = BidirectionalMap<>::Create(
            "leftx",        Gamepad::GamepadAxis::GAMEPAD_AXIS_LEFTX,
            "lefty",        Gamepad::GamepadAxis::GAMEPAD_AXIS_LEFTY,
            "rightx",       Gamepad::GamepadAxis::GAMEPAD_AXIS_RIGHTX,
            "righty",       Gamepad::GamepadAxis::GAMEPAD_AXIS_RIGHTY,
            "triggerleft",  Gamepad::GamepadAxis::GAMEPAD_AXIS_TRIGGERLEFT,
            "triggerright", Gamepad::GamepadAxis::GAMEPAD_AXIS_TRIGGERRIGHT
        );

        static constexpr auto buttons = BidirectionalMap<>::Create(
            "a",             Gamepad::GamepadButton::GAMEPAD_BUTTON_A,
            "b",             Gamepad::GamepadButton::GAMEPAD_BUTTON_B,
            "x",             Gamepad::GamepadButton::GAMEPAD_BUTTON_X,
            "y",             Gamepad::GamepadButton::GAMEPAD_BUTTON_Y,
            "back",          Gamepad::GamepadButton::GAMEPAD_BUTTON_BACK,
            "start",         Gamepad::GamepadButton::GAMEPAD_BUTTON_START,
            "leftshoulder",  Gamepad::GamepadButton::GAMEPAD_BUTTON_LEFT_SHOULDER,
            "rightshoulder", Gamepad::GamepadButton::GAMEPAD_BUTTON_RIGHT_SHOULDER,
            "dpup",          Gamepad::GamepadButton::GAMEPAD_BUTTON_DPAD_UP,
            "dpdown",        Gamepad::GamepadButton::GAMEPAD_BUTTON_DPAD_DOWN,
            "dpleft",        Gamepad::GamepadButton::GAMEPAD_BUTTON_DPAD_LEFT,
            "dpright",       Gamepad::GamepadButton::GAMEPAD_BUTTON_DPAD_RIGHT
        );
        // clang-format on
    };
} // namespace love
