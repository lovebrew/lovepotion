#include "modules/keyboard/Keyboard.hpp"
#include "driver/EventQueue.hpp"

namespace love
{
    Keyboard::Keyboard() : KeyboardBase(), state {}
    {}

    void Keyboard::setTextInput(const KeyboardOptions& options)
    {
        const auto length = this->getMaxEncodingLength(options.maxLength);

        try
        {
            this->text = std::make_unique<char[]>(length);
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        const auto type = (SwkbdType)options.type;

        swkbdInit(&this->state, type, 2, length);
        swkbdSetInitialText(&this->state, this->text.get());
        swkbdSetHintText(&this->state, options.hint.data());

        if (options.password)
            swkbdSetPasswordMode(&this->state, SWKBD_PASSWORD_HIDE_DELAY);

        this->showing = true;

        const auto button = swkbdInputText(&this->state, this->text.get(), length);

        if (button != SWKBD_BUTTON_LEFT)
            EventQueue::getInstance().sendTextInput(this->text);
    }
} // namespace love
