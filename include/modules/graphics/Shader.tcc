#pragma once

#include "common/Map.hpp"
#include "common/Object.hpp"
#include "common/StrongRef.hpp"

#include "modules/graphics/Resource.hpp"
#include "modules/graphics/ShaderStage.tcc"

#include <map>
#include <string>
#include <vector>

#include <stddef.h>

#if defined(__3DS__)
using Location = int8_t;
#elif defined(__SWITCH__)
using Location = uint8_t;
#else
using Location = uint32_t;
#endif

namespace love
{
    class ShaderBase : public Object, public Resource
    {
      public:
        static Type type;

        static int shaderSwitches;

        enum BuiltinUniform
        {
            BUILTIN_TEXTURE_MAIN,
            BUILTIN_TEXTURE_VIDEO_Y,
            BUILTIN_TEXTURE_VIDEO_CB,
            BUILTIN_TEXTURE_VIDEO_CR,
            BUILTIN_UNIFORMS_PER_DRAW,
            BUILTIN_UNIFORMS_MODEL_VIEW,
            BUILTIN_UNIFORMS_PROJECTION,
            BUILTIN_MAX_ENUM
        };

        struct CompileOptions
        {
            std::map<std::string, std::string> defines;
            std::string debugName;
        };

        enum UniformType
        {
            UNIFORM_FLOAT,
            UNIFORM_MATRIX,
            UNIFORM_INT,
            UNIFORM_UINT,
            UNIFORM_BOOL,
            UNIFORM_SAMPLER,
            UNIFORM_STORAGETEXTURE,
            UNIFORM_TEXELBUFFER,
            UNIFORM_STORAGEBUFFER,
            UNIFORM_UNKNOWN,
            UNIFORM_MAX_ENUM
        };

        enum StandardShader
        {
            STANDARD_DEFAULT,
            STANDARD_TEXTURE,
            STANDARD_VIDEO,
            STANDARD_MAX_ENUM
        };

        struct UniformInfo
        {
            UniformType type;
            uint32_t stageMask;
            bool active;

            Location location;
            int count;

            std::string name;
        };

        static ShaderBase* current;
        static ShaderBase* standardShaders[STANDARD_MAX_ENUM];

        ShaderBase(StrongRef<ShaderStageBase> stages[], const CompileOptions& options);

        virtual ~ShaderBase();

        bool hasStage(ShaderStageType stage);

        bool hasUniform(const std::string& name) const;

        const UniformInfo* getUniformInfo(const std::string& name) const;

        virtual void attach() = 0;

        static void attachDefault(StandardShader type);

        static bool isDefaultActive();

      protected:
        struct Reflection
        {
            std::map<std::string, UniformInfo*> uniforms;
            std::map<std::string, UniformInfo> localUniforms;
        };

        Reflection reflection;
        UniformInfo* builtinUniformInfo[BUILTIN_MAX_ENUM];

        std::array<StrongRef<ShaderStageBase>, SHADERSTAGE_MAX_ENUM> stages;
        std::string debugName;
    };
} // namespace love
