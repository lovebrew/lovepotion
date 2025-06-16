#pragma once

#include "modules/joystick/Joystick.tcc"

#include <vpad/input.h>

namespace love
{
    namespace vpad
    {
        class Joystick : public JoystickBase
        {
          public:
            Joystick(int id);

            Joystick(int id, int index);

            ~Joystick();

            virtual void update();

            VPADStatus& getVPADStatus()
            {
                return this->status;
            }

            virtual bool open(int64_t deviceId) override;

            virtual void close() override;

            virtual bool isConnected() const override;

            virtual float getAxis(GamepadAxis axis) const override;

            virtual std::vector<float> getAxes() const override;

            virtual bool isDown(std::span<GamepadButton> buttons) const override;

            virtual bool isHeld(std::span<GamepadButton> buttons) const override;

            virtual bool isUp(std::span<GamepadButton> buttons) const override;

            virtual bool isAxisChanged(GamepadAxis axis) const override;

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

            enum VPADAxis
            {
                VPADAXIS_LEFTX        = VPAD_STICK_L_EMULATION_LEFT | VPAD_STICK_L_EMULATION_RIGHT,
                VPADAXIS_LEFTY        = VPAD_STICK_L_EMULATION_UP | VPAD_STICK_L_EMULATION_DOWN,
                VPADAXIS_RIGHTX       = VPAD_STICK_R_EMULATION_LEFT | VPAD_STICK_R_EMULATION_RIGHT,
                VPADAXIS_RIGHTY       = VPAD_STICK_R_EMULATION_UP | VPAD_STICK_R_EMULATION_DOWN,
                VPADAXIS_TRIGGERLEFT  = VPAD_BUTTON_ZL,
                VPADAXIS_TRIGGERRIGHT = VPAD_BUTTON_ZR
            };

            // clang-format off
            ENUMMAP_DECLARE(VpadAxes, GamepadAxis, VPADAxis,
                { GAMEPAD_AXIS_LEFTX,        VPADAXIS_LEFTX        },
                { GAMEPAD_AXIS_LEFTY,        VPADAXIS_LEFTY        },
                { GAMEPAD_AXIS_RIGHTX,       VPADAXIS_RIGHTX       },
                { GAMEPAD_AXIS_RIGHTY,       VPADAXIS_RIGHTY       },
                { GAMEPAD_AXIS_TRIGGERLEFT,  VPADAXIS_TRIGGERLEFT  },
                { GAMEPAD_AXIS_TRIGGERRIGHT, VPADAXIS_TRIGGERRIGHT }
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
            // clang-format on

          private:
            VPADStatus status;
            VPADReadError error;

            mutable struct State
            {
                uint32_t pressed;
                uint32_t released;
                uint32_t held;
            } state;
        };
    } // namespace vpad
} // namespace love
