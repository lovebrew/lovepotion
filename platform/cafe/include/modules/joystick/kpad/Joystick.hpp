#pragma once

#include "common/Vector.hpp"

#include "modules/joystick/Joystick.tcc"
#include "utility/guid.hpp"

#include <padscore/kpad.h>
#include <padscore/wpad.h>

namespace love
{
    namespace kpad
    {
        class Joystick : public JoystickBase
        {
          public:
            Joystick(int id);

            Joystick(int id, int index);

            ~Joystick();

            virtual void update() override;

            virtual bool open(int64_t deviceId) override;

            virtual void close() override;

            virtual bool isConnected() const override;

            virtual float getAxis(GamepadAxis axis) const override;

            virtual std::vector<float> getAxes() const override;

            virtual bool isDown(std::span<GamepadButton> buttons) const override;

            virtual bool isHeld(std::span<GamepadButton> buttons) const override;

            virtual bool isUp(std::span<GamepadButton> buttons) const override;

            virtual bool isAxisChanged(GamepadAxis axis) const override;

            virtual ptrdiff_t getHandle() const override
            {
                return (ptrdiff_t)std::addressof(this->status);
            }

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

            PowerType getPowerInfo(int& percent) const override;

            ConnectionType getConnectionState() const override;

            Vector2 getPosition() const;

            Vector2 getAngle() const;

            using JoystickBase::getConstant;

            // #region Wii Remote
            // clang-format off
            ENUMMAP_DECLARE(CoreButtons, GamepadButton, WPADButton,
                { GAMEPAD_BUTTON_A, WPAD_BUTTON_A },
                { GAMEPAD_BUTTON_B, WPAD_BUTTON_B },

                { GAMEPAD_BUTTON_DPAD_LEFT,  WPAD_BUTTON_LEFT  },
                { GAMEPAD_BUTTON_DPAD_UP,    WPAD_BUTTON_UP    },
                { GAMEPAD_BUTTON_DPAD_RIGHT, WPAD_BUTTON_RIGHT },
                { GAMEPAD_BUTTON_DPAD_DOWN,  WPAD_BUTTON_DOWN  },

                { GAMEPAD_BUTTON_START, WPAD_BUTTON_PLUS  },
                { GAMEPAD_BUTTON_BACK,  WPAD_BUTTON_MINUS },

                { GAMEPAD_BUTTON_MISC1, WPAD_BUTTON_1 },
                { GAMEPAD_BUTTON_MISC2, WPAD_BUTTON_2 }
            )
            // clang-format on
            // #endregion

            // #region WiimoteNunchuck
            enum NunchuckAxis
            {
                NUNCHUCK_AXIS_X = WPAD_NUNCHUK_STICK_EMULATION_LEFT | WPAD_NUNCHUK_STICK_EMULATION_RIGHT,
                NUNCHUCK_AXIS_Y = WPAD_NUNCHUK_STICK_EMULATION_UP | WPAD_NUNCHUK_STICK_EMULATION_DOWN
            };

            // clang-format off
            ENUMMAP_DECLARE(NunchuckButtons, GamepadButton, WPADNunchukButton,
                { GAMEPAD_BUTTON_C, WPAD_NUNCHUK_BUTTON_C },
                { GAMEPAD_BUTTON_Z, WPAD_NUNCHUK_BUTTON_Z }
            );

            ENUMMAP_DECLARE(NunchuckAxes, GamepadAxis, NunchuckAxis,
                { GAMEPAD_AXIS_LEFTX, NUNCHUCK_AXIS_X },
                { GAMEPAD_AXIS_LEFTY, NUNCHUCK_AXIS_Y },
            );
            // clang-format on
            // #endregion

            // #region Classic
            // clang-format off
            ENUMMAP_DECLARE(GamepadTypes, WPADExtensionType, GamepadType,
                { WPAD_EXT_CORE,           GAMEPAD_TYPE_NINTENDO_WII_REMOTE         },
                { WPAD_EXT_CLASSIC,        GAMEPAD_TYPE_NINTENDO_WII_CLASSIC        },
                { WPAD_EXT_NUNCHUK,        GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK },
                { WPAD_EXT_PRO_CONTROLLER, GAMEPAD_TYPE_NINTENDO_WII_U_PRO          }
            );

            enum ClassicAxis
            {
                CLASSIC_AXIS_LEFTX  = WPAD_CLASSIC_STICK_L_EMULATION_LEFT | WPAD_CLASSIC_STICK_L_EMULATION_RIGHT,
                CLASSIC_AXIS_LEFTY  = WPAD_CLASSIC_STICK_L_EMULATION_UP | WPAD_CLASSIC_STICK_L_EMULATION_DOWN,
                CLASSIC_AXIS_RIGHTX = WPAD_CLASSIC_STICK_R_EMULATION_LEFT | WPAD_CLASSIC_STICK_R_EMULATION_RIGHT,
                CLASSIC_AXIS_RIGHTY = WPAD_CLASSIC_STICK_R_EMULATION_UP | WPAD_CLASSIC_STICK_R_EMULATION_DOWN,
                CLASSIC_AXIS_TRIGGERLEFT = WPAD_CLASSIC_BUTTON_ZL,
                CLASSIC_AXIS_TRIGGERRIGHT = WPAD_CLASSIC_BUTTON_ZR,
            };

