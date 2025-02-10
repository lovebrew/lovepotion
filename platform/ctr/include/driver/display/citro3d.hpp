#pragma once

#include <citro3d.h>

#include "common/Map.hpp"
#include "common/pixelformat.hpp"

#include "driver/display/Framebuffer.hpp"
#include "driver/display/Renderer.tcc"

#include "modules/graphics/renderstate.hpp"
#include "modules/graphics/samplerstate.hpp"

#include "driver/graphics/DrawCommand.hpp"

using C3D_IndexType = decltype(C3D_UNSIGNED_BYTE);

namespace love
{

    class citro3d : public RendererBase
    {
      public:
        enum TexEnvMode
        {
            TEXENV_MODE_PRIMITIVE,
            TEXENV_MODE_TEXTURE,
            TEXENV_MODE_FONT,
            TEXENV_MODE_MAX_ENUM
        };

        citro3d();

        void initialize();

        void setupContext();

        void deInitialize();

        void clear(const Color& color);

        void clearDepthStencil(int depth, uint8_t mask, double stencil);

        C3D_RenderTarget* getFramebuffer();

        void bindFramebuffer(C3D_RenderTarget* framebuffer);

        void present();

        void setBlendState(const BlendState& state);

        void setViewport(int width, int height, bool tilt);

        void setScissor(const Rect& scissor);

        void setCullMode(CullMode mode);

        void setColorMask(ColorChannelMask mask);

        void setVertexWinding(Winding winding);

        void setSamplerState(C3D_Tex* texture, SamplerState state);

        virtual void prepareDraw(GraphicsBase* graphics) override;

        void setVertexAttributes(TextureBase* texture, bool isFont);

        void bindTextureToUnit(TextureType target, C3D_Tex* texture, int unit);

        void bindTextureToUnit(TextureBase* texture, int unit);

        C3D_RenderTarget* getInternalBackbuffer() const;

        void setWideMode(bool wide)
        {
            this->modeChanged([this, wide]() { gfxSetWide(wide); });
        }

        bool isWideMode() const
        {
            return gfxIsWide();
        }

        void set3DMode(bool enable)
        {
            this->modeChanged([this, enable]() { gfxSet3D(enable); });
        }

        bool is3DMode() const
        {
            return gfxIs3D();
        }

        void ensureInFrame();

        void copyCurrentScanBuffer()
        {}

        static GPU_TEXTURE_MODE_PARAM getTextureType(TextureType type);

        static GPU_Primitive_t getPrimitiveType(PrimitiveType type);

        // clang-format off
        ENUMMAP_DECLARE(PixelFormats, PixelFormat, GPU_TEXCOLOR,
            { PIXELFORMAT_RGBA8_UNORM,  GPU_RGBA8    },
            { PIXELFORMAT_RGBA4_UNORM,  GPU_RGBA4    },
            { PIXELFORMAT_RGB565_UNORM, GPU_RGB565   },
            { PIXELFORMAT_RGB5A1_UNORM, GPU_RGBA5551 },
            { PIXELFORMAT_ETC1_UNORM,   GPU_ETC1     },
            { PIXELFORMAT_LA8_UNORM,    GPU_LA8      },
            { PIXELFORMAT_A4_UNORM,     GPU_A4       }
        );

        ENUMMAP_DECLARE(CullModes, CullMode, GPU_CULLMODE,
            { CULL_NONE,  GPU_CULL_NONE      },
            { CULL_FRONT, GPU_CULL_FRONT_CCW },
            { CULL_BACK,  GPU_CULL_BACK_CCW  }
        );

        ENUMMAP_DECLARE(BlendOperations, BlendOperation, GPU_BLENDEQUATION,
            { BLENDOP_ADD,              GPU_BLEND_ADD              },
            { BLENDOP_SUBTRACT,         GPU_BLEND_SUBTRACT         },
            { BLENDOP_REVERSE_SUBTRACT, GPU_BLEND_REVERSE_SUBTRACT },
            { BLENDOP_MIN,              GPU_BLEND_MIN              },
            { BLENDOP_MAX,              GPU_BLEND_MAX              }
        );

        ENUMMAP_DECLARE(BlendFactors, BlendFactor, GPU_BLENDFACTOR,
            { BLENDFACTOR_ZERO,                 GPU_ZERO                 },
            { BLENDFACTOR_ONE,                  GPU_ONE                  },
            { BLENDFACTOR_SRC_COLOR,            GPU_SRC_COLOR            },
            { BLENDFACTOR_ONE_MINUS_SRC_COLOR,  GPU_ONE_MINUS_SRC_COLOR  },
            { BLENDFACTOR_SRC_ALPHA,            GPU_SRC_ALPHA            },
            { BLENDFACTOR_ONE_MINUS_SRC_ALPHA,  GPU_ONE_MINUS_SRC_ALPHA  },
            { BLENDFACTOR_DST_COLOR,            GPU_DST_COLOR            },
            { BLENDFACTOR_ONE_MINUS_DST_COLOR,  GPU_ONE_MINUS_DST_COLOR  },
            { BLENDFACTOR_DST_ALPHA,            GPU_DST_ALPHA            },
            { BLENDFACTOR_ONE_MINUS_DST_ALPHA,  GPU_ONE_MINUS_DST_ALPHA  },
            { BLENDFACTOR_SRC_ALPHA_SATURATED,  GPU_SRC_ALPHA_SATURATE   }
        );
        // clang-format on

      private:
        static GPU_TEXTURE_WRAP_PARAM getWrapMode(SamplerState::WrapMode mode);

        static int getTextureUnit(GPU_TEXUNIT unit);

        struct Context : public ContextBase
        {
            C3D_Mtx modelView;
            C3D_Mtx projection;

            C3D_RenderTarget* boundFramebuffer = nullptr;

            int currentTextureUnit = 0;
            std::vector<C3D_Tex*> boundTextures[TEXTURE_MAX_ENUM + 1];
            TexEnvMode texEnvMode = TEXENV_MODE_MAX_ENUM;
            C3D_Tex* boundTexture = nullptr;
        } context;

        bool isDefaultFramebufferActive() const;

        void createFramebuffers();

        void destroyFramebuffers();

        template<typename T>
        void modeChanged(const T& function)
        {
            this->destroyFramebuffers();
            function();
            // love::currentScreen = love::Screen(0);
            this->createFramebuffers();
        }

        std::vector<Framebuffer> targets;

        void updateTexEnvMode(TexEnvMode mode);
    };

    extern citro3d c3d;
} // namespace love
