#pragma once

#include "common/Map.hpp"
#include "common/pixelformat.hpp"

#include "driver/display/Framebuffer.hpp"
#include "driver/display/Renderer.tcc"
#include "driver/display/Uniform.hpp"

#include "modules/graphics/vertex.hpp"
#include <gx2/enum.h>
#include <gx2/sampler.h>
#include <gx2/texture.h>

/* Enforces GLSL std140/std430 alignment rules for glm types */
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
/* Enables usage of SIMD CPU instructions (requiring the above as well) */
#define GLM_FORCE_INTRINSICS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace love
{
    class GX2 : public RendererBase
    {
      public:
        GX2();

        void initialize();

        void deInitialize();

        int onForegroundAcquired();

        int onForegroundReleased();

        void clear(const Color& color);

        void clearDepthStencil(int depth, uint8_t mask, double stencil);

        GX2ColorBuffer& getInternalBackbuffer();

        GX2DepthBuffer& getInternalDepthbuffer();

        void bindFramebuffer(GX2ColorBuffer* target);

        void present();

        void setBlendState(const BlendState& state);

        void setViewport(const Rect& viewport);

        void setScissor(const Rect& scissor);

        void setCullMode(CullMode mode);

        void setColorMask(ColorChannelMask mask);

        void setVertexWinding(Winding winding);

        void setSamplerState(TextureBase* texture, const SamplerState& state);

        void ensureInFrame();

        void prepareDraw(GraphicsBase* graphics);

        void bindTextureToUnit(TextureBase* texture, int unit);

        void bindTextureToUnit(GX2Texture* texture, GX2Sampler* sampler, int unit);

        void setMode(int width, int height);

        void copyCurrentScanBuffer();

        // clang-format off
        ENUMMAP_DECLARE(PixelFormats, PixelFormat, GX2SurfaceFormat,
            { PIXELFORMAT_R8_UNORM,         GX2_SURFACE_FORMAT_UNORM_R8          },
            { PIXELFORMAT_R16_UNORM,        GX2_SURFACE_FORMAT_UNORM_R16         },
            { PIXELFORMAT_RG8_UNORM,        GX2_SURFACE_FORMAT_UNORM_R8_G8       },
            { PIXELFORMAT_RGBA8_UNORM,      GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8 },
            { PIXELFORMAT_RGB565_UNORM,     GX2_SURFACE_FORMAT_UNORM_R5_G6_B5    },
            { PIXELFORMAT_RGBA8_sRGB,       GX2_SURFACE_FORMAT_SRGB_R8_G8_B8_A8  },
            { PIXELFORMAT_DXT1_UNORM,       GX2_SURFACE_FORMAT_UNORM_BC1         },
            { PIXELFORMAT_DXT3_UNORM,       GX2_SURFACE_FORMAT_UNORM_BC2         },
            { PIXELFORMAT_DXT5_UNORM,       GX2_SURFACE_FORMAT_UNORM_BC3         },
            { PIXELFORMAT_BC4_UNORM,        GX2_SURFACE_FORMAT_UNORM_BC4         },
            { PIXELFORMAT_BC5_UNORM,        GX2_SURFACE_FORMAT_UNORM_BC5         }
        );

        ENUMMAP_DECLARE(BlendOperations, BlendOperation, GX2BlendCombineMode,
            { BLENDOP_ADD,              GX2_BLEND_COMBINE_MODE_ADD     },
            { BLENDOP_SUBTRACT,         GX2_BLEND_COMBINE_MODE_SUB     },
            { BLENDOP_REVERSE_SUBTRACT, GX2_BLEND_COMBINE_MODE_REV_SUB },
            { BLENDOP_MIN,              GX2_BLEND_COMBINE_MODE_MIN     },
            { BLENDOP_MAX,              GX2_BLEND_COMBINE_MODE_MAX     }
        );

        ENUMMAP_DECLARE(BlendFactors, BlendFactor, GX2BlendMode,
            { BLENDFACTOR_ZERO,                GX2_BLEND_MODE_ZERO           },
            { BLENDFACTOR_ONE,                 GX2_BLEND_MODE_ONE            },
            { BLENDFACTOR_SRC_COLOR,           GX2_BLEND_MODE_SRC_COLOR      },
            { BLENDFACTOR_ONE_MINUS_SRC_COLOR, GX2_BLEND_MODE_INV_SRC_COLOR  },
            { BLENDFACTOR_SRC_ALPHA,           GX2_BLEND_MODE_SRC_ALPHA      },
            { BLENDFACTOR_ONE_MINUS_SRC_ALPHA, GX2_BLEND_MODE_INV_SRC_ALPHA  },
            { BLENDFACTOR_DST_COLOR,           GX2_BLEND_MODE_DST_COLOR      },
            { BLENDFACTOR_ONE_MINUS_DST_COLOR, GX2_BLEND_MODE_INV_DST_COLOR  },
            { BLENDFACTOR_DST_ALPHA,           GX2_BLEND_MODE_DST_ALPHA      },
            { BLENDFACTOR_ONE_MINUS_DST_ALPHA, GX2_BLEND_MODE_INV_DST_ALPHA  },
            { BLENDFACTOR_SRC_ALPHA_SATURATED, GX2_BLEND_MODE_SRC_ALPHA_SAT  }
        );

        ENUMMAP_DECLARE(WindingModes, Winding, GX2FrontFace,
            { WINDING_CW,  GX2_FRONT_FACE_CW  },
            { WINDING_CCW, GX2_FRONT_FACE_CCW }
        );

        ENUMMAP_DECLARE(ClampModes, SamplerState::WrapMode, GX2TexClampMode,
            { SamplerState::WRAP_CLAMP,           GX2_TEX_CLAMP_MODE_CLAMP         },
            { SamplerState::WRAP_CLAMP_ZERO,      GX2_TEX_CLAMP_MODE_CLAMP_BORDER  },
            { SamplerState::WRAP_REPEAT,          GX2_TEX_CLAMP_MODE_MIRROR        },
            { SamplerState::WRAP_MIRRORED_REPEAT, GX2_TEX_CLAMP_MODE_MIRROR        }
        );

        ENUMMAP_DECLARE(FilterModes, SamplerState::FilterMode, GX2TexXYFilterMode,
            { SamplerState::FILTER_NEAREST, GX2_TEX_XY_FILTER_MODE_POINT  },
            { SamplerState::FILTER_LINEAR,  GX2_TEX_XY_FILTER_MODE_LINEAR }
        );

        static GX2PrimitiveMode getPrimitiveType(PrimitiveType type)
        {
            switch (type)
            {
                case PRIMITIVE_TRIANGLES:
                    return GX2_PRIMITIVE_MODE_TRIANGLES;
                case PRIMITIVE_POINTS:
                    return GX2_PRIMITIVE_MODE_POINTS;
                case PRIMITIVE_TRIANGLE_STRIP:
                    return GX2_PRIMITIVE_MODE_TRIANGLE_STRIP;
                case PRIMITIVE_TRIANGLE_FAN:
                    return GX2_PRIMITIVE_MODE_TRIANGLE_FAN;
                default:
                    throw love::Exception("Invalid primitive type");
            }
        }

        static GX2IndexType getIndexType(IndexDataType type)
        {
            switch (type)
            {
                default:
                case INDEX_UINT16:
                    return GX2_INDEX_TYPE_U16;
                case INDEX_UINT32:
                    return GX2_INDEX_TYPE_U32;
            }
        }
        // clang-format on

      private:
        std::array<Framebuffer, 2> targets;

        void createFramebuffers();

        void destroyFramebuffers();

        GX2ColorBuffer* getFramebuffer();

        struct Context : public ContextBase
        {
            bool cullBack;
            bool cullFront;

            GX2FrontFace winding;
            bool depthWrite;
            bool depthTest;
            GX2CompareFunction compareMode;

            uint32_t writeMask;

            GX2ColorBuffer* boundFramebuffer = nullptr;
        } context;

        Uniform* uniform;

        bool inForeground;

        void* commandBuffer;
        GX2ContextState* state;
        bool dirtyProjection;
    };

    extern GX2 gx2;
} // namespace love
