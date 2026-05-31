#pragma once

#include <3ds/gpu/enums.h>
#include <citro3d.h>

#include "common/Map.hpp"
#include "common/config.hpp"
#include "common/pixelformat.hpp"

#include "driver/display/Framebuffer.hpp"
#include "driver/display/Renderer.tcc"

#include "modules/graphics/Texture.tcc"
#include "modules/graphics/renderstate.hpp"
#include "modules/graphics/samplerstate.hpp"
#include "modules/graphics/vertex.hpp"

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

        virtual void init() override;

        virtual void close() override;

        virtual void clearColor(const Color& color) override;

        virtual void clear(double depth, int stencil) override;

        virtual void setClearDepth(double depth) override
        {
            LOVE_UNUSED(depth);
        }

        virtual void setClearStencil(int stencil) override
        {
            LOVE_UNUSED(stencil);
        }

        virtual void setStencilState(const StencilState& state) override;

        virtual void setViewport(const Rect& viewport) override;

        virtual void setScissor(const Rect& scissor) override;

        virtual void setCullMode(CullMode mode) override;

        virtual void setVertexAttributes(const VertexAttributes& attributes,
                                         const BufferBindings& buffers) override;

        virtual void prepareDraw(GraphicsBase* graphics) override;

        virtual void present() override;

        C3D_RenderTarget* getFramebuffer();

        C3D_RenderTarget* getInternalBackbuffer() const;

        void bindFramebuffer(C3D_RenderTarget* framebuffer);

        void setSamplerState(C3D_Tex* texture, SamplerState state);

        void bindTextureToUnit(TextureType target, C3D_Tex* texture, int unit);

        void bindTextureToUnit(TextureBase* texture, int unit);

        void setWideMode(bool wide)
        {
            this->modeChanged([wide]() { gfxSetWide(wide); });
        }

        bool isWideMode() const
        {
            return gfxIsWide();
        }

        void set3DMode(bool enable)
        {
            this->modeChanged([enable]() { gfxSet3D(enable); });
        }

        bool is3DMode() const
        {
            return gfxIs3D();
        }

        void deferCallToEndOfFrame(std::function<void()>&& function)
        {
            this->deferred.emplace_back(std::move(function));
        }

        void ensureInFrame();

        void copyCurrentScanBuffer()
        {}

        void setTexEnvMode(TextureBase* texture, bool isFont);

        // clang-format off

       ENUMMAP_DECLARE(PrimitiveModes, PrimitiveType, GPU_Primitive_t,
            { PRIMITIVE_TRIANGLES,      GPU_TRIANGLES      },
            { PRIMITIVE_TRIANGLE_STRIP, GPU_TRIANGLE_STRIP },
            { PRIMITIVE_TRIANGLE_FAN,   GPU_TRIANGLE_FAN   }
        );

        ENUMMAP_DECLARE(TextureModes, TextureType, GPU_TEXTURE_MODE_PARAM,
            { TEXTURE_2D,       GPU_TEX_2D       },
            { TEXTURE_CUBE,     GPU_TEX_CUBE_MAP }
        );

        ENUMMAP_DECLARE(DataBaseTypes, DataBaseType, GPU_FORMATS,
            { DATA_BASETYPE_FLOAT,  GPU_FLOAT }
        );

        ENUMMAP_DECLARE(SamplerModes, SamplerState::WrapMode, GPU_TEXTURE_WRAP_PARAM,
            { SamplerState::WRAP_CLAMP,           GPU_CLAMP_TO_EDGE   },
            { SamplerState::WRAP_REPEAT,          GPU_REPEAT          },
            { SamplerState::WRAP_MIRRORED_REPEAT, GPU_MIRRORED_REPEAT }
        );

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
            { BLENDFACTOR_SRC_ALPHA_SATURATED, GPU_SRC_ALPHA_SATURATE   }
        );

        ENUMMAP_DECLARE(StencilOps, StencilAction, GPU_STENCILOP,
            { STENCIL_KEEP,           GPU_STENCIL_KEEP      },
            { STENCIL_ZERO,           GPU_STENCIL_ZERO      },
            { STENCIL_REPLACE,        GPU_STENCIL_REPLACE   },
            { STENCIL_INCREMENT,      GPU_STENCIL_INCR      },
            { STENCIL_DECREMENT,      GPU_STENCIL_DECR      },
            { STENCIL_INCREMENT_WRAP, GPU_STENCIL_INCR_WRAP },
            { STENCIL_DECREMENT_WRAP, GPU_STENCIL_DECR_WRAP },
            { STENCIL_INVERT,         GPU_STENCIL_INVERT    }
        );

        ENUMMAP_DECLARE(CompareModes, CompareMode, GPU_TESTFUNC,
            { COMPARE_LESS,     GPU_LESS     },
            { COMPARE_LEQUAL,   GPU_LEQUAL   },
            { COMPARE_EQUAL,    GPU_EQUAL    },
            { COMPARE_GEQUAL,   GPU_GEQUAL   },
            { COMPARE_GREATER,  GPU_GREATER  },
            { COMPARE_NOTEQUAL, GPU_NOTEQUAL },
            { COMPARE_ALWAYS,   GPU_ALWAYS   },
            { COMPARE_NEVER,    GPU_NEVER    }
        );

        ENUMMAP_DECLARE(GpuFormats, DataFormat, GPU_FORMATS,
            { DATAFORMAT_FLOAT,  GPU_FLOAT },
            { DATAFORMAT_UINT16, GPU_SHORT }
        );
        // clang-format on

      private:
        static int getTextureUnit(GPU_TEXUNIT unit);

        struct Context : public ContextBase
        {
            C3D_RenderTarget* boundFramebuffer = nullptr;
            std::vector<C3D_Tex*> boundTextures[TEXTURE_MAX_ENUM + 1];
            TexEnvMode texEnvMode = TEXENV_MODE_MAX_ENUM;
            C3D_Tex* boundTexture = nullptr;

            GPU_TESTFUNC testMode;
            GPU_WRITEMASK mask;
        } context;

        bool isDefaultFramebufferActive() const;

        void createFramebuffers();

        void destroyFramebuffers();

        template<typename T>
        void modeChanged(const T& function)
        {
            this->destroyFramebuffers();
            function();
            this->createFramebuffers();
        }

        std::vector<Framebuffer> targets;

        std::vector<std::function<void()>> deferred;
    };

    extern citro3d c3d;
} // namespace love
