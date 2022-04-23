#pragma once

#include "common/exception.h"
#include "objects/object.h"

#include <map>

namespace love
{
    class Module : public Object
    {
      public:
        static love::Type type;

        enum ModuleType
        {
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
            M_PHYSICS,
            M_SYSTEM,
            M_SOUND,
            M_THREAD,
            M_TIMER,
            M_TOUCH,
            M_WINDOW,
            M_VIDEO,
            M_MAX_ENUM
        };

        virtual ~Module();

        virtual ModuleType GetModuleType() const = 0;
        virtual const char* GetName() const      = 0;

        static void RegisterInstance(Module* instance);

        Module* GetInstance(const std::string& name);

        template<typename T>
        static T* GetInstance(ModuleType type)
        {
            return (T*)instances[type];
        }

      private:
        static Module* instances[M_MAX_ENUM];
    };
} // namespace love
