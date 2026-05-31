#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <padscore/kpad.h>
#include <padscore/wpad.h>

#include "common/int.hpp"

#include "modules/joystick/Joystick.tcc"
#include "modules/joystick/kpad/Joystick.hpp"
#include "modules/timer/Timer.hpp"
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
            WPADExtensionType type;
            WPADProbe(this->channel, &type);

            if (type != this->extension)
            {
                WPADMplsMode mode = WPAD_MPLS_MODE_DISABLE;
                if (Joystick::getConstant(this->extension, mode))
                    KPADEnableMpls(this->channel, mode);

                if (!Joystick::getConstant(type, this->gamepadType))
                    this->gamepadType = GAMEPAD_TYPE_UNKNOWN;

                this->guid = love::getGamepadGUID(this->gamepadType);
                if (!Joystick::getConstant(this->gamepadType, this->name))
                    this->name = "Unknown";

                this->extension = type;
            }

            if ((Timer::getTime() * 1000.0f) >= this->rumble.start + this->rumble.duration)
                this->setVibration();

            std::memset(&this->status, 0, sizeof(this->status));
            KPADReadEx(this->channel, &this->status, 1, &this->error);
        }

        bool Joystick::open(int64_t deviceId)
        {
            if (deviceId > WPAD_CHAN_4)
                return false;

            int index = (int)deviceId;
            this->close();

            this->instanceId = index;
            this->channel    = KPADChan(index - 1);

            if (WPADProbe(this->channel, &this->extension) < 0)
                return false;

            WPADMplsMode mode = WPAD_MPLS_MODE_DISABLE;
            if (Joystick::getConstant(this->extension, mode))
                KPADEnableMpls(this->channel, mode);

            if (!Joystick::getConstant(this->extension, this->gamepadType))
                this->gamepadType = GAMEPAD_TYPE_UNKNOWN;

            this->guid = love::getGamepadGUID(this->gamepadType);
            if (!Joystick::getConstant(this->gamepadType, this->name))
                this->name = "Unknown";

            this->joystickType = JOYSTICK_TYPE_GAMEPAD;
            this->update();

            return this->isConnected();
        }

        void Joystick::close()
        {
            if (KPADGetMplsStatus(this->channel) != WPAD_MPLS_MODE_DISABLE)
                KPADDisableMpls(this->channel);

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
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS:
                {
                    if (axis == GAMEPAD_AXIS_RIGHTX)
                        return clamp(this->status.angle.x);
                    else if (axis == GAMEPAD_AXIS_RIGHTY)
                        return clamp(this->status.angle.y);
                    break;
                }
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS_CLASSIC:
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
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS_NUNCHUK:
                {
                    if (axis == GAMEPAD_AXIS_LEFTX)
                        return clamp(this->status.nunchuk.stick.x);
                    else if (axis == GAMEPAD_AXIS_LEFTY)
                        return clamp(this->status.nunchuk.stick.y);
                    else if (axis == GAMEPAD_AXIS_RIGHTX)
                        return clamp(this->status.angle.x);
                    else if (axis == GAMEPAD_AXIS_RIGHTY)
                        return clamp(this->status.angle.y);
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

        /*
         * For some reason, Wiimote D-Pad also causes WPAD_NUNCHUK_STICK_EMULATION_* to be set
         * Current logic does not affect the Classic Controller.
         */
        static KPADStatus checkNunchukData(const KPADStatus& status)
        {
            auto modified              = status;
            const auto [nunchuk, core] = std::tie(status.nunchuk, status);

            // clang-format off
            auto handleStickEmulation = [](uint32_t& modified, uint32_t core, uint32_t nunchuk, WPADButton button) {
                if (core & button && nunchuk & button)
                    modified &= ~button;
            };
            // clang-format on

            handleStickEmulation(modified.nunchuk.hold, core.hold, nunchuk.hold, WPAD_BUTTON_LEFT);
            handleStickEmulation(modified.nunchuk.hold, core.hold, nunchuk.hold, WPAD_BUTTON_RIGHT);
            handleStickEmulation(modified.nunchuk.hold, core.hold, nunchuk.hold, WPAD_BUTTON_UP);
            handleStickEmulation(modified.nunchuk.hold, core.hold, nunchuk.hold, WPAD_BUTTON_DOWN);

            handleStickEmulation(modified.nunchuk.release, core.release, nunchuk.release, WPAD_BUTTON_LEFT);
            handleStickEmulation(modified.nunchuk.release, core.release, nunchuk.release, WPAD_BUTTON_RIGHT);
            handleStickEmulation(modified.nunchuk.release, core.release, nunchuk.release, WPAD_BUTTON_UP);
            handleStickEmulation(modified.nunchuk.release, core.release, nunchuk.release, WPAD_BUTTON_DOWN);

            return modified;
        }

        bool Joystick::isDown(std::span<Joystick::GamepadButton> buttons) const
        {
            if (!this->isConnected())
                return false;

            switch (this->gamepadType)
            {
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE:
                default:
                    return this->isButtonDown<WPADButton>(buttons, this->status);
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS_NUNCHUK:
                {
                    auto ext = this->isButtonDown<WPADNunchukButton>(buttons, this->status.nunchuk);
                    return this->isButtonDown<WPADButton>(buttons, this->status) || ext;
                }
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS_CLASSIC:
                    return this->isButtonDown<WPADClassicButton>(buttons, this->status.classic);
                case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                    return this->isButtonDown<WPADProButton>(buttons, this->status.pro);
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
                    return this->isButtonHeld<WPADButton>(buttons, this->status);
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS_NUNCHUK:
                {
                    auto ext = this->isButtonHeld<WPADNunchukButton>(buttons, this->status.nunchuk);
                    return this->isButtonHeld<WPADButton>(buttons, this->status) || ext;
                }
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS_CLASSIC:
                    return this->isButtonHeld<WPADClassicButton>(buttons, this->status.classic);
                case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                    return this->isButtonHeld<WPADProButton>(buttons, this->status.pro);
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
                    return this->checkButtonUpImpl<WPADButton>(buttons, this->status);
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS_NUNCHUK:
                {
                    auto ext = this->checkButtonUpImpl<WPADNunchukButton>(buttons, this->status.nunchuk);
                    return this->checkButtonUpImpl<WPADButton>(buttons, this->status) || ext;
                }
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS_CLASSIC:
                    return this->checkButtonUpImpl<WPADClassicButton>(buttons, this->status.classic);
                case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                    return this->checkButtonUpImpl<WPADProButton>(buttons, this->status.pro);
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
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS:
                {
                    if (this->status.angleDiff.x != 0.0f || this->status.angleDiff.y != 0.0f)
                        return true;
                    break;
                }
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS_NUNCHUK:
                {
                    bool wiimote  = (this->status.angleDiff.x != 0.0f || this->status.angleDiff.y != 0.0f);
                    auto filtered = checkNunchukData(this->status);
                    return wiimote || this->isAxisValueChangedImpl<NunchukAxis>(axis, filtered.nunchuk);
                }
                case GAMEPAD_TYPE_NINTENDO_WII_CLASSIC:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS_CLASSIC:
                    return this->isAxisValueChangedImpl<ClassicAxis>(axis, this->status.classic);
                case GAMEPAD_TYPE_NINTENDO_WII_U_PRO:
                    return this->isAxisValueChangedImpl<ProAxis>(axis, this->status.pro);
                default:
                    break;
            }

            return false;
        }

        Joystick::PowerType Joystick::getPowerInfo(int& percent) const
        {
            const auto battery = WPADGetBatteryLevel(this->channel);
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
            return true;
        }

        bool Joystick::setVibration(float left, float right, float duration)
        {
            left  = std::clamp(left, 0.0f, 1.0f);
            right = std::clamp(right, 0.0f, 1.0f);

            if (left == 0.0f && right == 0.0f)
                return this->setVibration();

            if (!this->isConnected())
                return false;

            uint32_t length = LOVE_UINT32_MAX;
            if (duration >= 0.0f)
            {
                float maxDuration = (float)std::numeric_limits<uint32_t>::max() / 1000.0f;
                length            = uint32_t(std::min(duration, maxDuration) * 1000);
            }

            WPADControlMotor(this->channel, true);

            this->rumble.start    = Timer::getTime() * 1000.0f;
            this->rumble.duration = length;

            return true;
        }

        bool Joystick::setVibration()
        {
            if (!this->isConnected())
                return false;

            WPADControlMotor(this->channel, false);
            this->rumble.duration = 0;
            this->rumble.start    = 0;

            return true;
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

        // Wiimote and Nunchuk have accelerometer data.
        // Wii Motion Plus™ has gyroscope data.
        std::vector<float> Joystick::getSensorData(Sensor::SensorType type) const
        {
            std::vector<float> data(3);

            if (!this->isSensorEnabled(type))
            {
                std::string_view name = "Unknown";
                Sensor::getConstant(type, name);

                throw love::Exception("\"{}\" gamepad sensor is not enabled.", name);
            }

            switch (this->gamepadType)
            {
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK:
                {
                    if (type == Sensor::SENSOR_ACCELEROMETER)
                    {
                        data.push_back(this->status.acc.x);
                        data.push_back(this->status.acc.y);
                        data.push_back(this->status.acc.z);
                    }
                    break;
                }
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS_NUNCHUK:
                {
                    if (type == Sensor::SENSOR_GYROSCOPE)
                    {
                        data.push_back(this->status.mplus.acc.x);
                        data.push_back(this->status.mplus.acc.y);
                        data.push_back(this->status.mplus.acc.z);
                    }
                    break;
                }
                default:
                    break;
            }

            if (this->extension == WPAD_EXT_NUNCHUK && type == Sensor::SENSOR_ACCELEROMETER)
            {
                data.push_back(this->status.nunchuk.acc.x);
                data.push_back(this->status.nunchuk.acc.y);
                data.push_back(this->status.nunchuk.acc.z);
            }

            return data;
        }

        // TODO: position to TV coordinates
        // this is in a newer wut commit not released
        Vector2 Joystick::getPosition() const
        {
            // const auto& size = love::getScreenInfo();
            // KPADRect screen { .topLeft = { 0.0f, 0.0f }, .bottomRight = { size.x, size.y }};

            Vector2 position {};

            switch (this->gamepadType)
            {
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS_NUNCHUK:
                {
                    if (this->status.posValid)
                    {
                        // KPADGetProjectionPos(&position, this->status.pos, &screen, 1.0f);
                        return Vector2 { this->status.pos.x, this->status.pos.y };
                    }
                }
                default:
                    break;
            }

            return position;
        }

        Vector2 Joystick::getAngle() const
        {
            Vector2 result {};

            switch (this->gamepadType)
            {
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS:
                case GAMEPAD_TYPE_NINTENDO_WII_REMOTE_MOTION_PLUS_NUNCHUK:
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
