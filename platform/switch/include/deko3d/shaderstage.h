#pragma once

#include "deko3d/CShader.h"
#include "common/stringmap.h"

#include "deko3d/deko.h"

namespace love
{
    namespace deko3d
    {
        class Graphics;
    }

    class ShaderStage : public Object
    {
        public:
            enum StageType
            {
                STAGE_VERTEX,
                STAGE_PIXEL,
                STAGE_MAX_ENUM
            };

            ShaderStage(deko3d::Graphics * gfx, StageType stage, const std::string & path);
            ~ShaderStage();

            StageType GetStageType() const { return stageType; }

            static bool GetConstant(const char * in, StageType & out);
            static bool GetConstant(StageType in, const char *& out);

        private:
            StageType stageType;
            CShader dekoShader;
            std::string path;

            static StringMap<StageType, STAGE_MAX_ENUM>::Entry stageNameEntries[];
            static StringMap<StageType, STAGE_MAX_ENUM> stageNames;
    };
}