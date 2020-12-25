#include <3ds.h>
#include "modules/keyboard/keyboard.h"

using namespace love;

Keyboard::Keyboard()
{}

std::string Keyboard::SetTextInput(const Keyboard::SwkbdOpt & options)
{
    char text[options.maxLength] = {0};

    SwkbdType type = SWKBD_TYPE_QWERTY;

    if (options.type == "normal")
        type = SWKBD_TYPE_NORMAL;
    else if (options.type == "numpad")
        type = SWKBD_TYPE_NUMPAD;

    swkbdInit(&this->keyboard, type, 2, (int)options.maxLength);
    swkbdSetInitialText(&this->keyboard, text);

    swkbdSetHintText(&this->keyboard, options.hint.c_str());

    if (options.isPassword)
        swkbdSetPasswordMode(&this->keyboard, SWKBD_PASSWORD_HIDE_DELAY);

    swkbdInputText(&this->keyboard, text, options.maxLength);

    return text;
}
