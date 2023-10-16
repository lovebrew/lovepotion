#include <modules/keyboard_ext.hpp>
#include <utilities/driver/hid_ext.hpp>

using namespace love;

Keyboard<Console::HAC>::Keyboard() :
    Keyboard<>(this->GetMaxEncodingLength(MAX_INPUT_LENGTH) + 1),
    config {},
    showing(false)
{}

void Keyboard<Console::HAC>::SetTextInput(const KeyboardOptions& options)
{
    if (R_FAILED(swkbdCreate(&this->config, 0)))
        return;

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

    this->showing = true;
    if (R_SUCCEEDED(swkbdShow(&this->config, this->text.get(), maxLength)))
        HID<Console::HAC>::Instance().SendTextInput(this->GetText());

    swkbdClose(&this->config);
}
