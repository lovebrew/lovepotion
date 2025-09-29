#pragma once

#include "modules/font/SystemFont.tcc"

#include <3ds.h>

namespace love
{
    class SystemFont final : public SystemFontBase
    {
      public:
        SystemFont(uint32_t type = 0);

        SystemFont(const SystemFont& other);

        ~SystemFont();

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
    };
} // namespace love
