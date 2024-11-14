#include "modules/joystick/vpad/Joystick.hpp"

namespace love
{
    namespace vpad
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
            VPADRead(VPAD_CHAN_0, &this->status, 1, &this->error);

            const auto& status = this->status;
            if (this->error != VPAD_READ_NO_SAMPLES)
                this->state = { status.trigger, status.release, status.hold };
        }

        bool Joystick::open(int64_t deviceId)
        {
            int index = (int)deviceId;
            this->close();

            this->instanceId = 0;

            this->gamepadType = GAMEPAD_TYPE_NINTENDO_WII_U_GAMEPAD;
            this->guid        = love::getGamepadGUID(this->gamepadType);

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
                case VPAD_READ_INVALID_CONTROLLER:
                case VPAD_READ_UNINITIALIZED:
                    return false;
                default:
                    return true;
            }

            return false;
        }

        float Joystick::getAxis(GamepadAxis axis) const
        {
            const auto status = this->status;

            switch (axis)
            {
                case GAMEPAD_AXIS_LEFTX:
                    return clamp(status.leftStick.x);
                case GAMEPAD_AXIS_LEFTY:
                    return clamp(status.leftStick.y);
                case GAMEPAD_AXIS_RIGHTX:
                    return clamp(status.rightStick.x);
                case GAMEPAD_AXIS_RIGHTY:
                    return clamp(status.rightStick.y);
                case GAMEPAD_AXIS_TRIGGERLEFT:
                    return status.hold & VPAD_BUTTON_ZL ? 1.0f : 0.0f;
                case GAMEPAD_AXIS_TRIGGERRIGHT:
                    return status.hold & VPAD_BUTTON_ZR ? 1.0f : 0.0f;
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

        bool Joystick::isDown(std::span<GamepadButton> buttons) const
        {
            VPADButtons result;

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

        bool Joystick::isUp(std::span<GamepadButton> buttons) const
        {
            if (!this->isConnected())
                return false;

            VPADButtons result;

            for (GamepadButton button : buttons)
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

        bool Joystick::isAxisChanged(GamepadAxis axis) const
        {
            if (!this->isConnected())
                return false;

            VPADAxis result;

            if (!Joystick::getConstant(axis, result))
                return false;

            if ((this->state.held & result) || (this->state.released & result))
            {
                this->state.held ^= result;
                return true;
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

            switch (type)
            {
                case Sensor::SENSOR_ACCELEROMETER:
                {
                    data.push_back(this->status.accelorometer.acc.x);
                    data.push_back(this->status.accelorometer.acc.y);
                    data.push_back(this->status.accelorometer.acc.z);

                    return data;
                }
                case Sensor::SENSOR_GYROSCOPE:
                {
                    data.push_back(this->status.gyro.x);
                    data.push_back(this->status.gyro.y);
                    data.push_back(this->status.gyro.z);

                    return data;
                }
                default:
                    break;
            }

            return data;
        }
    } // namespace vpad
} // namespace love
