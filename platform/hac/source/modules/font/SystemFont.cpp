#include "common/Exception.hpp"

#include "modules/font/SystemFont.hpp"
#include "modules/font/freetype/Font.hpp"

namespace love
{
    SystemFont::SystemFont(uint32_t type) : SystemFontBase(type)
    {
        if (R_FAILED(plGetSharedFontByType(&this->data, (PlSharedFontType)type)))
            throw love::Exception("Failed to load Shared Font {:d}", (int)type);

        std::string_view name {};
        FontModule::getConstant((PlSharedFontType)type, name);

        if (this->data.address == nullptr)
            throw love::Exception("Error loading system font '{:s}'", name);
    }

    SystemFont::SystemFont(const SystemFont& other) : SystemFontBase(other.type), data(other.data)
    {}

    SystemFont::~SystemFont()
    {}
} // namespace love
