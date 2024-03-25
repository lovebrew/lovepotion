#include "driver/EventQueue.hpp"

#include "modules/event/Event.hpp"

#include <mutex>

namespace love
{
    Event::Event() : Module(M_EVENT, "love.event"), event {}
    {}

    Event::~Event()
    {}

    // #region Message Conversion

    static Message* convertGeneralEvent(const LOVE_Event& event)
    {
        Message* result = nullptr;

        switch (event.subtype)
        {
            case SUBTYPE_QUIT:
            {
                result = new Message("quit");
                break;
            }
            case SUBTYPE_LOWMEMORY:
            {
                result = new Message("lowmemory");
                break;
            }
            default:
                break;
        }

        return result;
    }

    static Message* convertWindowEvent(const LOVE_Event& event, std::vector<Variant>& args)
    {
        Message* result = nullptr;

        switch (event.subtype)
        {
            case SUBTYPE_FOCUS_GAINED:
            case SUBTYPE_FOCUS_LOST:
            {
                args.emplace_back(event.subtype == SUBTYPE_FOCUS_GAINED);
                result = new Message("focus", args);
                break;
            }
            default:
                break;
        }

        return result;
    }

    static Message* convertTouchEvent(const LOVE_Event& event, std::vector<Variant>& args)
    {
        Message* result = nullptr;

        return result;
    }

    static Message* convert(const LOVE_Event& event)
    {
        Message* message = nullptr;

        std::vector<Variant> args;
        args.reserve(4);

        switch (event.type)
        {
            case TYPE_GENERAL:
            {
                message = convertGeneralEvent(event);
                break;
            }
            case TYPE_TOUCH:
            {
                message = convertTouchEvent(event, args);
                break;
            }
            default:
                break;
        }

        return message;
    }

    // #endregion

    void Event::push(Message* message)
    {
        std::unique_lock lock(this->mutex);
        message->retain();

        this->messages.push(message);
    }

    bool Event::poll(Message*& message)
    {
        std::unique_lock lock(this->mutex);

        if (this->messages.empty())
            return false;

        message = this->messages.front();
        this->messages.pop();

        return true;
    }

    void Event::clear()
    {
        std::unique_lock lock(this->mutex);

        while (!this->messages.empty())
        {
            this->messages.front()->release();
            this->messages.pop();
        }
    }

    void Event::pump()
    {
        while (EventQueue::getInstance().poll(&this->event))
        {
            Message* message = convert(this->event);

            if (message)
            {
                this->push(message);
                message->release();
            }
        }
    }

    Message* Event::wait()
    {
        LOVE_Event event {};

        if (EventQueue::getInstance().poll(&event))
            return convert(event);

        return nullptr;
    }
} // namespace love
