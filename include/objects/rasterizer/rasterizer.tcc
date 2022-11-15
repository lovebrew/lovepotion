#pragma once

#include <common/console.hpp>
#include <common/exception.hpp>

#include <common/object.hpp>

#include <objects/glyphdata/glyphdata.hpp>

#include <utf8.h>

namespace love
{
    using Utf8Iterator = utf8::iterator<std::string_view::const_iterator>;

    template<Console::Platform T = Console::ALL>
    class Rasterizer : public Object
    {
      public:
        struct FontMetrics
        {
            int advance;
            int ascent;
            int descent;
            int height;
        };

        enum DataType
        {
            DATA_TRUETYPE,
            DATA_IMAGE,
            DATA_BCFNT
        };

        static inline Type type = Type("Rasterizer", &Object::type);

        virtual ~Rasterizer()
        {}

        int GetAdvance() const
        {
            return this->metrics.advance;
        }

        int GetAscent() const
        {
            return this->metrics.ascent;
        }

        int GetDescent() const
        {
            return this->metrics.descent;
        }

        int GetHeight() const
        {
            return this->metrics.height;
        }

        float GetKerning(uint32_t /* left */, uint32_t /* right */) const
        {
            return 0.0f;
        }

        float GetDPIScale() const
        {
            return this->dpiScale;
        }

        DataType GetDataType() const
        {
            return this->dataType;
        }

      protected:
        FontMetrics metrics;
        float dpiScale;
        DataType dataType;
    };
} // namespace love
