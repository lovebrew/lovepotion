#include "modules/joystick/Joystick.hpp"

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

        if (deviceId == 0)
            padInitializeDefault(&this->state);
        else
            padInitialize(&this->state, HidNpadIdType(index));

        padUpdate(&this->state);

        const uint32_t tag = padGetStyleSet(&this->state);

        if (tag & HidNpadStyleTag_NpadHandheld)
            this->gamepadType = GAMEPAD_TYPE_NINTENDO_SWITCH_HANDHELD;
        else if (tag & HidNpadStyleTag_NpadFullKey)
            this->gamepadType = GAMEPAD_TYPE_NINTENDO_SWITCH_PRO;
        else if (tag & HidNpadStyleTag_NpadJoyDual)
            this->gamepadType = GAMEPAD_TYPE_JOYCON_PAIR;
        else
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
        return padIsConnected(&this->state);
    }

    float Joystick::getAxis(GamepadAxis axis) const
    {
        switch (axis)
        {
            case GAMEPAD_AXIS_LEFTX:
            case GAMEPAD_AXIS_LEFTY:
            {
                const auto stick = padGetStickPos(&this->state, 0);
                return axis == GAMEPAD_AXIS_LEFTX ? clamp(stick.x) : clamp(stick.y);
            }
            case GAMEPAD_AXIS_RIGHTX:
            case GAMEPAD_AXIS_RIGHTY:
            {
                const auto stick = padGetStickPos(&this->state, 1);
                return axis == GAMEPAD_AXIS_RIGHTX ? clamp(stick.x) : clamp(stick.y);
            }
            case GAMEPAD_AXIS_TRIGGERLEFT:
                return padGetButtons(&this->state) & HidNpadButton_ZL ? 1.0f : 0.0f;
            case GAMEPAD_AXIS_TRIGGERRIGHT:
                return padGetButtons(&this->state) & HidNpadButton_ZR ? 1.0f : 0.0f;
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

        for (int i = 0; i < count; i++)
            axes.push_back(this->getAxis((GamepadAxis)i));

        return axes;
    }

    bool Joystick::isDown(std::span<GamepadButton> buttons) const
    {
        if (!this->isConnected())
            return false;

        HidNpadButton result;

        for (GamepadButton button : buttons)
        {
            if (!Joystick::getConstant(button, result))
                continue;

            if (padGetButtonsDown(&this->state) & result)
                return true;
        }

        return false;
    }

    bool Joystick::isUp(std::span<GamepadButton> buttons) const
    {
        if (!this->isConnected())
            return false;

        HidNpadButton result;

        for (GamepadButton button : buttons)
        {
            if (!Joystick::getConstant(button, result))
                continue;

            if (padGetButtonsUp(&this->state) & result)
                return true;
        }

        return false;
    }

    bool Joystick::isAxisChanged(GamepadAxis axis) const
    {
        if (!this->isConnected())
            return false;

        HidNpadAxis result;

        if (!Joystick::getConstant(axis, result))
            return false;

        if ((padGetButtons(&this->state) & result) || (padGetButtonsUp(&this->state) & result))
            return true;

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
        return true;
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

    HidNpadIdType Joystick::getHandleCount(int& handles, HidNpadStyleTag& tag) const
    {
        HidNpadIdType id = HidNpadIdType(HidNpadIdType_No1 + this->id);
        if (padIsHandheld(&this->state))
            id = HidNpadIdType_Handheld;

        Joystick::getConstant(this->gamepadType, tag);

        handles = 1;
        if (tag == HidNpadStyleTag_NpadJoyDual)
            handles = 2;

        return id;
    }

    bool Joystick::isSensorEnabled(Sensor::SensorType type) const
    {
        return this->sensors.at(type) == true;
    }

    void Joystick::setSensorEnabled(Sensor::SensorType type, bool enable)
    {
        int handles = 0;
        HidNpadStyleTag tag;

        HidNpadIdType id = this->getHandleCount(handles, tag);
        Result result    = 0;

        switch (type)
        {
            case Sensor::SENSOR_ACCELEROMETER:
                result = hidGetSixAxisSensorHandles(this->accelerometer, handles, id, tag);
                break;
            case Sensor::SENSOR_GYROSCOPE:
                result = hidGetSixAxisSensorHandles(this->gyroscope, handles, id, tag);
                break;
            default:
                return;
        }

        if (R_FAILED(result))
            return;

        switch (type)
        {
            case Sensor::SENSOR_ACCELEROMETER:
            {
                for (int index = 0; index < handles; index++)
                {
                    if (enable)
                        result = hidStartSixAxisSensor(this->accelerometer[index]);
                    else
                        hidStopSixAxisSensor(this->accelerometer[index]);
                }
                break;
            }
            case Sensor::SENSOR_GYROSCOPE:
            {
                for (int index = 0; index < handles; index++)
                {
                    if (enable)
                        result = hidStartSixAxisSensor(this->gyroscope[index]);
                    else
                        hidStopSixAxisSensor(this->gyroscope[index]);
                }
                break;
            }
            default:
                return;
        }

        this->sensors[type] = R_SUCCEEDED(result);
    }

    std::vector<float> Joystick::getSensorData(Sensor::SensorType type) const
    {
        std::vector<float> data {};

        if (!this->isSensorEnabled(type))
        {
            std::string_view name = "Unknown";
            Sensor::getConstant(type, name);

            throw love::Exception("\"{}\" gamepad sensor is not enabled.", name);
        }

        data.reserve(6);

        int handles = 0;
        HidNpadStyleTag tag;

        HidNpadIdType id = this->getHandleCount(handles, tag);

        switch (type)
        {
            case Sensor::SENSOR_ACCELEROMETER:
            {
                for (int index = 0; index < handles; index++)
                {
                    HidSixAxisSensorState state {};
                    hidGetSixAxisSensorStates(this->accelerometer[index], &state, 1);

                    data.push_back(state.acceleration.x);
                    data.push_back(state.acceleration.y);
                    data.push_back(state.acceleration.z);
                }
                break;
            }
            case Sensor::SENSOR_GYROSCOPE:
            {
                for (int index = 0; index < handles; index++)
                {
                    HidSixAxisSensorState state {};
                    hidGetSixAxisSensorStates(this->gyroscope[index], &state, 1);

                    data.push_back(state.angular_velocity.x);
                    data.push_back(state.angular_velocity.y);
                    data.push_back(state.angular_velocity.z);
                }
                break;
            }
            default:
                return data;
        }

        return data;
    }
} // namespace love
