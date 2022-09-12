#pragma once

#include <common/console.hpp>
#include <common/message.hpp>
#include <common/module.hpp>

#include <utilities/driver/events.hpp>
#include <utilities/threads/threads.hpp>

#include <queue>

namespace love
{
    class Event : public Module
    {
      public:
        ModuleType GetModuleType() const override
        {
            return M_EVENT;
        }

        const char* GetName() const override
        {
            return "love.event";
        }

        void Push(Message* message);

        bool Poll(Message*& message);

        void Clear();

        void Pump();

        Message* Wait();

        void InternalClear();

        Message* Convert(const LOVE_Event& event);

        Message* ConvertJoystickEvent(const LOVE_Event& event, std::vector<Variant>& args);

        Message* ConvertTouchEvent(const LOVE_Event& event, std::vector<Variant>& args);

        Message* ConvertWindowEvent(const LOVE_Event& event)
        {
            return nullptr;
        }

      protected:
        std::queue<Message*> queue;
        LOVE_Event event;
        love::mutex mutex;
    };
} // namespace love
