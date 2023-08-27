#pragma once

#include <common/color.hpp>
#include <common/console.hpp>
#include <common/exception.hpp>
#include <common/math.hpp>
#include <common/strongreference.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <utilities/driver/renderer/renderstate.hpp>
#include <utilities/driver/renderer/samplerstate.hpp>
#include <utilities/driver/renderer/vertex.hpp>

#include <objects/rasterizer/rasterizer.tcc>
#include <objects/texture/texture.tcc>

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

        template<typename Tex = Texture<Console::Which>, size_t N = 4>
        struct Glyph
        {
            Tex* texture;
            int spacing;
            std::array<vertex::Vertex, N> vertices;

            int sheet;
        };

        template<typename Tex = Texture<Console::Which>>
        struct DrawCommand
        {
            Tex* texture;
            int start;
            int count;

            int sheet;
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
        static void GetCodepointsFromString(const ColoredStrings& strings, ColoredCodepoints& out)
        {
            if (strings.empty())
                return;

            out.codepoints.reserve(strings[0].string.size());

            for (const auto& coloredString : strings)
            {
                if (coloredString.string.size() == 0)
                    continue;

                IndexedColor color = { coloredString.color, (int)out.codepoints.size() };
                out.colors.push_back(color);

                Font::GetCodepointsFromString(coloredString.string, out.codepoints);
            }

            if (out.colors.size() == 1)
            {
                IndexedColor color = out.colors[0];
                if (color.index == 0 && color.color == Color(Color::WHITE))
                    out.colors.pop_back();
            }
        }

        static void GetCodepointsFromString(std::string_view text, Codepoints& out)
        {
            out.reserve(text.size());

            try
            {
                Utf8Iterator start(text.begin(), text.begin(), text.end());
                Utf8Iterator end(text.end(), text.begin(), text.end());

                while (start != end)
                {
                    auto glyph = *start++;
                    out.push_back(glyph);
                }
            }
            catch (utf8::exception& exception)
            {
                throw love::Exception("UTF-8 decoding error: %s", exception.what());
            }
        }

        static constexpr auto SPACES_PER_TAB  = 0x04;
        static constexpr uint32_t TAB_GLYPH   = 9;
        static constexpr uint32_t SPACE_GLYPH = 32;

        static constexpr uint32_t NEWLINE_GLYPH  = 10;
        static constexpr uint32_t CARRIAGE_GLYPH = 13;

        float lineHeight;
        float height;

        SamplerState samplerState;
        float dpiScale;
        bool useSpacesAsTab;
        std::vector<Rasterizer<Console::Which>*> rasterizers;

        PixelFormat format;
    };
} // namespace love
