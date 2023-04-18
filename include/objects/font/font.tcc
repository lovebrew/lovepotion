#pragma once

#include <common/color.hpp>
#include <common/console.hpp>
#include <common/exception.hpp>
#include <common/math.hpp>
#include <common/strongreference.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <utilities/driver/renderer/renderstate.hpp>
#include <utilities/driver/renderer/samplerstate.hpp>

#include <objects/rasterizer/rasterizer.tcc>

#include <vector>

namespace love
{
    using StrongRasterizer = StrongReference<Rasterizer<Console::Which>>;

    template<Console::Platform T = Console::ALL>
    class Font : public Object
    {
      public:
        static inline Type type = Type("Font", &Object::type);

        using Codepoints = std::vector<uint32_t>;

        enum AlignMode
        {
            ALIGN_LEFT,
            ALIGN_CENTER,
            ALIGN_RIGHT,
            ALIGN_JUSTIFY,
            ALIGN_MAX_ENUM
        };

        struct ColoredString
        {
            std::string string;
            Color color;
        };

        using ColoredStrings = std::vector<ColoredString>;

        struct IndexedColor
        {
            Color color;
            int index;
        };

        struct ColoredCodepoints
        {
            std::vector<uint32_t> codepoints;
            std::vector<IndexedColor> colors;
        };

        struct TextInfo
        {
            int width;
            int height;
        };

        static inline int fontCount = 0;

        Font() : lineHeight(1.0f), samplerState {}, dpiScale(1.0f), useSpacesAsTab(false)
        {
            Font::fontCount++;
        }

        virtual ~Font()
        {
            Font::fontCount--;
        }

        void SetLineHeight(float height)
        {
            this->lineHeight = height;
        }

        float GetLineHeight() const
        {
            return this->lineHeight;
        }

        void SetSamplerState(const SamplerState& state)
        {
            this->samplerState.minFilter     = state.minFilter;
            this->samplerState.magFilter     = state.magFilter;
            this->samplerState.maxAnisotropy = state.maxAnisotropy;
        }

        const SamplerState& GetSamplerState() const
        {
            return this->samplerState;
        }

        float GetDPIScale() const
        {
            return this->dpiScale;
        }

        // clang-format off
        static constexpr BidirectionalMap alignModes = {
            "left",    ALIGN_LEFT,
            "center",  ALIGN_CENTER,
            "right",   ALIGN_RIGHT,
            "justify", ALIGN_JUSTIFY
        };
        // clang-format on

      protected:
        float lineHeight;
        float height;

        SamplerState samplerState;
        float dpiScale;
        bool useSpacesAsTab;

        PixelFormat format;
    };
} // namespace love
