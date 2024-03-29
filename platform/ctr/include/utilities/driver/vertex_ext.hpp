#pragma once

#include <utilities/driver/renderer/vertex.hpp>

#include <citro3d.h>

namespace love
{
    namespace vertex
    {
        namespace attributes
        {
            typedef void (*AttributeSetFunction)(void);

            enum TEXENV_MODE
            {
                TEXENV_MODE_PRIMITIVE,
                TEXENV_MODE_TEXTURE,
                TEXENV_MODE_TEXT,
                TEXENV_MODE_MAX_ENUM
            };

            void SetPrimitiveAttribute()
            {
                C3D_TexEnv* env = C3D_GetTexEnv(0);
                C3D_TexEnvInit(env);

                C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR,
                              GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
            }

            void SetTextureAttribute()
            {
                C3D_TexEnv* env = C3D_GetTexEnv(0);
                C3D_TexEnvInit(env);

                C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
            }

            void SetFontAttribute()
            {
                C3D_TexEnv* env = C3D_GetTexEnv(0);
                C3D_TexEnvInit(env);

                C3D_TexEnvSrc(env, C3D_RGB, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR,
                              GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);

                C3D_TexEnvSrc(env, C3D_Alpha, GPU_PRIMARY_COLOR, GPU_TEXTURE0, GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
            }

            static inline CommonFormat s_format = CommonFormat::NONE;

            static inline void SetTexEnvFunction(CommonFormat format)
            {
                if (format == s_format)
                    return;

                switch (format)
                {
                    case CommonFormat::PRIMITIVE:
                    default:
                    {
                        SetPrimitiveAttribute();
                        break;
                    }
                    case CommonFormat::TEXTURE:
                    {
                        SetTextureAttribute();
                        break;
                    }
                    case CommonFormat::FONT:
                    {
                        SetFontAttribute();
                        break;
                    }
                }

                s_format = format;
            }
        } // namespace attributes
    }     // namespace vertex
} // namespace love
