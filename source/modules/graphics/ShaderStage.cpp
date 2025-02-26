#include "common/Exception.hpp"

#include "modules/graphics/ShaderStage.hpp"

namespace love
{
    ShaderStageBase::ShaderStageBase(ShaderStageType stage, const std::string& filepath) :
        filepath(filepath),
        stageType(stage)
    {}
} // namespace love
