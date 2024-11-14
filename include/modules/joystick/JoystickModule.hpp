#pragma once

#include "common/Module.hpp"
#include "modules/joystick/Joystick.tcc"

#include <list>
#include <map>
#include <string>
#include <vector>

namespace love
{
    class JoystickModule : public Module
    {
      public:
        JoystickModule();

        virtual ~JoystickModule();

        JoystickBase* addJoystick(int64_t deviceId);

        void removeJoystick(JoystickBase* joystick);

        JoystickBase* getJoystickFromID(int instanceId);

        JoystickBase* getJoystick(int index);

        int getIndex(const JoystickBase* joystick);

        int getJoystickCount() const;

      private:
        void checkGamepads(const std::string& guid) const;

        std::string getDeviceGUID(int64_t deviceId) const;

        std::vector<JoystickBase*> activeSticks;
        std::list<JoystickBase*> joysticks;

        std::map<std::string, bool> recentGamepadGUIDs;
    };

    namespace joystick
    {
        /*
        ** Return the number of connected joysticks.
        ** This is implemented in the platform-specific code.
        */
        int getJoystickCount();

        JoystickBase* openJoystick(int index);
    } // namespace joystick
} // namespace love
