#include "common/Data.hpp"

namespace love
{
    class SystemFontBase : public Data
    {
      public:
        SystemFontBase(uint32_t type = 0) : type(type)
        {}

        virtual ~SystemFontBase()
        {}

      protected:
        uint32_t type;
    };
} // namespace love
