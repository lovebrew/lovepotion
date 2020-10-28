#include "common/runtime.h"
#include "deko3d/shaderstage.h"

using namespace love;

ShaderStage::ShaderStage(deko3d::Graphics * gfx, StageType stage,
                         const std::string & path) : stageType(stage),
                                                     path(path)
{
    this->dekoShader.load(*dk3d.GetCode(), this->path.c_str());
}

ShaderStage::ShaderStage(deko3d::Graphics * gfx, StageType stage,
                         void * data, size_t size) : stageType(stage)
{
    this->dekoShader.loadMemory(*dk3d.GetCode(), data, size);
}

ShaderStage::~ShaderStage()
{}

bool ShaderStage::GetConstant(const char *in, StageType & out)
{
    return stageNames.Find(in, out);
}

bool ShaderStage::GetConstant(StageType in, const char *& out)
{
    return stageNames.Find(in, out);
}

StringMap<ShaderStage::StageType, ShaderStage::STAGE_MAX_ENUM>::Entry ShaderStage::stageNameEntries[] =
{
    { "vertex", STAGE_VERTEX },
    { "pixel",  STAGE_PIXEL  },
};

StringMap<ShaderStage::StageType, ShaderStage::STAGE_MAX_ENUM> ShaderStage::stageNames(ShaderStage::stageNameEntries, sizeof(ShaderStage::stageNameEntries));