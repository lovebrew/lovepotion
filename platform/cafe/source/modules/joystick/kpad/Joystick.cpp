#include "modules/joystick/kpad/Joystick.hpp"

namespace love
{
    namespace kpad
    {
        Joystick::Joystick(int id) : JoystickBase(id)
        {}

        Joystick::Joystick(int id, int index) : JoystickBase(id, index)
        {
            this->open(index);
        }

        Joystick::~Joystick()
        {
            this->close();
        }

        void Joystick::update()
        {
            KPADReadEx(WPAD_CHAN_0, &this->status, 1, &this->error);

            if (this->error != KPAD_ERROR_NO_SAMPLES)
            {
                switch (this->getGamepadType())
                {
                    case GAMEPAD_TYPE_NINTENDO_WII_REMOTE:
                    {
                        this->state = { this->status.trigger, this->status.release, this->status.hold };
                        break;
                    }
                    case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                    {
                        this->state = { this->status.classic.trigger, this->status.classic.release,
                                        this->status.classic.hold };
                        break;
                    }
                    case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                    {
                        this->state = { this->status.pro.trigger, this->status.pro.release,
                                        this->status.pro.hold };
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        bool Joystick::open(int64_t deviceId)
        {
            if (deviceId > WPAD_CHAN_4)
                return false;

            int index = (int)deviceId;
            this->close();

            this->instanceId = 0;

            WPADExtensionType extension;
            if (WPADProbe(WPADChan(index - 1), &extension) < 0)
                return false;

            switch (extension)
            {
                default:
                case WPAD_EXT_CORE:
                {
                    this->gamepadType = GAMEPAD_TYPE_NINTENDO_WII_REMOTE;
                    break;
                }
                case WPAD_EXT_CLASSIC:
                {
                    this->gamepadType = GAMEPAD_TYPE_NINTENDO_WII_CLASSIC;
                    break;
                }
                case WPAD_EXT_NUNCHUK:
                {
                    this->gamepadType = GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUCK;
                    break;
                }
                case WPAD_EXT_PRO_CONTROLLER:
                {
                    this->gamepadType = GAMEPAD_TYPE_NINTENDO_WII_U_PRO;
                    break;
                }
            }

            this->guid = love::getGamepadGUID(this->gamepadType);
            if (!Joystick::getConstant(this->gamepadType, this->name))
                this->name = "Unknown";

            this->joystickType = JOYSTICK_TYPE_GAMEPAD;
            this->update();

            return this->isConnected();
        }

        void Joystick::close()
        {
            this->instanceId = -1;
        }

        bool Joystick::isConnected() const
        {
            switch (this->error)
            {
                case KPAD_ERROR_INVALID_CONTROLLER:
                case KPAD_ERROR_UNINITIALIZED:
                    return false;
                default:
                    return true;
            }

            return false;
        }

        float Joystick::getAxis(GamepadAxis axis) const
        {
            switch (this->gamepadType)
            {
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE:
                    return 0.0f;
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                {
                    if (axis == GAMEPAD_AXIS_LEFTX)
                        return clamp(this->status.classic.leftStick.x);
                    else if (axis == GAMEPAD_AXIS_LEFTY)
                        return clamp(this->status.classic.leftStick.y);
                    else if (axis == GAMEPAD_AXIS_RIGHTX)
                        return clamp(this->status.classic.rightStick.x);
                    else if (axis == GAMEPAD_AXIS_RIGHTY)
                        return clamp(this->status.classic.rightStick.y);
                    else if (axis == GAMEPAD_AXIS_TRIGGERLEFT)
                        return this->status.classic.hold & WPAD_CLASSIC_BUTTON_ZL ? 1.0f : 0.0f;

                    return this->status.classic.hold & WPAD_CLASSIC_BUTTON_ZR ? 1.0f : 0.0f;
                }
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUCK:
                {
                    if (axis == GAMEPAD_AXIS_TRIGGERLEFT)
                        return this->status.nunchuk.hold & WPAD_NUNCHUK_BUTTON_Z ? 1.0f : 0.0f;

                    break;
                }
                case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                {
                    if (axis == GAMEPAD_AXIS_LEFTX)
                        return clamp(this->status.pro.leftStick.x);
                    else if (axis == GAMEPAD_AXIS_LEFTY)
                        return clamp(this->status.pro.leftStick.y);
                    else if (axis == GAMEPAD_AXIS_RIGHTX)
                        return clamp(this->status.pro.rightStick.x);
                    else if (axis == GAMEPAD_AXIS_RIGHTY)
                        return clamp(this->status.pro.rightStick.y);
                    else if (axis == GAMEPAD_AXIS_TRIGGERLEFT)
                        return this->status.pro.hold & WPAD_PRO_TRIGGER_ZL ? 1.0f : 0.0f;

                    return this->status.pro.hold & WPAD_PRO_TRIGGER_ZR ? 1.0f : 0.0f;
                }
                default:
                    return 0.0f;
            }

            return 0.0f;
        }

        std::vector<float> Joystick::getAxes() const
        {
            std::vector<float> axes;
            int count = this->getAxisCount();

            if (!this->isConnected() || count <= 0)
                return axes;

            axes.reserve(count);

            for (int i = 0; i < count; i++)
                axes.push_back(this->getAxis((GamepadAxis)i));

            return axes;
        }

        template<typename T>
        bool Joystick::isButtonDown(std::span<Joystick::GamepadButton> buttons) const
        {
            T result;

            for (const auto& button : buttons)
            {
                if (!Joystick::getConstant(button, result))
                    continue;

                if (this->state.pressed & result)
                {
                    this->state.pressed ^= result;
                    return true;
                }
            }

            return false;
        }

        template<typename T>
        bool Joystick::isButtonUp(std::span<Joystick::GamepadButton> buttons) const
        {
            T result;

            for (const auto& button : buttons)
            {
                if (!Joystick::getConstant(button, result))
                    continue;

                if (this->state.released & result)
                {
                    this->state.released ^= result;
                    return true;
                }
            }

            return false;
        }

        bool Joystick::isDown(std::span<Joystick::GamepadButton> buttons) const
        {
            if (!this->isConnected())
                return false;

            switch (this->gamepadType)
            {
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE:
                    return isButtonDown<WPADButton>(buttons);
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUCK:
                    break;
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                    return isButtonDown<WPADClassicButton>(buttons);
                case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                    return isButtonDown<WPADProButton>(buttons);
                default:
                    break;
            }

            return false;
        }

        bool Joystick::isUp(std::span<GamepadButton> buttons) const
        {
            if (!this->isConnected())
                return false;

            switch (this->gamepadType)
            {
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE:
                    return isButtonUp<WPADButton>(buttons);
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUCK:
                    break;
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                    return isButtonUp<WPADClassicButton>(buttons);
                case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                    return isButtonUp<WPADProButton>(buttons);
                default:
                    break;
            }

            return false;
        }

        template<typename T>
        bool Joystick::isAxisValueChanged(GamepadAxis axis) const
        {
            T result;

            if (!Joystick::getConstant(axis, result))
                return false;

            if ((this->state.held & result) || (this->state.released & result))
            {
                this->state.held ^= result;
                return true;
            }

            return false;
        }

        bool Joystick::isAxisChanged(GamepadAxis axis) const
        {
            if (!this->isConnected())
                return false;

            switch (this->gamepadType)
            {
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE:
                    break;
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUCK:
                    return isAxisValueChanged<NunchuckAxis>(axis);
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                    return isAxisValueChanged<ClassicAxis>(axis);
                case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                    return isAxisValueChanged<ProAxis>(axis);
                default:
                    break;
            }

            return false;
        }

        void Joystick::setPlayerIndex(int)
        {}

        int Joystick::getPlayerIndex() const
        {
            return this->id;
        }

        Joystick::JoystickInput Joystick::getGamepadMapping(const GamepadInput& input) const
        {
            JoystickInput result {};

            return result;
        }

        std::string Joystick::getGamepadMappingString() const
        {
            return std::string();
        }

        bool Joystick::isVibrationSupported() const
        {
            return false;
        }

        bool Joystick::setVibration(float, float, float)
        {
            return false;
        }

        bool Joystick::setVibration()
        {
            return false;
        }

        void Joystick::getVibration(float&, float&) const
        {}

        bool Joystick::hasSensor(Sensor::SensorType type) const
        {
            switch (type)
            {
                case Sensor::SENSOR_ACCELEROMETER:
                    return getGamepadHasAccelerometer(this->gamepadType);
                case Sensor::SENSOR_GYROSCOPE:
                    return getGamepadHasGyroscope(this->gamepadType);
                default:
                    break;
            }

            return false;
        }

        bool Joystick::isSensorEnabled(Sensor::SensorType type) const
        {
            return this->sensors.at(type) == true;
        }

        void Joystick::setSensorEnabled(Sensor::SensorType type, bool enable)
        {
            if (!this->hasSensor(type))
            {
                std::string_view name = "Unknown";
                Sensor::getConstant(type, name);

                throw love::Exception("\"{}\" gamepad sensor is not supported", name);
            }

            switch (type)
            {
                case Sensor::SENSOR_ACCELEROMETER:
                case Sensor::SENSOR_GYROSCOPE:
                    this->sensors[type] = enable;
                    break;
                default:
                    break;
            }
        }

        std::vector<float> Joystick::getSensorData(Sensor::SensorType type) const
        {
            std::vector<float> data {};

            if (!this->hasSensor(type))
            {
                std::string_view name = "Unknown";
                Sensor::getConstant(type, name);

                throw love::Exception("\"{}\" gamepad sensor is not enabled.", name);
            }

            data.reserve(3);

            return data;
        }
    } // namespace kpad
} // namespace love
