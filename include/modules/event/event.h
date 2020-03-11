/*
** modules/event.h
** @brief    : Handles events.. like quitting.. and other things
*/

#pragma once

#include "modules/joystick/joystick.h"

namespace love
{
    class Event : public Module
    {
        public:
            ModuleType GetModuleType() const { return M_EVENT; }

            const char * GetName() const override { return "love.event"; }

            void Clear();

            void Pump();

            void Push(Message * message);

            bool Poll(Message *& message);

            bool Quit();

        private:
            LOVE_Event event;
            std::queue<Message *> queue;
    };
}
