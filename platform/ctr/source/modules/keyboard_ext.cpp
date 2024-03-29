#include <modules/keyboard_ext.hpp>
#include <utilities/driver/hid_ext.hpp>

using namespace love;

Keyboard<Console::CTR>::Keyboard() :
    Keyboard<Console::ALL>(this->GetMaxEncodingLength(MAX_INPUT_LENGTH * 3) + 1),
    state {},
    showing(false)
{}

void Keyboard<Console::CTR>::SetTextInput(const KeyboardOptions& options)
{
    uint32_t maxLength = this->GetMaxEncodingLength(options.maxLength);
    this->text         = std::make_unique<char[]>(maxLength);

    const auto type = (SwkbdType)options.type;

    swkbdInit(&this->state, type, 2, maxLength);
    swkbdSetInitialText(&this->state, this->text.get());

    swkbdSetHintText(&this->state, options.hint.data());

    if (options.isPassword)
        swkbdSetPasswordMode(&this->state, SWKBD_PASSWORD_HIDE_DELAY);

    this->showing     = true;
    const auto button = swkbdInputText(&this->state, this->text.get(), maxLength);

    if (button != SWKBD_BUTTON_LEFT)
        HID<Console::CTR>::Instance().SendTextInput(this->GetText());
}
