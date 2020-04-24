#include "common/runtime.h"
#include "modules/keyboard/keyboard.h"

using namespace love;

Keyboard::Keyboard()
{

}

std::string Keyboard::SetTextInput(const Keyboard::SwkbdOpt & options)
{
    Result res = swkbdCreate(&this->keyboard, 0);

    if (R_FAILED(res))
        return NULL;

    char text[options.maxLength] = {0};

    if (options.type == "basic")
        swkbdConfigSetType(&this->keyboard, SwkbdType_Normal);
    else if (options.type == "standard")
        swkbdConfigSetType(&this->keyboard, SwkbdType_All);
    else if (options.type == "numpad")
        swkbdConfigSetType(&this->keyboard, SwkbdType_NumPad);

    swkbdConfigSetInitialCursorPos(&this->keyboard, 1);
    swkbdConfigSetBlurBackground(&this->keyboard, 1);

    if (options.isPassword)
        swkbdConfigSetPasswordFlag(&this->keyboard, 1);

    swkbdConfigSetDicFlag(&this->keyboard, 1);
    swkbdConfigSetStringLenMax(&this->keyboard, options.maxLength);

    swkbdConfigSetGuideText(&this->keyboard, options.hint.c_str());

    res = swkbdShow(&this->keyboard, text, sizeof(text));

    if (R_SUCCEEDED(res))
        swkbdClose(&this->keyboard);

    return text;
}
