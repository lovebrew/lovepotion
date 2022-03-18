#pragma once

#include "objects/object.h"

#include <limits>
#include <optional>
#include <vector>

namespace love::common
{
    class Gamepad : public Object
    {
      public:
        enum InputType
        {
            INPUT_TYPE_AXIS,
            INPUT_TYPE_BUTTON,
            INPUT_TYPE_MAX_ENUM
        };

        enum class GamepadAxis : uint64_t;

        enum class GamepadButton : uint64_t;

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

        typedef std::pair<const char*, int64_t> ButtonMapping;

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

        virtual ~Gamepad()
        {}

        virtual bool Open(size_t index) = 0;

        virtual void Close() = 0;

        virtual bool IsConnected() const = 0;

        virtual const char* GetName() const = 0;

        virtual size_t GetAxisCount() const = 0;

        virtual size_t GetButtonCount() const = 0;

        virtual float GetAxis(size_t axis) const = 0;

        virtual std::vector<float> GetAxes() const = 0;

        virtual bool IsDown(const std::vector<size_t>& buttons) const = 0;

        bool IsGamepad() const;

        virtual float GetGamepadAxis(GamepadAxis axis) const = 0;

        virtual bool IsGamepadDown(const std::vector<GamepadButton>& buttons) const = 0;

        std::string GetGUID() const;

        size_t GetInstanceID() const;

        size_t GetID() const;

        virtual bool IsVibrationSupported() = 0;

        virtual bool SetVibration(float left, float right, float duration = -1.0f) = 0;

        virtual bool SetVibration() = 0;

        virtual void GetVibration(float& left, float& right) = 0;

        /* helpers */

        virtual bool IsDown(size_t index, ButtonMapping& button) = 0;

        virtual bool IsHeld(size_t index, ButtonMapping& button) const = 0;

        virtual bool IsUp(size_t index, ButtonMapping& button) = 0;

        static bool GetConstant(const char* in, InputType& out);
        static bool GetConstant(InputType in, const char*& out);

        static bool GetConstant(const char* in, GamepadAxis& out);
        static bool GetConstant(GamepadAxis in, const char*& out);

        static bool GetConstant(const char* in, GamepadButton& out);
        static bool GetConstant(GamepadButton in, const char*& out);

        static float ClampValue(float x);

      protected:
        size_t id;
        int8_t instanceID;

        Vibration vibration;

        std::string name;
        std::string guid;

      private:
        Gamepad()
        {}
    };
} // namespace love::common
