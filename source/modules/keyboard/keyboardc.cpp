#include "modules/keyboard/keyboardc.h"
#include "common/debug/logger.h"

using namespace love::common;

Keyboard::Keyboard(uint32_t swkbdMaxLength) : text(nullptr)
{
    try
    {
        this->text = new char[swkbdMaxLength];
    }
    catch (const std::bad_alloc &)
    {
        throw love::Exception("Out of memory.");
    }
}

Keyboard::~Keyboard()
{
    delete [] this->text;
}

const uint32_t Keyboard::CalculateEncodingMaxLength(const uint32_t in)
{
    return in * this->ENCODING_MULTIPLIER() + 1;
}

bool Keyboard::GetConstant(const char * in, KeyboardOption & out)
{
    return keyboardOptions.Find(in, out);
}

bool Keyboard::GetConstant(KeyboardOption in, const char *& out)
{
    return keyboardOptions.Find(in, out);
}

std::vector<std::string> Keyboard::GetConstants(KeyboardOption)
{
    return keyboardOptions.GetNames();
}

const char * Keyboard::GetConstant(KeyboardOption in)
{
    const char * name = nullptr;
    Keyboard::GetConstant(in, name);

    return name;
}

StringMap<Keyboard::KeyboardOption, Keyboard::OPTION_MAX_ENUM>::Entry Keyboard::keyboardOptionsEntries[] =
{
    { "type",      Keyboard::OPTION_TYPE     },
    { "password",  Keyboard::OPTION_PASSCODE },
    { "hint",      Keyboard::OPTION_HINT     },
    { "maxLength", Keyboard::OPTION_MAX_LEN  }
};

StringMap<Keyboard::KeyboardOption, Keyboard::OPTION_MAX_ENUM> Keyboard::keyboardOptions(Keyboard::keyboardOptionsEntries, sizeof(Keyboard::keyboardOptionsEntries));