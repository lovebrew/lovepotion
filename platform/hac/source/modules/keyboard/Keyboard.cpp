#include "modules/keyboard/Keyboard.hpp"
#include "driver/EventQueue.hpp"

namespace love
{
    Keyboard::Keyboard() : KeyboardBase(), config {}
    {}

    void Keyboard::setTextInput(const KeyboardOptions& options)
    {
        if (!Result(swkbdCreate(&this->config, 0)))
            throw love::Exception("Failed to create software keyboard.");

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
        swkbdConfigSetType(&this->config, type);

        swkbdConfigSetInitialCursorPos(&this->config, 1);
        swkbdConfigSetBlurBackground(&this->config, 1);

        swkbdConfigSetPasswordFlag(&this->config, options.password);
        swkbdConfigSetStringLenMax(&this->config, length);
        swkbdConfigSetDicFlag(&this->config, 1);

        swkbdConfigSetGuideText(&this->config, options.hint.data());

        this->showing = true;

        if (Result(swkbdShow(&this->config, this->text.get(), length)))
            EventQueue::getInstance().sendTextInput(this->text);

        swkbdClose(&this->config);
    }
} // namespace love
