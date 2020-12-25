/*
** modules/event.h
** @brief    : Handles events.. like quitting.. and other things
*/

#pragma once

#include "common/module.h"

#include "common/message.h"

#include "modules/joystick/joystick.h"

#include "modules/thread/types/mutex.h"
#include "modules/thread/types/lock.h"

#include <queue>
#include <vector>

struct LOVE_Event;

namespace love
{
    class Event : public Module
    {
        public:
            Event();

            ~Event();

            ModuleType GetModuleType() const { return M_EVENT; }

            const char * GetName() const override { return "love.event"; }

            void InternalClear();

            void Pump();

            Message * Wait();

            void Clear();

            void Push(Message * message);

            bool Poll(Message *& message);

            void ExceptionIfInRenderPass(const char * name);


        private:
            love::thread::MutexRef mutex;
            std::queue<Message *> queue;

            Message * Convert(const LOVE_Event & event);

            Message * ConvertJoystickEvent(const LOVE_Event & event) const;

            Message * ConvertWindowEvent(const LOVE_Event & event);
    };
}
