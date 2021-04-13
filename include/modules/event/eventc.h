/*
** modules/event.h
** @brief    : Handles events.. like quitting.. and other things
*/

#pragma once

#include "common/module.h"

#include "common/message.h"

#include "modules/joystick/joystick.h"

#include "modules/thread/types/lock.h"
#include "modules/thread/types/mutex.h"

#include "driver/hidrv.h"

#include <memory>
#include <queue>
#include <vector>

struct LOVE_Event;

namespace love::driver
{
    class Hidrv;
}

namespace love::common
{
    class Event : public Module
    {
      public:
        Event();

        ModuleType GetModuleType() const
        {
            return M_EVENT;
        }

        const char* GetName() const override
        {
            return "love.event";
        }

        void InternalClear();

        void Pump();

        Message* Wait();

        void Clear();

        void Push(Message* message);

        bool Poll(Message*& message);

        void ExceptionIfInRenderPass(const char* name);

        std::unique_ptr<love::driver::Hidrv>& GetDriver();

      protected:
        love::thread::MutexRef mutex;

        std::unique_ptr<love::driver::Hidrv> driver;

      private:
        std::queue<Message*> queue;

        Message* Convert(const driver::Hidrv::LOVE_Event& event);

        Message* ConvertJoystickEvent(const driver::Hidrv::LOVE_Event& event) const;

        Message* ConvertWindowEvent(const driver::Hidrv::LOVE_Event& event);
    };
} // namespace love::common
