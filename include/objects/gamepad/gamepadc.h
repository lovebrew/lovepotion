#pragma once

#include "objects/object.h"
#include "common/stringmap.h"

namespace love::common
{
    class Gamepad : public Object
    {
        public:
            enum GamepadAxis
            {
                GAMEPAD_AXIS_INVALID,
                GAMEPAD_AXIS_LEFTX,
                GAMEPAD_AXIS_LEFTY,
                GAMEPAD_AXIS_RIGHTX,
                GAMEPAD_AXIS_RIGHTY,
                GAMEPAD_AXIS_TRIGGERLEFT, //< zl button
                GAMEPAD_AXIS_TRIGGERRIGHT, //< zr button
                GAMEPAD_AXIS_MAX_ENUM
            };

            enum GamepadButton
            {
                GAMEPAD_BUTTON_INVALID,

                GAMEPAD_BUTTON_A,
                GAMEPAD_BUTTON_B,
                GAMEPAD_BUTTON_X,
                GAMEPAD_BUTTON_Y,

                GAMEPAD_BUTTON_BACK, //< select/minus
                GAMEPAD_BUTTON_GUIDE, //< home
                GAMEPAD_BUTTON_START, //< start/plus

                GAMEPAD_BUTTON_LEFTSTICK,
                GAMEPAD_BUTTON_RIGHTSTICK,

                GAMEPAD_BUTTON_LEFTSHOULDER, //< l button
                GAMEPAD_BUTTON_RIGHTSHOULDER, //< r button

                GAMEPAD_BUTTON_DPAD_UP,
                GAMEPAD_BUTTON_DPAD_DOWN,
                GAMEPAD_BUTTON_DPAD_LEFT,
                GAMEPAD_BUTTON_DPAD_RIGHT,


                GAMEPAD_BUTTON_MAX_ENUM
            };

            enum InputType
            {
                INPUT_TYPE_AXIS,
                INPUT_TYPE_BUTTON,
                INPUT_TYPE_MAX_ENUM
            };

            struct GamepadInput
            {
                InputType type;

                union
                {
                    GamepadAxis axis;
                    GamepadButton button;
                };
            };

            struct JoystickInput
            {
                InputType type;

                union
                {
                    int axis;
                    int button;
                };
            };

            struct Vibration
            {
                float left  = 0.0f;
                float right = 0.0f;

                float endTime = 0.0f;

                int id = -1;

                static constexpr uint32_t max = std::numeric_limits<uint32_t>::max();
            };

            static love::Type type;

            Gamepad(size_t id);

            Gamepad(size_t id, size_t index);

            virtual ~Gamepad() {}

            virtual bool Open(size_t index) = 0;

            virtual void Close() = 0;

            virtual bool IsConnected() const = 0;

            virtual const char * GetName() const = 0;

            virtual size_t GetAxisCount() const = 0;

            virtual size_t GetButtonCount() const = 0;

            virtual float GetAxis(size_t axis) const = 0;

            virtual std::vector<float> GetAxes() const = 0;

            virtual bool IsDown(const std::vector<size_t> & buttons) const = 0;

            bool IsGamepad() const;

            virtual float GetGamepadAxis(GamepadAxis axis) const = 0;

            virtual bool IsGamepadDown(const std::vector<GamepadButton> & buttons) const = 0;

            std::string GetGUID() const;

            size_t GetInstanceID() const;

            size_t GetID() const;

            virtual bool IsVibrationSupported() = 0;

            virtual bool SetVibration(float left, float right, float duration = -1.0f) = 0;

            virtual bool SetVibration() = 0;

            virtual void GetVibration(float & left, float & right) = 0;

            static bool GetConstant(const char * in, GamepadAxis & out);
            static bool GetConstant(GamepadAxis in, const char *& out);

            static bool GetConstant(const char * in, GamepadButton & out);
            static bool GetConstant(GamepadButton in, const char *& out);

            static bool GetConstant(const char * in, InputType & out);
            static bool GetConstant(InputType in, const char *& out);

            static float ClampValue(float x);

        protected:
            size_t id;
            int8_t instanceID;

            Vibration vibration;

            std::string name;
            std::string guid;

        private:
            Gamepad() {}

            static StringMap<GamepadAxis, GAMEPAD_AXIS_MAX_ENUM>::Entry axisEntries[];
            static StringMap<GamepadAxis, GAMEPAD_AXIS_MAX_ENUM> axes;

            static StringMap<GamepadButton, GAMEPAD_BUTTON_MAX_ENUM>::Entry buttonEntries[];
            static StringMap<GamepadButton, GAMEPAD_BUTTON_MAX_ENUM> buttons;

            static StringMap<InputType, INPUT_TYPE_MAX_ENUM>::Entry inputTypeEntries[];
            static StringMap<InputType, INPUT_TYPE_MAX_ENUM> inputTypes;
    };
}
