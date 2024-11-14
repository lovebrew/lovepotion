#pragma once

#include "common/Module.hpp"
#include "common/StrongRef.hpp"
#include "common/int.hpp"

#include "modules/filesystem/FileData.hpp"
#include "modules/font/Rasterizer.hpp"
#include "modules/image/ImageData.hpp"

#include <string>
#include <vector>

namespace love
{
    class SystemFont : public Data
    {
      public:
        static inline Type type = Type("SystemFont", &Data::type);

        SystemFont(uint32_t dataType);

        virtual ~SystemFont();

        SystemFont(const SystemFont& other) : data(other.data), size(other.size), dataType(other.dataType)
        {}

        SystemFont* clone() const override
        {
            return new SystemFont(*this);
        }

        void* getData() const override
        {
            return this->data;
        }

        size_t getSize() const override
        {
            return this->size;
        }

      private:
        void* data;
        size_t size;

        uint32_t dataType;
    };

    class FontModuleBase : public Module
    {
      public:
        FontModuleBase(const char* name) : Module(M_FONT, name)
        {}

        virtual ~FontModuleBase()
        {}

        virtual Rasterizer* newRasterizer(FileData* data) const = 0;

        Rasterizer* newTrueTypeRasterizer(int size, const Rasterizer::Settings& settings) const;

        virtual Rasterizer* newTrueTypeRasterizer(Data* data, int size,
                                                  const Rasterizer::Settings& settings) const = 0;

        virtual GlyphData* newGlyphData(Rasterizer* rasterizer, const std::string& glyph) const;

        virtual GlyphData* newGlyphData(Rasterizer* rasterizer, uint32_t glyph) const;

      protected:
        StrongRef<Data> defaultFontData;
    };
} // namespace love
