#include "driver/display/Attribute.hpp"

#include <gx2/utils.h>

namespace love
{
    static uint32_t getAttributeFormatSelector(int components)
    {
        static constexpr std::array masks = {
            GX2_SEL_MASK(GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_1),
            GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_1),
            GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_0, GX2_SQ_SEL_1),
            GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_Z, GX2_SQ_SEL_1),
            GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_Z, GX2_SQ_SEL_W)
        };

        return masks[components];
    }

    GX2Attribute::GX2Attribute() : stream {}, enabled(false)
    {}

    GX2Attribute::GX2Attribute(uint32_t location, int components, GX2AttribFormat format, uint32_t offset) :
        stream {},
        enabled(false)
    {
        this->update(location, components, format, offset);
    }

    void GX2Attribute::update(uint32_t location, int components, GX2AttribFormat format, uint32_t offset)
    {
        this->stream.location   = location;
        this->stream.buffer     = 0;
        this->stream.offset     = offset;
        this->stream.format     = format;
        this->stream.type       = GX2_ATTRIB_INDEX_PER_VERTEX;
        this->stream.aluDivisor = 0;
        this->stream.mask       = getAttributeFormatSelector(components);
        this->stream.endianSwap = GX2_ENDIAN_SWAP_DEFAULT;
    }

    void GX2Attribute::setEnabled(bool enabled)
    {
        this->enabled = enabled;
    }

    void GX2Attribute::setBuffer(uint32_t buffer)
    {
        this->stream.buffer = buffer;
    }

    void GX2Attribute::setDivisor(uint32_t divisor)
    {
        this->stream.aluDivisor = divisor;
    }

    bool GX2Attribute::operator==(const GX2Attribute& other)
    {
        return this->stream.location == other.stream.location && this->stream.buffer == other.stream.buffer &&
               this->stream.offset == other.stream.offset && this->stream.format == other.stream.format &&
               this->stream.type == other.stream.type && this->stream.mask == other.stream.mask &&
               this->stream.aluDivisor == other.stream.aluDivisor;
    }
} // namespace love
