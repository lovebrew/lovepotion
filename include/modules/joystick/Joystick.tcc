#pragma once

#include "common/Map.hpp"
#include "common/Object.hpp"

#include "modules/sensor/Sensor.hpp"

#include "utility/guid.hpp"

#include <unordered_map>

namespace love
{
    class JoystickBase : public Object
    {
      public:
        static inline Type type = Type("Joystick", &Object::type);

        enum JoystickType
        {
            JOYSTICK_TYPE_UNKNOWN,
            JOYSTICK_TYPE_GAMEPAD,
            JOYSTICK_TYPE_MAX_ENUM
        };

        enum GamepadAxis
        {
            GAMEPAD_AXIS_INVALID = -1,
            GAMEPAD_AXIS_LEFTX,
            GAMEPAD_AXIS_LEFTY,
            GAMEPAD_AXIS_RIGHTX,
            GAMEPAD_AXIS_RIGHTY,
            GAMEPAD_AXIS_TRIGGERLEFT,
            GAMEPAD_AXIS_TRIGGERRIGHT,
            GAMEPAD_AXIS_MAX_ENUM
        };

        enum GamepadButton
        {
            GAMEPAD_BUTTON_INVALID = -1,
            GAMEPAD_BUTTON_A,
            GAMEPAD_BUTTON_B,
            GAMEPAD_BUTTON_X,
            GAMEPAD_BUTTON_Y,
            GAMEPAD_BUTTON_BACK,
            GAMEPAD_BUTTON_GUIDE,
            GAMEPAD_BUTTON_START,
            GAMEPAD_BUTTON_LEFTSTICK,
            GAMEPAD_BUTTON_RIGHTSTICK,
            GAMEPAD_BUTTON_LEFTSHOULDER,
            GAMEPAD_BUTTON_RIGHTSHOULDER,
            GAMEPAD_BUTTON_DPAD_UP,
            GAMEPAD_BUTTON_DPAD_DOWN,
            GAMEPAD_BUTTON_DPAD_LEFT,
            GAMEPAD_BUTTON_DPAD_RIGHT,
            GAMEPAD_BUTTON_MISC1,
            GAMEPAD_BUTTON_MISC2,
            GAMEPAD_BUTTON_Z, //< Nunchuk only
            GAMEPAD_BUTTON_C, //< Nunchuk only
            GAMEPAD_BUTTON_MAX_ENUM
        };

        enum InputType
        {
            INPUT_TYPE_AXIS,
            INPUT_TYPE_BUTTON,
            INPUT_TYPE_HAT,
            INPUT_TYPE_MAX_ENUM
        };

        // Represents a gamepad input value, e.g. the "x" button or the left trigger.
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

        virtual ~JoystickBase()
        {}

        virtual void update()
        {}

        std::string_view getName() const
        {
            return this->name;
        }

        JoystickType getJoystickType() const
        {
            return this->joystickType;
        }

        GamepadType getGamepadType() const
        {
            return this->gamepadType;
        }

        int getInstanceID() const
        {
            return this->instanceId;
        }

        int getID() const
        {
            return this->id;
        }

        std::string getGUID() const
        {
            return this->guid;
        }

        virtual bool isConnected() const = 0;

        int getAxisCount() const
        {
            return this->isConnected() ? getGamepadAxisCount(this->gamepadType) : 0;
        }

        int getButtonCount() const
        {
            return this->isConnected() ? getGamepadButtonCount(this->gamepadType) : 0;
        }

        int getHatCount() const
        {
            return this->isConnected() ? getGamepadHatCount(this->gamepadType) : 0;
        }

        bool isGamepad() const
        {
            return true;
        }

        void getDeviceInfo(int& vendorID, int& productID, int& productVersion)
        {
            getGamepadDeviceInfo(this->gamepadType, vendorID, productID, productVersion);
        }

        virtual bool open(int64_t deviceId) = 0;

        virtual void close() = 0;

        virtual float getAxis(GamepadAxis axis) const = 0;

        virtual std::vector<float> getAxes() const = 0;

        virtual bool isDown(std::span<GamepadButton> buttons) const = 0;

        virtual bool isHeld(std::span<GamepadButton> buttons) const = 0;

        virtual bool isUp(std::span<GamepadButton> buttons) const = 0;

        virtual bool isAxisChanged(GamepadAxis axis) const = 0;

        virtual void setPlayerIndex(int index) = 0;

        virtual int getPlayerIndex() const = 0;

        virtual JoystickInput getGamepadMapping(const GamepadInput& input) const = 0;

        virtual std::string getGamepadMappingString() const = 0;

        virtual bool isVibrationSupported() const = 0;

        virtual bool setVibration(float left, float right, float duration = -1.0f) = 0;

        virtual bool setVibration() = 0;

        virtual void getVibration(float& left, float& right) const = 0;

        virtual bool hasSensor(Sensor::SensorType type) const = 0;

        virtual bool isSensorEnabled(Sensor::SensorType type) const = 0;

        virtual void setSensorEnabled(Sensor::SensorType type, bool enable) = 0;

        virtual std::vector<float> getSensorData(Sensor::SensorType type) const = 0;

