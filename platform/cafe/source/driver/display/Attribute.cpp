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

    GX2Attribute::GX2Attribute() : stream {}
    {}

    GX2Attribute& GX2Attribute::setComponents(uint32_t components)
    {
        const auto mask   = getAttributeFormatSelector(components);
        this->stream.mask = mask;
        return *this;
    }
} // namespace love
