#include "modules/font/SystemFont.hpp"
#include "modules/font/freetype/Font.hpp"

namespace love
{
    SystemFont::SystemFont(uint32_t type) : SystemFontBase(type), data(nullptr), size(0)
    {
        std::string_view name {};
        FontModule::getConstant((OSSharedDataType)type, name);

        if (!OSGetSharedData((OSSharedDataType)type, 0, &this->data, &this->size))
            throw love::Exception("Failed to load system font: {:s}", name);
    }

    SystemFont::SystemFont(const SystemFont& other) : data(other.data), size(other.size)
    {}

    SystemFont::~SystemFont()
    {}
} // namespace love