        // clang-format off
        STRINGMAP_DECLARE(GamepadButtons, GamepadButton,
            { "a",             GAMEPAD_BUTTON_A             },
            { "b",             GAMEPAD_BUTTON_B             },
            { "x",             GAMEPAD_BUTTON_X             },
            { "y",             GAMEPAD_BUTTON_Y             },
            { "back",          GAMEPAD_BUTTON_BACK          },
            { "guide",         GAMEPAD_BUTTON_GUIDE         },
            { "start",         GAMEPAD_BUTTON_START         },
            { "leftstick",     GAMEPAD_BUTTON_LEFTSTICK     },
            { "rightstick",    GAMEPAD_BUTTON_RIGHTSTICK    },
            { "leftshoulder",  GAMEPAD_BUTTON_LEFTSHOULDER  },
            { "rightshoulder", GAMEPAD_BUTTON_RIGHTSHOULDER },
            { "dpup",          GAMEPAD_BUTTON_DPAD_UP       },
            { "dpdown",        GAMEPAD_BUTTON_DPAD_DOWN     },
            { "dpleft",        GAMEPAD_BUTTON_DPAD_LEFT     },
            { "dpright",       GAMEPAD_BUTTON_DPAD_RIGHT    },
            { "misc1",         GAMEPAD_BUTTON_MISC1         },
            { "misc2",         GAMEPAD_BUTTON_MISC2         },
            { "z",             GAMEPAD_BUTTON_Z             }, //< Nunchuk only
            { "c",             GAMEPAD_BUTTON_C             }  //< Nunchuk only
        );

        STRINGMAP_DECLARE(GamepadAxes, GamepadAxis,
            { "leftx",        GAMEPAD_AXIS_LEFTX        },
            { "lefty",        GAMEPAD_AXIS_LEFTY        },
            { "rightx",       GAMEPAD_AXIS_RIGHTX       },
            { "righty",       GAMEPAD_AXIS_RIGHTY       },
            { "triggerleft",  GAMEPAD_AXIS_TRIGGERLEFT  },
            { "triggerright", GAMEPAD_AXIS_TRIGGERRIGHT }
        );

        STRINGMAP_DECLARE(InputTypes, InputType,
            { "axis",   INPUT_TYPE_AXIS   },
            { "button", INPUT_TYPE_BUTTON },
            { "hat",    INPUT_TYPE_HAT    }
        );

        STRINGMAP_DECLARE(GamepadTypes, GamepadType,
            { "unknown",           GAMEPAD_TYPE_UNKNOWN                     },
            { "3ds",               GAMEPAD_TYPE_NINTENDO_3DS                },
            { "3dsxl",             GAMEPAD_TYPE_NINTENDO_3DS_XL             },
            { "new3ds",            GAMEPAD_TYPE_NEW_NINTENDO_3DS            },
            { "new3dsxl",          GAMEPAD_TYPE_NEW_NINTENDO_3DS_XL         },
            { "2ds",               GAMEPAD_TYPE_NINTENDO_2DS                },
            { "new2dsxl",          GAMEPAD_TYPE_NEW_NINTENDO_2DS_XL         },
            { "wiiremote",         GAMEPAD_TYPE_NINTENDO_WII_REMOTE         },
            { "wiiremotenunchuk",  GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK },
            { "wiiclassic",        GAMEPAD_TYPE_NINTENDO_WII_CLASSIC        },
            { "wiiugamepad",       GAMEPAD_TYPE_NINTENDO_WII_U_GAMEPAD      },
            { "wiiupro",           GAMEPAD_TYPE_NINTENDO_WII_U_PRO          },
            { "switchhandheld",    GAMEPAD_TYPE_NINTENDO_SWITCH_HANDHELD    },
            { "switchpro",         GAMEPAD_TYPE_NINTENDO_SWITCH_PRO         },
            { "joyconleft",        GAMEPAD_TYPE_JOYCON_LEFT                 },
            { "joyconright",       GAMEPAD_TYPE_JOYCON_RIGHT                },
            { "joyconpair",        GAMEPAD_TYPE_JOYCON_PAIR                 }
        );

        STRINGMAP_DECLARE(JoystickTypes, JoystickType,
            { "unknown", JOYSTICK_TYPE_UNKNOWN },
            { "gamepad", JOYSTICK_TYPE_GAMEPAD }
        );
        // clang-format on

      protected:
#if defined(__3DS__)
        static constexpr float MAX_AXIS_VALUE = 150.0f;
#elif defined(__SWITCH__)
        static constexpr float MAX_AXIS_VALUE = 32768.0f;
#else // Wii U
        static constexpr float MAX_AXIS_VALUE = 1.0f;
#endif

        static float clamp(const float value)
        {
            return std::clamp(value / MAX_AXIS_VALUE, -1.0f, 1.0f);
        }

        JoystickBase(int id) : joystickType(JOYSTICK_TYPE_UNKNOWN), instanceId(-1), id(id), sensors()
        {
            this->sensors[Sensor::SENSOR_ACCELEROMETER] = false;
            this->sensors[Sensor::SENSOR_GYROSCOPE]     = false;
        }

        JoystickBase(int id, int) : instanceId(-1), id(id), sensors()
        {
            this->sensors[Sensor::SENSOR_ACCELEROMETER] = false;
            this->sensors[Sensor::SENSOR_GYROSCOPE]     = false;
        }

        JoystickType joystickType;
        GamepadType gamepadType;

        int instanceId;
        std::string guid;
        int id;

        std::string_view name;

      protected:
        std::unordered_map<Sensor::SensorType, bool> sensors;
    };
} // namespace love
