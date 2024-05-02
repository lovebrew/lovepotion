#pragma once

#include "modules/graphics/vertex.hpp"

#include <3ds.h>
#include <citro3d.h>

namespace love
{
    enum TexEnvMode
    {
        TEXENV_MODE_PRIMITIVE,
        TEXENV_MODE_TEXTURE,
        TEXENV_MODE_FONT,
        TEXENV_MODE_MAX_ENUM
    };

    static TexEnvMode s_TexEnvMode = TEXENV_MODE_MAX_ENUM;

    void setTexEnvAttribute(TexEnvMode mode)
    {
        if (s_TexEnvMode == mode || mode == TEXENV_MODE_MAX_ENUM)
            return;

        switch (mode)
        {
            default:
            case TEXENV_MODE_PRIMITIVE:
            {
                C3D_TexEnv* env = C3D_GetTexEnv(0);
                C3D_TexEnvInit(env);

                C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);

                break;
            }
            case TEXENV_MODE_TEXTURE:
            {
                C3D_TexEnv* env = C3D_GetTexEnv(0);
                C3D_TexEnvInit(env);

                C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

                break;
            }
            case TEXENV_MODE_FONT:
            {
                C3D_TexEnv* env = C3D_GetTexEnv(0);
                C3D_TexEnvInit(env);

                C3D_TexEnvSrc(env, C3D_RGB, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);

                C3D_TexEnvSrc(env, C3D_Alpha, GPU_PRIMARY_COLOR, GPU_TEXTURE0, GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);

                break;
            }
        }

        s_TexEnvMode = mode;
    }
} // namespace love
