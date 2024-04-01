#pragma once

#include "modules/joystick/Joystick.tcc"

#include <padscore/kpad.h>
#include <vpad/input.h>

namespace love
{
    class Joystick : public JoystickBase
    {
      public:
        Joystick(int id);

        Joystick(int id, int index);

        ~Joystick();

        virtual void update();

        bool isDRCGamepad() const
        {
            return this->gamepadType == GAMEPAD_TYPE_WII_U_GAMEPAD;
        }

        VPADStatus& getVPADStatus()
        {
            return this->vpadStatus;
        }

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

        using JoystickBase::getConstant;

        // clang-format off
        ENUMMAP_DECLARE(WpadProButtons, GamepadButton, WPADProButton,
            { GAMEPAD_BUTTON_A,             WPAD_PRO_BUTTON_A       },
            { GAMEPAD_BUTTON_B,             WPAD_PRO_BUTTON_B       },
            { GAMEPAD_BUTTON_X,             WPAD_PRO_BUTTON_X       },
            { GAMEPAD_BUTTON_Y,             WPAD_PRO_BUTTON_Y       },

            { GAMEPAD_BUTTON_DPAD_LEFT,     WPAD_PRO_BUTTON_LEFT    },
            { GAMEPAD_BUTTON_DPAD_UP,       WPAD_PRO_BUTTON_UP      },
            { GAMEPAD_BUTTON_DPAD_RIGHT,    WPAD_PRO_BUTTON_RIGHT   },
            { GAMEPAD_BUTTON_DPAD_DOWN,     WPAD_PRO_BUTTON_DOWN    },

            { GAMEPAD_BUTTON_START,         WPAD_PRO_BUTTON_PLUS    },
            { GAMEPAD_BUTTON_BACK,          WPAD_PRO_BUTTON_MINUS   },
            { GAMEPAD_BUTTON_LEFTSHOULDER,  WPAD_PRO_TRIGGER_L      },
            { GAMEPAD_BUTTON_RIGHTSHOULDER, WPAD_PRO_TRIGGER_R      },

            { GAMEPAD_BUTTON_LEFTSTICK,     WPAD_PRO_BUTTON_STICK_L },
            { GAMEPAD_BUTTON_RIGHTSTICK,    WPAD_PRO_BUTTON_STICK_R }
        );

        ENUMMAP_DECLARE(VpadButtons, GamepadButton, VPADButtons,
            { GAMEPAD_BUTTON_A,             VPAD_BUTTON_A       },
            { GAMEPAD_BUTTON_B,             VPAD_BUTTON_B       },
            { GAMEPAD_BUTTON_X,             VPAD_BUTTON_X       },
            { GAMEPAD_BUTTON_Y,             VPAD_BUTTON_Y       },

            { GAMEPAD_BUTTON_DPAD_LEFT,     VPAD_BUTTON_LEFT    },
            { GAMEPAD_BUTTON_DPAD_UP,       VPAD_BUTTON_UP      },
            { GAMEPAD_BUTTON_DPAD_RIGHT,    VPAD_BUTTON_RIGHT   },
            { GAMEPAD_BUTTON_DPAD_DOWN,     VPAD_BUTTON_DOWN    },

            { GAMEPAD_BUTTON_START,         VPAD_BUTTON_PLUS    },
            { GAMEPAD_BUTTON_BACK,          VPAD_BUTTON_MINUS   },
            { GAMEPAD_BUTTON_LEFTSHOULDER,  VPAD_BUTTON_L       },
            { GAMEPAD_BUTTON_RIGHTSHOULDER, VPAD_BUTTON_R       },

            { GAMEPAD_BUTTON_LEFTSTICK,     VPAD_BUTTON_STICK_L },
            { GAMEPAD_BUTTON_RIGHTSTICK,    VPAD_BUTTON_STICK_R }
        );

        ENUMMAP_DECLARE(WpadTypes, GamepadType, WPADExtensionType,
            { GAMEPAD_TYPE_WII_U_PRO, WPAD_EXT_PRO_CONTROLLER }
        );
        // clang-format on

      private:
        VPADStatus vpadStatus;
        VPADReadError vpadError;

        KPADStatus kpadStatus;
        KPADError kpadError;

        float getVPADAxis(GamepadAxis axis) const;

        bool isVPADButtonDown(std::span<GamepadButton> buttons) const;

        float getKPADAxis(GamepadAxis axis) const;

        bool isKPADButtonDown(std::span<GamepadButton> buttons) const;
    };
} // namespace love
