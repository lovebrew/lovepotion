#include "modules/keyboard/keyboardc.h"
#include "common/bidirectionalmap.h"
#include "common/debug/logger.h"

using namespace love::common;

Keyboard::Keyboard(uint32_t swkbdMaxLength) : text(nullptr)
{
    try
    {
        this->text = new char[swkbdMaxLength];
    }
    catch (const std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }
}

Keyboard::~Keyboard()
{
    delete[] this->text;
}

const uint32_t Keyboard::CalculateEncodingMaxLength(const uint32_t in)
{
    return in * this->ENCODING_MULTIPLIER() + 1;
}

// clang-format off
constexpr auto keyboardOptions = BidirectionalMap<>::Create(
    "type",      Keyboard::KeyboardOption::OPTION_TYPE,
    "password",  Keyboard::KeyboardOption::OPTION_PASSCODE,
    "hint",      Keyboard::KeyboardOption::OPTION_HINT,
    "maxLength", Keyboard::KeyboardOption::OPTION_MAX_LEN
);
// clang-format on

bool Keyboard::GetConstant(const char* in, KeyboardOption& out)
{
    return keyboardOptions.Find(in, out);
}

bool Keyboard::GetConstant(KeyboardOption in, const char*& out)
{
    return keyboardOptions.ReverseFind(in, out);
}

std::vector<const char*> Keyboard::GetConstants(KeyboardOption)
{
    return keyboardOptions.GetNames();
}

const char* Keyboard::GetConstant(KeyboardOption in)
{
    const char* name = nullptr;
    Keyboard::GetConstant(in, name);

    return name;
}
