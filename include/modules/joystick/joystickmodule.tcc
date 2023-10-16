#pragma once

#include <common/console.hpp>
#include <common/module.hpp>

#include <objects/joystick_ext.hpp>

#include <list>
#include <map>
#include <vector>

namespace love
{
    template<Console::Platform T = Console::ALL>
    class JoystickModule : public Module
    {
      public:
        JoystickModule()
        {}

        virtual ~JoystickModule()
        {
            for (auto joystick : this->active)
            {
                joystick->Close();
                joystick->Release();
            }
        }

        ModuleType GetModuleType() const override
        {
            return M_JOYSTICK;
        }

        const char* GetName() const override
        {
            return "love.joystick";
        }

        void RemoveJoystick(Joystick<love::Console::Which>* joystick)
        {
            if (!joystick)
                return;

            auto iterator = std::find(this->active.begin(), this->active.end(), joystick);

            if (iterator != this->active.end())
            {
                (*iterator)->Close();
                this->active.erase(iterator);
            }
        }

        Joystick<love::Console::Which>* GetJoystickFromId(int instanceId)
        {
            for (auto joystick : this->active)
            {
                if (joystick->GetInstanceID() == instanceId)
                    return joystick;
            }

            return nullptr;
        }

        Joystick<love::Console::Which>* GetJoystick(int index)
        {
            if (index < 0 || (size_t)index >= this->active.size())
                return nullptr;

            return this->active[index];
        }

        int GetIndex(const Joystick<love::Console::Which>* joystick)
        {
            for (size_t index = 0; index < this->active.size(); index++)
            {
                if (this->active[index] == joystick)
                    return index;
            }

            return -1;
        }

        int GetJoystickCount() const
        {
            return (int)this->active.size();
        }

      protected:
        std::vector<Joystick<love::Console::Which>*> active;
        std::list<Joystick<love::Console::Which>*> joysticks;

        std::map<std::string, bool> recentGUIDs;
    };
} // namespace love
