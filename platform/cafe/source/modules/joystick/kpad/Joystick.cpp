#include <cstring>
#include <padscore/wpad.h>

#include "common/debug.hpp"
#include "common/screen.hpp"

#include "modules/joystick/kpad/Joystick.hpp"
#include "utility/guid.hpp"

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
            std::memset(&this->status, 0, sizeof(this->status));
            KPADReadEx(WPADChan(this->instanceId - 1), &this->status, 1, &this->error);
            LOG("Joystick::update: instanceId=%d error=%d", this->instanceId, this->error);
        }

        bool Joystick::open(int64_t deviceId)
        {
            if (deviceId > WPAD_CHAN_4)
                return false;

            int index = (int)deviceId;
            this->close();

            this->instanceId = index;

            if (WPADProbe(WPADChan(index - 1), &this->extension) < 0)
                return false;
            LOG("Joystick::open: index=%d extension=%d", index, this->extension);
            if (!Joystick::getConstant(this->extension, this->gamepadType))
                this->gamepadType = GAMEPAD_TYPE_UNKNOWN;
            LOG("Joystick::open: gamepadType=%d", this->gamepadType);
            this->guid = love::getGamepadGUID(this->gamepadType);
            if (!Joystick::getConstant(this->gamepadType, this->name))
                this->name = "Unknown";
            LOG("Joystick::open: name=%s", this->name.data());
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
                    else if (axis == GAMEPAD_AXIS_TRIGGERRIGHT)
                        return this->status.classic.hold & WPAD_CLASSIC_BUTTON_ZR ? 1.0f : 0.0f;

                    break;
                }
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK:
                {
                    if (axis == GAMEPAD_AXIS_LEFTX)
                        return clamp(this->status.nunchuk.stick.x);
                    else if (axis == GAMEPAD_AXIS_LEFTY)
                        return clamp(this->status.nunchuk.stick.y);

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
                    else if (axis == GAMEPAD_AXIS_TRIGGERRIGHT)
                        return this->status.pro.hold & WPAD_PRO_TRIGGER_ZR ? 1.0f : 0.0f;

                    break;
                }
                default:
                    break;
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

        bool Joystick::isDown(std::span<Joystick::GamepadButton> buttons) const
        {
            if (!this->isConnected())
                return false;

            switch (this->gamepadType)
            {
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE:
                default:
                    return this->checkButtonImpl<WPADButton>(buttons, this->status.trigger);
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK:
                    return this->checkButtonImpl<WPADButton>(buttons, this->status.trigger) ||
                           this->checkButtonImpl<WPADNunchukButton>(buttons, this->status.nunchuk.trigger);
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                    return this->checkButtonImpl<WPADClassicButton>(buttons, this->status.classic.trigger);
                case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                    return this->checkButtonImpl<WPADProButton>(buttons, this->status.pro.trigger);
            }

            return false;
        }

        bool Joystick::isHeld(std::span<Joystick::GamepadButton> buttons) const
        {
            if (!this->isConnected())
                return false;

            switch (this->gamepadType)
            {
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE:
                    return this->checkButtonImpl<WPADButton>(buttons, this->status.hold);
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK:
                    return this->checkButtonImpl<WPADButton>(buttons, this->status.hold) ||
                           this->checkButtonImpl<WPADNunchukButton>(buttons, this->status.nunchuk.hold);
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                    return this->checkButtonImpl<WPADClassicButton>(buttons, this->status.classic.hold);
                case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                    return this->checkButtonImpl<WPADProButton>(buttons, this->status.pro.hold);
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
                    return this->checkButtonImpl<WPADButton>(buttons, this->status.release);
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK:
                    return this->checkButtonImpl<WPADButton>(buttons, this->status.release) ||
                           this->checkButtonImpl<WPADNunchukButton>(buttons, this->status.nunchuk.release);
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                    return this->checkButtonImpl<WPADClassicButton>(buttons, this->status.classic.release);
                case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                    return this->checkButtonImpl<WPADProButton>(buttons, this->status.pro.release);
                default:
                    break;
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
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK:
                    return this->isAxisValueChangedImpl<NunchuckAxis>(axis, this->status.nunchuk.hold,
                                                                      this->status.nunchuk.release);
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                    return this->isAxisValueChangedImpl<ClassicAxis>(axis, this->status.classic.hold,
                                                                     this->status.classic.release);
                case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                    return this->isAxisValueChangedImpl<ProAxis>(axis, this->status.pro.hold,
                                                                 this->status.pro.release);
                default:
                    break;
            }

            return false;
        }

        Joystick::PowerType Joystick::getPowerInfo(int& percent) const
        {
            const auto battery = WPADGetBatteryLevel(WPADChan(this->instanceId - 1));
            percent            = battery * 25.0f;

            if (this->gamepadType != GAMEPAD_TYPE_NINTENDO_WII_U_PRO)
                return POWER_ON_BATTERY;

            if (!this->status.pro.charging && battery == 4)
                return POWER_CHARGED;

            return (this->status.pro.charging) ? POWER_CHARGING : POWER_ON_BATTERY;
        }

        Joystick::ConnectionType Joystick::getConnectionState() const
        {
            return CONNECTION_UNKNOWN;
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

            switch (this->gamepadType)
            {
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE:
                {
                    data.push_back(this->status.acc.x);
                    data.push_back(this->status.acc.y);
                    data.push_back(this->status.acc.z);
                    break;
                }
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK:
                    data.push_back(this->status.acc.x);
                    data.push_back(this->status.acc.y);
                    data.push_back(this->status.acc.z);

                    data.push_back(this->status.nunchuk.acc.x);
                    data.push_back(this->status.nunchuk.acc.y);
                    data.push_back(this->status.nunchuk.acc.z);
                    break;
                case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                default:
                    break;
            }

            return data;
        }

        Vector2 Joystick::getPosition() const
        {
            switch (this->gamepadType)
            {
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK:
                {
                    if (this->status.posValid)
                        return Vector2 { this->status.pos.x, this->status.pos.y };
                }
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                default:
                    break;
            }

            return Vector2 {};
        }

        Vector2 Joystick::getAngle() const
        {
            Vector2 result {};

            switch (this->gamepadType)
            {
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK:
                {
                    if (!this->status.posValid)
                        break;

                    result.x = this->status.angle.x;
                    result.y = this->status.angle.y;
                    break;
                }
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                default:
                    break;
            }

            return result;
        }
    } // namespace kpad
} // namespace love
