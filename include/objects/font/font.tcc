#pragma once

#include <common/color.hpp>
#include <common/console.hpp>
#include <common/exception.hpp>
#include <common/math.hpp>
#include <common/strongreference.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <utilities/driver/renderer/renderstate.hpp>
#include <utilities/driver/renderer/samplerstate.hpp>

#include <modules/graphics/graphics.tcc>

#include <objects/rasterizer/rasterizer.tcc>

#include <vector>

namespace love
{
    using StrongRasterizer = StrongReference<Rasterizer<Console::Which>>;

    template<Console::Platform T = Console::ALL>
    class Font : public Object
    {
      public:
        enum AlignMode
        {
            ALIGN_LEFT,
            ALIGN_CENTER,
            ALIGN_RIGHT,
            ALIGN_JUSTIFY
        };

        struct ColoredString
        {
            std::string string;
            Color color;
        };

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

        Font() : lineHeight(1.0f), dpiScale(1.0f)
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

        int GetAscent() const
        {
            return std::floor(this->rasterizers[0]->GetAscent() / this->dpiScale + 0.5f);
        }

        int GetDescent() const
        {
            return std::floor(this->rasterizers[0]->GetDescent() / this->dpiScale + 0.5f);
        }

        float GetHeight() const
        {
            return this->height;
        }

        float GetBaseline() const
        {
            float ascent = this->GetAscent();

            if (ascent != 0.0f)
                return ascent;
            else if (this->rasterizers[0]->GetDataType() ==
                     Rasterizer<Console::Which>::DATA_TRUETYPE)
                return std::floor(this->GetHeight() / 1.25f + 0.5f);

            return 0.0f;
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

        std::vector<StrongRasterizer> rasterizers;
    };
} // namespace love
