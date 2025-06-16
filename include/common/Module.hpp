#pragma once

#include "common/Exception.hpp"
#include "common/Object.hpp"

namespace love
{
    class Module : public Object
    {
      public:
        static Type type;

        enum ModuleType
        {
            M_UNKNOWN = -1, // Use this for modules outside of LOVE's source code.
            M_AUDIO,
            M_DATA,
            M_EVENT,
            M_FILESYSTEM,
            M_FONT,
            M_GRAPHICS,
            M_IMAGE,
            M_JOYSTICK,
            M_KEYBOARD,
            M_MATH,
            M_MOUSE,
            M_PHYSICS,
            M_SENSOR,
            M_SOUND,
            M_SYSTEM,
            M_THREAD,
            M_TIMER,
            M_TOUCH,
            M_VIDEO,
            M_WINDOW,
            M_MAX_ENUM
        };

        Module(ModuleType type, const char* name);

        virtual ~Module();

        ModuleType getModuleType() const
        {
            return this->moduleType;
        }

        const char* getName() const
        {
            return this->name.c_str();
        }

        static Module* getInstance(const std::string& name);

        template<typename T>
        static T* getInstance(ModuleType type)
        {
            return type != M_UNKNOWN ? (T*)instances[type] : nullptr;
        }

      private:
        static void registerInstance(Module* instance);

        ModuleType moduleType;
        std::string name;

        static Module* instances[M_MAX_ENUM];
    };
} // namespace love