            ENUMMAP_DECLARE(ClassicButtons, GamepadButton, WPADClassicButton,
                { GAMEPAD_BUTTON_A, WPAD_CLASSIC_BUTTON_A },
                { GAMEPAD_BUTTON_B, WPAD_CLASSIC_BUTTON_B },
                { GAMEPAD_BUTTON_X, WPAD_CLASSIC_BUTTON_X },
                { GAMEPAD_BUTTON_Y, WPAD_CLASSIC_BUTTON_Y },

                { GAMEPAD_BUTTON_DPAD_LEFT,     WPAD_CLASSIC_BUTTON_LEFT  },
                { GAMEPAD_BUTTON_DPAD_UP,       WPAD_CLASSIC_BUTTON_UP    },
                { GAMEPAD_BUTTON_DPAD_RIGHT,    WPAD_CLASSIC_BUTTON_RIGHT },
                { GAMEPAD_BUTTON_DPAD_DOWN,     WPAD_CLASSIC_BUTTON_DOWN  },

                { GAMEPAD_BUTTON_START,         WPAD_CLASSIC_BUTTON_PLUS  },
                { GAMEPAD_BUTTON_BACK,          WPAD_CLASSIC_BUTTON_MINUS },

                { GAMEPAD_BUTTON_LEFTSHOULDER,  WPAD_CLASSIC_BUTTON_L },
                { GAMEPAD_BUTTON_RIGHTSHOULDER, WPAD_CLASSIC_BUTTON_R }
            );

            ENUMMAP_DECLARE(ClassicAxes, GamepadAxis, ClassicAxis,
                { GAMEPAD_AXIS_LEFTX,        CLASSIC_AXIS_LEFTX        },
                { GAMEPAD_AXIS_LEFTY,        CLASSIC_AXIS_LEFTY        },
                { GAMEPAD_AXIS_RIGHTX,       CLASSIC_AXIS_RIGHTX       },
                { GAMEPAD_AXIS_RIGHTY,       CLASSIC_AXIS_RIGHTY       },
                { GAMEPAD_AXIS_TRIGGERLEFT,  CLASSIC_AXIS_TRIGGERLEFT  },
                { GAMEPAD_AXIS_TRIGGERRIGHT, CLASSIC_AXIS_TRIGGERRIGHT }
            );
            // clang-format on
            // #endregion

            // #region Pro
            enum ProAxis
            {
                PRO_AXIS_LEFTX  = WPAD_PRO_STICK_L_EMULATION_LEFT | WPAD_PRO_STICK_L_EMULATION_RIGHT,
                PRO_AXIS_LEFTY  = WPAD_PRO_STICK_L_EMULATION_UP | WPAD_PRO_STICK_L_EMULATION_DOWN,
                PRO_AXIS_RIGHTX = WPAD_PRO_STICK_R_EMULATION_LEFT | WPAD_PRO_STICK_R_EMULATION_RIGHT,
                PRO_AXIS_RIGHTY = WPAD_PRO_STICK_R_EMULATION_UP | WPAD_PRO_STICK_R_EMULATION_DOWN
            };

            // clang-format off
            ENUMMAP_DECLARE(ProButtons, GamepadButton, WPADProButton,
                { GAMEPAD_BUTTON_A, WPAD_PRO_BUTTON_A },
                { GAMEPAD_BUTTON_B, WPAD_PRO_BUTTON_B },
                { GAMEPAD_BUTTON_X, WPAD_PRO_BUTTON_X },
                { GAMEPAD_BUTTON_Y, WPAD_PRO_BUTTON_Y },

                { GAMEPAD_BUTTON_DPAD_LEFT,  WPAD_PRO_BUTTON_LEFT  },
                { GAMEPAD_BUTTON_DPAD_UP,    WPAD_PRO_BUTTON_UP    },
                { GAMEPAD_BUTTON_DPAD_RIGHT, WPAD_PRO_BUTTON_RIGHT },
                { GAMEPAD_BUTTON_DPAD_DOWN,  WPAD_PRO_BUTTON_DOWN  },

                { GAMEPAD_BUTTON_START, WPAD_PRO_BUTTON_PLUS  },
                { GAMEPAD_BUTTON_BACK,  WPAD_PRO_BUTTON_MINUS },

                { GAMEPAD_BUTTON_LEFTSHOULDER,  WPAD_PRO_TRIGGER_L },
                { GAMEPAD_BUTTON_RIGHTSHOULDER, WPAD_PRO_TRIGGER_R },

                { GAMEPAD_BUTTON_LEFTSTICK,  WPAD_PRO_BUTTON_STICK_L },
                { GAMEPAD_BUTTON_RIGHTSTICK, WPAD_PRO_BUTTON_STICK_R }
            );

            ENUMMAP_DECLARE(ProAxes, GamepadAxis, ProAxis,
                { GAMEPAD_AXIS_LEFTX,  PRO_AXIS_LEFTX  },
                { GAMEPAD_AXIS_LEFTY,  PRO_AXIS_LEFTY  },
                { GAMEPAD_AXIS_RIGHTX, PRO_AXIS_RIGHTX },
                { GAMEPAD_AXIS_RIGHTY, PRO_AXIS_RIGHTY }
            );
            // clang-format on
            // #endregion

          private:
            template<typename ButtonType>
            bool checkButtonImpl(std::span<Joystick::GamepadButton> buttons, uint32_t field) const
            {
                ButtonType result;

                for (Joystick::GamepadButton button : buttons)
                {
                    if (!Joystick::getConstant(button, result))
                        continue;

                    if (field & result)
                        return true;
                }

                return false;
            }

            template<typename Button>
            bool isAxisValueChangedImpl(GamepadAxis axis, uint32_t held, uint32_t released) const
            {
                Button result;

                if (!Joystick::getConstant(axis, result))
                    return false;

                if ((held & result) || (released & result))
                    return true;

                return false;
            }

            KPADStatus status;
            KPADError error;

            WPADExtensionType extension;

            struct WPADRumble
            {
                float duration;
                float start;
            } rumble;
        };
    } // namespace kpad
} // namespace love
