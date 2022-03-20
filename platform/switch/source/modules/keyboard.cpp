#include "modules/keyboard/keyboard.h"
#include "common/bidirectionalmap.h"
#include <switch.h>

using namespace love;

Keyboard::Keyboard() : common::Keyboard((MAX_INPUT_LENGTH * 4) + 1)
{}

std::string Keyboard::SetTextInput(const Keyboard::SwkbdOpt& options)
{
    Result res = swkbdCreate(&this->keyboard, 0);

    if (R_FAILED(res))
        return std::string();

    uint32_t maxLength = this->CalculateEncodingMaxLength(options.maxLength);
    memset(this->text, 0, maxLength);

    /* set the software keyboard type */

    swkbdConfigSetType(&this->keyboard, static_cast<SwkbdType>(options.type));

    /* generic config things */

    swkbdConfigSetInitialCursorPos(&this->keyboard, 1);
    swkbdConfigSetBlurBackground(&this->keyboard, 1);

    /* password flag */

    swkbdConfigSetPasswordFlag(&this->keyboard, options.isPassword);

    /* allow the dictionary to be used */

    swkbdConfigSetDicFlag(&this->keyboard, 1);

    /* set max input length */

    swkbdConfigSetStringLenMax(&this->keyboard, maxLength);

    /* set the hint text */

    swkbdConfigSetGuideText(&this->keyboard, options.hint.c_str());

    /* show the software keyboard with the text */

    res = swkbdShow(&this->keyboard, text, maxLength);

    if (R_SUCCEEDED(res))
        swkbdClose(&this->keyboard);

    return text;
}

// clang-format off
constexpr auto keyboardTypes = BidirectionalMap<>::Create(
    "normal", Keyboard::KeyboardType::TYPE_NORMAL,
    "qwerty", Keyboard::KeyboardType::TYPE_QWERTY,
    "numpad", Keyboard::KeyboardType::TYPE_NUMPAD
);
// clang-format on

bool Keyboard::GetConstant(const char* in, KeyboardType& out)
{
    return keyboardTypes.Find(in, out);
}

bool Keyboard::GetConstant(KeyboardType in, const char*& out)
{
    return keyboardTypes.ReverseFind(in, out);
}

std::vector<const char*> Keyboard::GetConstants(KeyboardType)
{
    return keyboardTypes.GetNames();
}
