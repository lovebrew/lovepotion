#include <modules/keyboard_ext.hpp>

#include <utilities/result.hpp>

using namespace love;

Keyboard<Console::HAC>::Keyboard() : Keyboard<>(this->GetMaxEncodingLength(MAX_INPUT_LENGTH) + 1)
{}

std::string Keyboard<Console::HAC>::SetTextInput(const KeyboardOptions& options)
{
    R_UNLESS(swkbdCreate(&this->config, 0), std::string {});

    uint32_t maxLength = this->GetMaxEncodingLength(options.maxLength);
    this->text         = std::make_unique<char[]>(maxLength);

    auto type = (SwkbdType)options.type;
    swkbdConfigSetType(&this->config, type);

    swkbdConfigSetInitialCursorPos(&this->config, 1);
    swkbdConfigSetBlurBackground(&this->config, 1);

    swkbdConfigSetPasswordFlag(&this->config, options.isPassword);
    swkbdConfigSetStringLenMax(&this->config, maxLength);
    swkbdConfigSetDicFlag(&this->config, 1);

    swkbdConfigSetGuideText(&this->config, options.hint.data());

    R_UNLESS(swkbdShow(&this->config, this->text.get(), maxLength), std::string {});

    swkbdClose(&this->config);

    return this->text.get();
}
