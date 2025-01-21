#include "modules/graphics/ShaderStage.tcc"
#include "modules/graphics/Volatile.hpp"

#include "driver/display/deko3d/CMemPool.h"
#include <deko3d.hpp>

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
        CMemPool::Handle memory;
        dk::Shader shader;
    };
} // namespace love
