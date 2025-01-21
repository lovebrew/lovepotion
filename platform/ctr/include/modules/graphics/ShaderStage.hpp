#pragma once

#include "modules/graphics/ShaderStage.tcc"
#include "modules/graphics/Volatile.hpp"

#include <3ds.h>

namespace love
{
    class ShaderStage final : public ShaderStageBase, public Volatile
    {
      public:
        ShaderStage(ShaderStageType stage, const std::string& filepath);

        virtual ~ShaderStage();

        ptrdiff_t getHandle() const override;

        bool loadVolatile() override;

        void unloadVolatile() override;

      private:
        std::vector<uint32_t> code;
        DVLB_s* dvlb;
    };
} // namespace love
