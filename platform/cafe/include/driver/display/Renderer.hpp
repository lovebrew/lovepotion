#pragma once

#include "common/Map.hpp"
#include "common/pixelformat.hpp"

#include "driver/display/Framebuffer.hpp"
#include "driver/display/Renderer.tcc"

#include "modules/graphics/vertex.hpp"

#include <gx2/enum.h>

namespace love
{
    class Renderer : public RendererBase<Renderer>
    {
      public:
        Renderer();

        void initialize();

        ~Renderer();

        int onForegroundAcquired();

        int onForegroundReleased();

        void clear(const Color& color);

        void clearDepthStencil(int depth, uint8_t mask, double stencil);

        void bindFramebuffer();

        void present();

        void setBlendState(const BlendState& state);

        void setViewport(const Rect& viewport);

        void setScissor(const Rect& scissor);

        void setCullMode(CullMode mode);

        void setColorMask(ColorChannelMask mask);

        void setVertexWinding(Winding winding);

        // clang-format off
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
            { BLENDFACTOR_SRC_COLOR,           GX2_BLEND_MODE_SRC_COLOR,     },
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
        // clang-format on

      private:
        std::array<Framebuffer, 2> targets;

        void ensureInFrame();

        void createFramebuffers();

        void destroyFramebuffers();

        struct Context : public ContextBase
        {
            glm::mat4 modelView;
            glm::mat4 projection;

            bool cullBack;
            bool cullFront;

            GX2FrontFace winding;
            bool depthWrite;
            bool depthTest;
            GX2CompareFunction compareMode;

            uint32_t writeMask;

            Framebuffer target;
        } context;

        bool inForeground;

        void* commandBuffer;
        GX2ContextState* state;
    };
} // namespace love
