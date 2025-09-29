#include "modules/font/SystemFont.hpp"
#include "modules/font/Font.hpp"

namespace love
{
    SystemFont::SystemFont(uint32_t type) : SystemFontBase(type)
    {
        this->data = FontModule::loadSystemFont((CFG_Region)type, this->size);

        std::string_view name {};
        FontModule::getConstant((CFG_Region)type, name);

        if (this->data == nullptr)
            throw love::Exception("Error loading system font of type {:s}", name);
    }

    SystemFont::SystemFont(const SystemFont& other) :
        SystemFontBase(other.type),
        data(other.data),
        size(other.size)
    {}

    SystemFont::~SystemFont()
    {
        if (this->data != nullptr)
            linearFree(this->data);
    }
} // namespace love
