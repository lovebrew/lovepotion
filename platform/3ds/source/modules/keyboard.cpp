#include "modules/keyboard/keyboard.h"
#include <3ds.h>

using namespace love;

Keyboard::Keyboard() : common::Keyboard((MAX_INPUT_LENGTH * 3) + 1)
{}

std::string Keyboard::SetTextInput(const Keyboard::SwkbdOpt& options)
{
    uint32_t maxLength = this->CalculateEncodingMaxLength(options.maxLength);
    memset(this->text, 0, maxLength);

    SwkbdType type = static_cast<SwkbdType>(options.type);

    swkbdInit(&this->keyboard, type, 2, maxLength);
    swkbdSetInitialText(&this->keyboard, text);

    swkbdSetHintText(&this->keyboard, options.hint.c_str());

    if (options.isPassword)
        swkbdSetPasswordMode(&this->keyboard, SWKBD_PASSWORD_HIDE_DELAY);

    SwkbdButton button = swkbdInputText(&this->keyboard, text, maxLength);

    if (button == SWKBD_BUTTON_NONE || button == SWKBD_BUTTON_LEFT)
        return std::string();

    return text;
}

bool Keyboard::GetConstant(const char* in, KeyboardType& out)
{
    return keyboardTypes.Find(in, out);
}

bool Keyboard::GetConstant(KeyboardType in, const char*& out)
{
    return keyboardTypes.Find(in, out);
}

std::vector<const char*> Keyboard::GetConstants(KeyboardType)
{
    return keyboardTypes.GetNames();
}

// clang-format off
constexpr StringMap<Keyboard::KeyboardType, Keyboard::MAX_TYPES>::Entry keyboardTypeEntries[] =
{
    { "normal", Keyboard::KeyboardType::TYPE_NORMAL },
    { "qwerty", Keyboard::KeyboardType::TYPE_QWERTY },
    { "numpad", Keyboard::KeyboardType::TYPE_NUMPAD }
};

constinit const StringMap<Keyboard::KeyboardType, Keyboard::MAX_TYPES> Keyboard::keyboardTypes(keyboardTypeEntries);
// clang-format on
