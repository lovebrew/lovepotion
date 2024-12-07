#pragma once

#include "common/Map.hpp"
#include "common/pixelformat.hpp"

#include "driver/display/Framebuffer.hpp"
#include "driver/display/Renderer.tcc"

#include "driver/graphics/BitAlloc.hpp"

#include "driver/display/deko3d/CCmdMemRing.h"
#include "driver/display/deko3d/CDescriptorSet.h"

#include "modules/graphics/vertex.hpp"

/* Enforces GLSL std140/std430 alignment rules for glm types */
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
/* Enables usage of SIMD CPU instructions (requiring the above as well) */
#define GLM_FORCE_INTRINSICS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define MAX_OBJECTS       0x400
#define MAX_RENDERTARGETS 2

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

        void deInitialize();

        void clear(const Color& color);

        void clearDepthStencil(int stencil, uint8_t mask, double depth);

        dk::Image& getInternalBackbuffer();

        void bindFramebuffer(dk::Image* target = nullptr);

        void present();

        void setBlendState(const BlendState& state);

        void setViewport(const Rect& viewport);

        void setScissor(const Rect& scissor);

        void setCullMode(CullMode mode);

        void setColorMask(ColorChannelMask mask);

        void setVertexWinding(Winding winding);

        void setPointSize(float size);

        void setSamplerState(TextureBase* texture, const SamplerState& state);

        void prepareDraw(GraphicsBase* graphics);

        void useProgram(const dk::Shader& vertex, const dk::Shader& fragment);

        void bindBuffer(BufferUsage usage, CMemPool::Handle& memory);

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

        dk::Device getDevice()
        {
            return this->device;
        }

        dk::Queue getQueue(QueueType type)
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

        void registerTexture(TextureBase* texture, bool registering);

        void ensureInFrame();

        void setVertexAttributes(bool isTexture);

        void bindTextureToUnit(TextureBase* texture, int unit);

        void bindTextureToUnit(DkResHandle texture, int unit);

        void drawIndexed(DkPrimitive primitive, uint32_t indexCount, uint32_t indexOffset,
                         uint32_t instanceCount);

        void draw(DkPrimitive primitive, uint32_t vertexCount, uint32_t firstVertex);

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

        ENUMMAP_DECLARE(FilterModes, SamplerState::FilterMode, DkFilter,
            { SamplerState::FILTER_LINEAR,  DkFilter_Linear  },
            { SamplerState::FILTER_NEAREST, DkFilter_Nearest }
        );

        ENUMMAP_DECLARE(MipFilterModes, SamplerState::MipmapFilterMode, DkMipFilter,
            { SamplerState::MIPMAP_FILTER_LINEAR,  DkMipFilter_Linear  },
            { SamplerState::MIPMAP_FILTER_NEAREST, DkMipFilter_Nearest },
            { SamplerState::MIPMAP_FILTER_NONE,    DkMipFilter_None    }
        );

        ENUMMAP_DECLARE(WrapModes, SamplerState::WrapMode, DkWrapMode,
            { SamplerState::WRAP_CLAMP,           DkWrapMode_Clamp          },
            { SamplerState::WRAP_CLAMP_ZERO,      DkWrapMode_ClampToBorder  },
            { SamplerState::WRAP_REPEAT,          DkWrapMode_Repeat         },
            { SamplerState::WRAP_MIRRORED_REPEAT, DkWrapMode_MirroredRepeat }
        );

        ENUMMAP_DECLARE(PixelFormats, PixelFormat, DkImageFormat,
            { PIXELFORMAT_R8_UNORM,         DkImageFormat_R8_Unorm            },
            { PIXELFORMAT_R16_UNORM,        DkImageFormat_R16_Unorm           },
            { PIXELFORMAT_RG8_UNORM,        DkImageFormat_RG8_Unorm           },
            { PIXELFORMAT_RGBA8_UNORM,      DkImageFormat_RGBA8_Unorm         },
            { PIXELFORMAT_RGB565_UNORM,     DkImageFormat_RGB565_Unorm        },
            { PIXELFORMAT_RGBA8_sRGB,       DkImageFormat_RGBA8_Unorm_sRGB    },
            { PIXELFORMAT_DXT1_UNORM,       DkImageFormat_RGBA_BC1            },
            { PIXELFORMAT_DXT3_UNORM,       DkImageFormat_RGBA_BC2            },
            { PIXELFORMAT_DXT5_UNORM,       DkImageFormat_RGBA_BC3            },
            { PIXELFORMAT_ETC1_UNORM,       DkImageFormat_RGB_ETC2            },
            { PIXELFORMAT_ETC2_RGB_UNORM,   DkImageFormat_RGB_ETC2            },
            { PIXELFORMAT_ETC2_RGBA1_UNORM, DkImageFormat_RGBA_ETC2           },
            { PIXELFORMAT_ETC2_RGBA_UNORM,  DkImageFormat_RGBA_ETC2           },
            { PIXELFORMAT_BC4_UNORM,        DkImageFormat_R_BC4_Unorm         },
            { PIXELFORMAT_BC5_UNORM,        DkImageFormat_RG_BC5_Unorm        },
            { PIXELFORMAT_BC7_UNORM,        DkImageFormat_RGBA_BC7_Unorm      },
            { PIXELFORMAT_BC7_sRGB,         DkImageFormat_RGBA_BC7_Unorm_sRGB },
            { PIXELFORMAT_ASTC_4x4_UNORM,   DkImageFormat_RGBA_ASTC_4x4       },
            { PIXELFORMAT_ASTC_5x4_UNORM,   DkImageFormat_RGBA_ASTC_5x4       },
            { PIXELFORMAT_ASTC_6x5_UNORM,   DkImageFormat_RGBA_ASTC_6x5       },
            { PIXELFORMAT_ASTC_6x6_UNORM,   DkImageFormat_RGBA_ASTC_6x6       },
            { PIXELFORMAT_ASTC_8x5_UNORM,   DkImageFormat_RGBA_ASTC_8x5       },
            { PIXELFORMAT_ASTC_8x6_UNORM,   DkImageFormat_RGBA_ASTC_8x6       },
            { PIXELFORMAT_ASTC_8x8_UNORM,   DkImageFormat_RGBA_ASTC_8x8       },
            { PIXELFORMAT_ASTC_10x5_UNORM,  DkImageFormat_RGBA_ASTC_10x5      },
            { PIXELFORMAT_ASTC_10x6_UNORM,  DkImageFormat_RGBA_ASTC_10x6      },
            { PIXELFORMAT_ASTC_10x8_UNORM,  DkImageFormat_RGBA_ASTC_10x8      },
            { PIXELFORMAT_ASTC_10x10_UNORM, DkImageFormat_RGBA_ASTC_10x10     },
            { PIXELFORMAT_ASTC_12x10_UNORM, DkImageFormat_RGBA_ASTC_12x10     },
            { PIXELFORMAT_ASTC_12x12_UNORM, DkImageFormat_RGBA_ASTC_12x12     }
        );

        ENUMMAP_DECLARE(PrimitiveTypes, PrimitiveType, DkPrimitive,
            { PRIMITIVE_TRIANGLES,      DkPrimitive_Triangles     },
            { PRIMITIVE_POINTS,         DkPrimitive_Points        },
            { PRIMITIVE_TRIANGLE_STRIP, DkPrimitive_TriangleStrip },
            { PRIMITIVE_TRIANGLE_FAN,   DkPrimitive_TriangleFan   }
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

        static constexpr int COMMAND_SIZE = 0x100000;

        void createFramebuffers();

        void destroyFramebuffers();

        struct Context : public ContextBase
        {
            dk::RasterizerState rasterizer;
            dk::ColorWriteState colorWrite;
            dk::BlendState blend;
            dk::ColorState color;
            dk::DepthStencilState depthStencil;

            dk::Image* boundFramebuffer;
            bool descriptorsDirty;
        } context;

        struct Transform
        {
            glm::mat4 modelView;
            glm::mat4 projection;
        } transform;

        static constexpr auto TRANSFORM_SIZE = sizeof(Transform);

        CMemPool::Handle uniform;

        dk::UniqueDevice device;

        dk::UniqueQueue mainQueue;
        dk::UniqueQueue textureQueue;

        dk::UniqueCmdBuf commandBuffer;
        dk::UniqueSwapchain swapchain;

        CMemPool images;
        CMemPool data;
        CMemPool code;

        int framebufferSlot;

        Framebuffer framebuffers[MAX_RENDERTARGETS];
        Framebuffer depthbuffer;

        CCmdMemRing<MAX_RENDERTARGETS> commands;
        std::array<DkImage const*, MAX_RENDERTARGETS> targets;

        BitwiseAlloc<MAX_OBJECTS> textureHandles;
        CDescriptorSet<MAX_OBJECTS> imageSet;
        CDescriptorSet<MAX_OBJECTS> samplerSet;
    };

    extern deko3d d3d;
} // namespace love
