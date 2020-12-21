/*
** modules/event.h
** @brief    : Handles events.. like quitting.. and other things
*/

#pragma once

#include "modules/joystick/joystickc.h"

namespace love
{
    class Event : public Module
    {
        public:
            Event();
            ~Event();

            ModuleType GetModuleType() const { return M_EVENT; }

            const char * GetName() const override { return "love.event"; }

            void Clear();

            void Pump();

            void Push(Message * message);

            bool Poll(Message *& message);

            bool Quit();

        private:
            love::thread::MutexRef mutex;
            std::queue<Message *> queue;
    };
}
