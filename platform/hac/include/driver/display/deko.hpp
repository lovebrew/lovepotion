#pragma once

#include "common/Map.hpp"
#include "common/pixelformat.hpp"

#include "driver/display/Framebuffer.hpp"
#include "driver/display/Renderer.tcc"

#include "driver/display/deko3d/CCmdMemRing.h"
#include "driver/display/deko3d/CCmdVtxRing.h"
#include "driver/display/deko3d/CDescriptorSet.h"

#include "modules/graphics/Texture.hpp"
#include "modules/graphics/vertex.hpp"

/* Enforces GLSL std140/std430 alignment rules for glm types */
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
/* Enables usage of SIMD CPU instructions (requiring the above as well) */
#define GLM_FORCE_INTRINSICS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace love
{
    class deko3d : public RendererBase
    {
      public:
        enum MemoryPool
        {
            MEMORYPOOL_IMAGE,
            MEMORYPOOL_CODE,
            MEMORYPOOL_DATA
        };

        enum QueueType
        {
            QUEUE_TYPE_MAIN,
            QUEUE_TYPE_IMAGES
        };

        deko3d();

        void initialize();

        ~deko3d();

        void clear(const Color& color);

        void clearDepthStencil(int stencil, uint8_t mask, double depth);

        dk::Image& getInternalBackbuffer();

        void bindFramebuffer(dk::Image& target);

        void present();

        void setBlendState(const BlendState& state);

        void setViewport(const Rect& viewport);

        void setScissor(const Rect& scissor);

        void setCullMode(CullMode mode);

        void setColorMask(ColorChannelMask mask);

        void setVertexWinding(Winding winding);

        void prepareDraw(GraphicsBase* graphics);

        void useProgram(const dk::Shader& vertex, const dk::Shader& fragment);

        void bindBuffer(BufferUsage usage, DkGpuAddr buffer, size_t size);

        void onModeChanged()
        {
            this->destroyFramebuffers();
            this->createFramebuffers();
        }

        CMemPool& getMemoryPool(MemoryPool pool)
        {
            switch (pool)
            {
                default:
                case MEMORYPOOL_IMAGE:
                    return images;
                case MEMORYPOOL_CODE:
                    return code;
                case MEMORYPOOL_DATA:
                    return data;
            }
        }

        dk::Device& getDevice()
        {
            return this->device;
        }

        dk::Queue& getQueue(QueueType type)
        {
            switch (type)
            {
                default:
                case QUEUE_TYPE_MAIN:
                    return mainQueue;
                case QUEUE_TYPE_IMAGES:
                    return textureQueue;
            }
        }

        // clang-format off
        ENUMMAP_DECLARE(BlendOperations, BlendOperation, DkBlendOp,
            { BLENDOP_ADD,              DkBlendOp_Add    },
            { BLENDOP_SUBTRACT,         DkBlendOp_Sub    },
            { BLENDOP_REVERSE_SUBTRACT, DkBlendOp_RevSub },
            { BLENDOP_MIN,              DkBlendOp_Min    },
            { BLENDOP_MAX,              DkBlendOp_Max    }
        );

        ENUMMAP_DECLARE(BlendFactors, BlendFactor, DkBlendFactor,
            { BLENDFACTOR_ZERO,                DkBlendFactor_Zero             },
            { BLENDFACTOR_ONE,                 DkBlendFactor_One              },
            { BLENDFACTOR_SRC_COLOR,           DkBlendFactor_SrcColor         },
            { BLENDFACTOR_ONE_MINUS_SRC_COLOR, DkBlendFactor_InvSrcColor      },
            { BLENDFACTOR_SRC_ALPHA,           DkBlendFactor_SrcAlpha         },
            { BLENDFACTOR_ONE_MINUS_SRC_ALPHA, DkBlendFactor_InvSrcAlpha      },
            { BLENDFACTOR_DST_COLOR,           DkBlendFactor_DstColor         },
            { BLENDFACTOR_ONE_MINUS_DST_COLOR, DkBlendFactor_InvDstColor      },
            { BLENDFACTOR_DST_ALPHA,           DkBlendFactor_DstAlpha         },
            { BLENDFACTOR_ONE_MINUS_DST_ALPHA, DkBlendFactor_InvDstAlpha      },
            { BLENDFACTOR_SRC_ALPHA_SATURATED, DkBlendFactor_SrcAlphaSaturate }
        );

        ENUMMAP_DECLARE(CullModes, CullMode, DkFace,
            { CULL_NONE,  DkFace_None  },
            { CULL_FRONT, DkFace_Front },
            { CULL_BACK,  DkFace_Back  }
        );

        ENUMMAP_DECLARE(WindingModes, Winding, DkFrontFace,
            { WINDING_CCW, DkFrontFace_CCW },
            { WINDING_CW,  DkFrontFace_CW  }
        );
        // clang-format on

      private:
        static constexpr int GPU_POOL_SIZE = 0x4000000;
        static constexpr int GPU_USE_FLAGS = (DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image);

        static constexpr int CPU_POOL_SIZE = 0x100000;
        static constexpr int CPU_USE_FLAGS = (DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached);

        static constexpr int SHADER_POOL_SIZE = 0x20000;
        static constexpr int SHADER_USE_FLAGS =
            DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code;

        static constexpr int RENDERTARGET_USE_FLAGS =
            DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression;

        static constexpr int COMMAND_SIZE        = 0x100000;
        static constexpr int VERTEX_COMMAND_SIZE = 0x100000;

        void ensureInFrame();

        void createFramebuffers();

        void destroyFramebuffers();

        struct Context : public ContextBase
        {
            dk::RasterizerState rasterizer;
            dk::ColorWriteState colorWrite;
            dk::BlendState blend;

            dk::Image* boundFramebuffer;
        } context;

        struct Transform
        {
            glm::mat4 projection;
            glm::mat4 modelView;
        } transform;

        static constexpr auto TRANSFORM_SIZE = sizeof(Transform);

        CMemPool::Handle uniformBuffer;

        dk::UniqueDevice device;

        dk::UniqueQueue mainQueue;
        dk::UniqueQueue textureQueue;

        dk::UniqueCmdBuf commandBuffer;
        dk::UniqueSwapchain swapchain;

        CMemPool images;
        CMemPool data;
        CMemPool code;

        int framebufferSlot;

        Framebuffer framebuffers[0x02];
        Framebuffer depthbuffer;

        CCmdMemRing<2> commands;
        std::array<DkImage const*, 2> targets;
    };

    extern deko3d d3d;
} // namespace love
