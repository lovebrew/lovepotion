#include "common/Module.hpp"
#include "common/debug.hpp"
#include "driver/EventQueue.hpp"

#include "modules/joystick/JoystickModule.hpp"

namespace love
{
#if defined(__WIIU__)
    #include <padscore/wpad.h>
    void extensionCallback(WPADChan channel, WPADExtensionType extension)
    {
        auto* instance = Module::getInstance<JoystickModule>(Module::M_JOYSTICK);
        LOG("ExtensionCallback: channel=%d, extension=%d", channel, extension);
        instance->addJoystick(channel + 1);
    }

    void connectCallback(WPADChan channel, WPADError error)
    {
        LOG("ConnectCallback: channel=%d, error=%d", channel, error);
        EventQueue::getInstance().sendJoystickStatus(false, channel + 1);
    }
#endif

    JoystickModule::JoystickModule() : Module(M_JOYSTICK, "love.joystick")
    {
        for (size_t index = 0; index < (size_t)joystick::getJoystickCount(); index++)
        {
            this->addJoystick(index);
            EventQueue::getInstance().sendJoystickStatus(true, index);
        }

#if defined(__WIIU__)
        for (size_t channel = 0; channel < 4; channel++)
            WPADSetExtensionCallback((WPADChan)channel, extensionCallback);

        for (size_t channel = 0; channel < 4; channel++)
            WPADSetConnectCallback((WPADChan)channel, connectCallback);
#endif
    }

    JoystickModule::~JoystickModule()
    {
        for (JoystickBase* joystick : this->joysticks)
        {
            joystick->close();
            joystick->release();
        }
    }

    JoystickBase* JoystickModule::getJoystick(int index)
    {
        if (index < 0 || (size_t)index >= this->activeSticks.size())
            return nullptr;

        return this->activeSticks[index];
    }

    int JoystickModule::getIndex(const JoystickBase* joystick)
    {
        for (int i = 0; i < (int)this->activeSticks.size(); i++)
        {
            if (this->activeSticks[i] == joystick)
                return i;
        }

        return -1;
    }

    int JoystickModule::getJoystickCount() const
    {
        return (int)this->activeSticks.size();
    }

    JoystickBase* JoystickModule::getJoystickFromID(int instanceId)
    {
        for (auto* joystick : this->joysticks)
        {
            if (joystick->getInstanceID() == instanceId)
                return joystick;
        }

        return nullptr;
    }

    JoystickBase* JoystickModule::addJoystick(int64_t deviceId)
    {
        if (deviceId < 0 || (int)deviceId >= joystick::getJoystickCount())
            return nullptr;

        std::string guid       = this->getDeviceGUID(deviceId);
        JoystickBase* joystick = nullptr;
        bool reused            = false;

        for (auto* stick : this->joysticks)
        {
            if (!stick->isConnected() && stick->getGUID() == guid)
            {
                joystick = stick;
                reused   = true;
                break;
            }
        }
        LOG("addJoystick: deviceId=%lld, guid=%s, reused=%d", deviceId, guid.c_str(), reused);
        LOG("addJoystick: %p", joystick);
        if (!joystick)
        {
            joystick = love::joystick::openJoystick(this->joysticks.size());
            this->joysticks.push_back(joystick);
        }

        this->removeJoystick(joystick);

        if (!joystick->open(deviceId))
            return nullptr;
        LOG("addJoystick: open success, handle=%p", (void*)joystick->getHandle());
        for (auto* activeStick : this->activeSticks)
        {
            if (joystick->getHandle() == activeStick->getHandle())
            {
                joystick->close();
                if (!reused)
                {
                    this->joysticks.remove(joystick);
                    joystick->release();
                }
                LOG("addJoystick: handle conflict, returning active stick");
                return activeStick;
            }
        }

        if (joystick->isGamepad())
            this->recentGamepadGUIDs[joystick->getGUID()] = true;
        LOG("addJoystick: new joystick, handle=%p", (void*)joystick->getHandle());
        this->activeSticks.push_back(joystick);
        return joystick;
    }

    void JoystickModule::removeJoystick(JoystickBase* joystick)
    {
        if (!joystick)
            return;

        auto iterator = std::find(this->activeSticks.begin(), this->activeSticks.end(), joystick);

        if (iterator != this->activeSticks.end())
        {
            (*iterator)->close();
            this->activeSticks.erase(iterator);
        }
    }

    std::string JoystickModule::getDeviceGUID(int64_t deviceId) const
    {
        int index = (int)deviceId;
        if (index < 0 || index >= (int)this->activeSticks.size())
            return std::string();

        return this->activeSticks[index]->getGUID();
    }
} // namespace love
