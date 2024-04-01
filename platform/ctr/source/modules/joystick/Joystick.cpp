#include "modules/joystick/Joystick.hpp"
#include "utility/guid.hpp"

#include "utility/logfile.hpp"

namespace love
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

    bool Joystick::open(int64_t deviceId)
    {
        int index = (int)deviceId;
        this->close();

        this->instanceId = 0;

        CFG_SystemModel model = (CFG_SystemModel)0;
        CFGU_GetSystemModel((uint8_t*)&model);

        if (!Joystick::getConstant(model, this->gamepadType))
            this->gamepadType = GAMEPAD_TYPE_UNKNOWN;

        this->guid = getGamepadGUID(this->gamepadType);

        if (!Joystick::getConstant(this->gamepadType, this->name))
            this->name = "Unknown";

        this->joystickType = JOYSTICK_TYPE_GAMEPAD;

        return this->isConnected();
    }

    void Joystick::close()
    {
        this->instanceId = -1;
    }

    bool Joystick::isConnected() const
    {
        return true;
    }

    float Joystick::getAxis(GamepadAxis axis) const
    {
        switch (axis)
        {
            case GAMEPAD_AXIS_LEFTX:
            case GAMEPAD_AXIS_LEFTY:
            {
                circlePosition position {};
                hidCircleRead(&position);

                return axis == GAMEPAD_AXIS_LEFTX ? clamp(position.dx) : clamp(position.dy);
            }
            case GAMEPAD_AXIS_RIGHTX:
            case GAMEPAD_AXIS_RIGHTY:
            {
                circlePosition position {};
                irrstCstickRead(&position);

                return axis == GAMEPAD_AXIS_RIGHTX ? clamp(position.dx) : clamp(position.dy);
            }
            case GAMEPAD_AXIS_TRIGGERLEFT:
                return hidKeysHeld() & KEY_ZL ? 1.0f : 0.0f;
            case GAMEPAD_AXIS_TRIGGERRIGHT:
                return hidKeysHeld() & KEY_ZR ? 1.0f : 0.0f;
            default:
                return 0.0f;
        }
    }

    std::vector<float> Joystick::getAxes() const
    {
        std::vector<float> axes {};
        int count = this->getAxisCount();

        if (!this->isConnected() || count <= 0)
            return axes;

        axes.reserve(count);

        for (int index = 0; index < count; index++)
            axes.push_back(this->getAxis((GamepadAxis)index));

        return axes;
    }

    bool Joystick::isDown(std::span<GamepadButton> buttons) const
    {
        if (!this->isConnected())
            return false;

        HidKeyType result;

        for (GamepadButton button : buttons)
        {
            if (!Joystick::getConstant(button, result))
                continue;

            if (hidKeysDown() & result)
                return true;
        }

        return false;
    }

    void Joystick::setPlayerIndex(int)
    {}

    int Joystick::getPlayerIndex() const
    {
        return 0;
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
            case Sensor::SENSOR_GYROSCOPE:
                return true;
            default:
                return false;
        }
    }

    bool Joystick::isSensorEnabled(Sensor::SensorType type) const
    {
        return this->sensors.at(type) == true;
    }

    void Joystick::setSensorEnabled(Sensor::SensorType type, bool enable)
    {
        Result result = 0;

        switch (type)
        {
            case Sensor::SENSOR_ACCELEROMETER:
                result = enable ? HIDUSER_EnableAccelerometer() : HIDUSER_DisableAccelerometer();
                break;
            case Sensor::SENSOR_GYROSCOPE:
                result = enable ? HIDUSER_EnableGyroscope() : HIDUSER_DisableGyroscope();
                break;
            default:
                break;
        }

        this->sensors[type] = R_SUCCEEDED(result);
    }

    std::vector<float> Joystick::getSensorData(Sensor::SensorType type) const
    {
        std::vector<float> data(3);

        if (!this->isSensorEnabled(type))
        {
            std::string_view name = "Unknown";
            Sensor::getConstant(type, name);

            throw love::Exception("\"{}\" gamepad sensor is not enabled.", name);
        }

        switch (type)
        {
            case Sensor::SENSOR_ACCELEROMETER:
            {
                accelVector vector {};
                hidAccelRead(&vector);

                return { (float)vector.x, (float)vector.y, (float)vector.z };
            }
            case Sensor::SENSOR_GYROSCOPE:
            {
                angularRate vector {};
                hidGyroRead(&vector);

                return { (float)vector.x, (float)vector.y, (float)vector.z };
            }
            default:
                return std::vector<float>();
        }
    }
} // namespace love
