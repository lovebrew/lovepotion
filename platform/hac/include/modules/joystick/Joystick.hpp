#pragma once

#include "modules/joystick/Joystick.tcc"

#include <switch.h>

namespace love
{
    class Joystick : public JoystickBase
    {
      public:
        Joystick(int id);

        Joystick(int id, int index);

        ~Joystick();

        virtual bool open(int64_t deviceId) override;

        virtual void close() override;

        virtual bool isConnected() const override;

        virtual float getAxis(GamepadAxis axis) const override;

        virtual std::vector<float> getAxes() const override;

        virtual bool isDown(std::span<GamepadButton> buttons) const override;

        virtual void setPlayerIndex(int index) override;

        virtual int getPlayerIndex() const override;

        virtual JoystickInput getGamepadMapping(const GamepadInput& input) const override;

        virtual std::string getGamepadMappingString() const override;

        virtual bool isVibrationSupported() const override;

        virtual bool setVibration(float left, float right, float duration = -1.0f) override;

        virtual bool setVibration() override;

        virtual void getVibration(float& left, float& right) const override;

        virtual bool hasSensor(Sensor::SensorType type) const override;

        virtual bool isSensorEnabled(Sensor::SensorType type) const override;

        virtual void setSensorEnabled(Sensor::SensorType type, bool enable) override;

        virtual std::vector<float> getSensorData(Sensor::SensorType type) const override;

        // clang-format off
        ENUMMAP_DECLARE(HacButtonTypes, GamepadButton, HidNpadButton,
            { GAMEPAD_BUTTON_A,             HidNpadButton_A      },
            { GAMEPAD_BUTTON_B,             HidNpadButton_B      },
            { GAMEPAD_BUTTON_X,             HidNpadButton_X      },
            { GAMEPAD_BUTTON_Y,             HidNpadButton_Y      },

            { GAMEPAD_BUTTON_DPAD_LEFT,     HidNpadButton_Left   },
            { GAMEPAD_BUTTON_DPAD_UP,       HidNpadButton_Up     },
            { GAMEPAD_BUTTON_DPAD_RIGHT,    HidNpadButton_Right  },
            { GAMEPAD_BUTTON_DPAD_DOWN,     HidNpadButton_Down   },

            { GAMEPAD_BUTTON_START,         HidNpadButton_Plus   },
            { GAMEPAD_BUTTON_BACK,          HidNpadButton_Minus  },
            { GAMEPAD_BUTTON_LEFTSHOULDER,  HidNpadButton_L      },
            { GAMEPAD_BUTTON_RIGHTSHOULDER, HidNpadButton_R      },

            { GAMEPAD_BUTTON_LEFTSTICK,     HidNpadButton_StickL },
            { GAMEPAD_BUTTON_RIGHTSTICK,    HidNpadButton_StickR }
        );

        ENUMMAP_DECLARE(HacGamepadTypes, GamepadType, HidNpadStyleTag,
            { GAMEPAD_TYPE_JOYCON_LEFT,              HidNpadStyleTag_NpadJoyLeft  },
            { GAMEPAD_TYPE_JOYCON_RIGHT,             HidNpadStyleTag_NpadJoyRight },
            { GAMEPAD_TYPE_JOYCON_PAIR,              HidNpadStyleTag_NpadJoyDual  },
            { GAMEPAD_TYPE_NINTENDO_SWITCH_PRO,      HidNpadStyleTag_NpadFullKey  },
            { GAMEPAD_TYPE_NINTENDO_SWITCH_HANDHELD, HidNpadStyleTag_NpadHandheld }
        );
        // clang-format on

        using JoystickBase::getConstant;

        virtual void update()
        {
            padUpdate(&this->state);
        }

      private:
        HidNpadIdType getHandleCount(int& handles, HidNpadStyleTag& tag) const;

        PadState state;

        HidSixAxisSensorHandle accelerometer[0x02];
        HidSixAxisSensorHandle gyroscope[0x02];
    };
} // namespace love
