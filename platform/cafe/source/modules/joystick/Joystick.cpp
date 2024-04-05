#include "modules/joystick/Joystick.hpp"

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

    void Joystick::update()
    {
        if (this->isGamepad())
            VPADRead(VPAD_CHAN_0, &this->vpadStatus, 1, &this->vpadError);
        else
            KPADReadEx(KPADChan(this->id - 1), &this->kpadStatus, 1, &this->kpadError);
    }

    static GamepadType getSystemGamepadType(int id)
    {
        if (id == 0)
            return GAMEPAD_TYPE_NINTENDO_WII_U_GAMEPAD;

        KPADStatus status {};
        KPADError error = KPAD_ERROR_OK;

        KPADReadEx(KPADChan(id - 1), &status, 16, &error);

        GamepadType type = GAMEPAD_TYPE_UNKNOWN;
        Joystick::getConstant((WPADExtensionType)status.extensionType, type);

        return type;
    }

    bool Joystick::open(int64_t deviceId)
    {
        int index = (int)deviceId;
        this->close();

        this->instanceId = 0;

        this->gamepadType = getSystemGamepadType(this->id);
        this->guid        = getGamepadGUID(this->gamepadType);

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
        if (this->isDRCGamepad())
        {
            LOG("VPADStatus: {}", (int)this->vpadError);
            switch (this->vpadError)
            {
                case VPAD_READ_INVALID_CONTROLLER:
                case VPAD_READ_UNINITIALIZED:
                    return false;
                default:
                    return true;
            }
        }

        LOG("KPADStatus: {}", (int)this->kpadError);
        switch (this->kpadError)
        {
            case KPAD_ERROR_INVALID_CONTROLLER:
            case KPAD_ERROR_UNINITIALIZED:
                return false;
            default:
                return true;
        }

        return false;
    }

    float Joystick::getVPADAxis(GamepadAxis axis) const
    {
        const auto& status = this->vpadStatus;

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
    }

    float Joystick::getKPADAxis(GamepadAxis axis) const
    {
        const auto& status = this->kpadStatus;

        switch (this->gamepadType)
        {
            case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
            {
                switch (axis)
                {
                    case GAMEPAD_AXIS_LEFTX:
                        return clamp(status.pro.leftStick.x);
                    case GAMEPAD_AXIS_LEFTY:
                        return clamp(status.pro.leftStick.y);
                    case GAMEPAD_AXIS_RIGHTX:
                        return clamp(status.pro.rightStick.x);
                    case GAMEPAD_AXIS_RIGHTY:
                        return clamp(status.pro.rightStick.y);
                    case GAMEPAD_AXIS_TRIGGERLEFT:
                        return status.hold & WPAD_PRO_TRIGGER_ZL ? 1.0f : 0.0f;
                    case GAMEPAD_AXIS_TRIGGERRIGHT:
                        return status.hold & WPAD_PRO_TRIGGER_ZR ? 1.0f : 0.0f;
                    default:
                        break;
                }
                break;
            }
            default:
                return 0.0f;
        }

        return 0.0f;
    }

    float Joystick::getAxis(GamepadAxis axis) const
    {
        switch (this->gamepadType)
        {
            case GAMEPAD_TYPE_NINTENDO_WII_U_GAMEPAD:
                return this->getVPADAxis(axis);
            case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                return this->getKPADAxis(axis);
            default:
                return 0.0f;
        }
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

    bool Joystick::isVPADButtonDown(std::span<GamepadButton> buttons) const
    {
        VPADButtons result;

        for (const auto& button : buttons)
        {
            if (!Joystick::getConstant(button, result))
                continue;

            if (this->vpadStatus.trigger & result)
                return true;
        }

        return false;
    }

    static bool isProControllerDown(const KPADStatus& status,
                                    std::span<Joystick::GamepadButton> buttons)
    {
        WPADProButton result;

        for (const auto& button : buttons)
        {
            if (!Joystick::getConstant(button, result))
                continue;

            if (status.pro.trigger & result)
                return true;
        }

        return false;
    }

    bool Joystick::isKPADButtonDown(std::span<GamepadButton> buttons) const
    {
        switch (this->gamepadType)
        {
            case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                return isProControllerDown(this->kpadStatus, buttons);
            default:
                break;
        }

        return false;
    }

    bool Joystick::isDown(std::span<GamepadButton> buttons) const
    {
        if (!this->isConnected())
            return false;

        if (this->isDRCGamepad())
            return this->isVPADButtonDown(buttons);
        else
            return this->isKPADButtonDown(buttons);

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

        if (!this->isSensorEnabled(type))
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
                const auto& accelerometer = this->vpadStatus.accelorometer;

                data.push_back(accelerometer.acc.x);
                data.push_back(accelerometer.acc.y);
                data.push_back(accelerometer.acc.z);

                return data;
            }
            case Sensor::SENSOR_GYROSCOPE:
            {
                const auto& gyroscope = this->vpadStatus.gyro;

                data.push_back(gyroscope.x);
                data.push_back(gyroscope.y);
                data.push_back(gyroscope.z);

                return data;
            }
            default:
                break;
        }

        return data;
    }
} // namespace love
