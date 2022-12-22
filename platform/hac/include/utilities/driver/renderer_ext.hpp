#pragma once

#include <utilities/driver/renderer/renderer.tcc>

#include <common/color.hpp>
#include <common/math.hpp>
#include <common/pixelformat.hpp>

#include <modules/graphics_ext.hpp>

#include <utilities/driver/renderer/renderstate.hpp>
#include <utilities/driver/renderer/samplerstate.hpp>
#include <utilities/driver/vertex_ext.hpp>

#include <utilities/bidirectionalmap/smallvector.hpp>

#include <utilities/deko3d/CCmdMemRing.h>
#include <utilities/deko3d/CCmdVtxRing.h>
#include <utilities/deko3d/CDescriptorSet.h>
#include <utilities/deko3d/CMemPool.h>

#include <utilities/deko3d/bitalloc.hpp>
#include <utilities/deko3d/common.h>

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
    enum class Screen : uint8_t
    {
        SCREEN_DEFAULT
    };

    template<>
    class Renderer<Console::HAC> : public Renderer<Console::ALL>
    {
      private:
        static constexpr const char* RENDERER_NAME    = "deko3d";
        static constexpr const char* RENDERER_VERSION = "0.4.0";
        static constexpr const char* RENDERER_VENDOR  = "devkitPro";
        static constexpr const char* RENDERER_DEVICE  = "NVIDIA Tegra X1";

        static constexpr uint8_t MAX_RENDERTARGETS = 0x02;
        static constexpr int MAX_ANISOTROPY        = 0x10;

        static constexpr int COMMAND_SIZE        = 0x100000;
        static constexpr int VERTEX_COMMAND_SIZE = 0x100000;

        static constexpr int MAX_OBJECTS = 0x250;

        static constexpr float Z_NEAR = -10.0f;
        static constexpr float Z_FAR  = 10.0f;

        // clang-format off
        static constexpr int GPU_POOL_SIZE = 0x4000000;
        static constexpr int GPU_USE_FLAGS = (DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image);

        static constexpr int CPU_POOL_SIZE = 0x100000;
        static constexpr int CPU_USE_FLAGS = (DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached);

        static constexpr int SHADER_POOL_SIZE = 0x20000;
        static constexpr int SHADER_USE_FLAGS = DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached |
                                                DkMemBlockFlags_Code;

        static constexpr int RENDERTARGET_USE_FLAGS = DkImageFlags_UsageRender | DkImageFlags_UsagePresent |
                                                      DkImageFlags_HwCompression;
        // clang-format on

        Renderer();

      public:
        enum MemPoolType
        {
            IMAGE,
            CODE,
            DATA
        };

        enum QueueType
        {
            QUEUE_MAIN,
            QUEUE_IMAGES
        };

        static Renderer& Instance()
        {
            static Renderer instance;
            return instance;
        }

        virtual ~Renderer();

        Info GetRendererInfo();

        void DestroyFramebuffers();

        void CreateFramebuffers();

        void Clear(const Color& color);

        void ClearDepthStencil(int stencil, uint8_t mask, double depth);

        void SetBlendColor(const Color& color);

        void SetBlendMode(const RenderState::BlendState& state);

        void BindFramebuffer(Texture<Console::Which>* texture = nullptr);

        void Present();

        void SetViewport(const Rect& viewport);

        void SetScissor(bool enable, const Rect& scissor, bool canvasActive);

        void SetStencil(RenderState::CompareMode mode, int value);

        void SetMeshCullMode(vertex::CullMode mode);

        void SetVertexWinding(vertex::Winding winding);

        void SetSamplerState(Texture<Console::HAC>* texture, SamplerState& state);

        void SetColorMask(const RenderState::ColorMask& mask);

        void SetLineWidth(float lineWidth);

        void SetLineStyle(Graphics<Console::Which>::LineStyle style);

        void SetPointSize(float size);

        void CheckDescriptorsDirty(const std::vector<DkResHandle>& handles);

        bool Render(const Graphics<Console::HAC>::DrawCommand& command);

        void UseProgram(Shader<Console::Which>::Program program);

        void Register(Texture<Console::Which>* texture, DkResHandle& handle);

        void UnRegister(Texture<Console::Which>* texture);

        void SetAttributes(const vertex::attributes::Attribs& attributes);

        std::optional<Screen> CheckScreen(const char* name) const;

        SmallTrivialVector<const char*, 1> GetScreens() const;

        bool IsHandheldMode() const;

        CMemPool::Handle Allocate(MemPoolType type, size_t size, uint32_t alignment = 0);

        CMemPool& GetMemPool(MemPoolType type);

        dk::Queue GetQueue(QueueType type);

        dk::Device GetDevice()
        {
            return this->device;
        }

        // clang-format off
        static constexpr BidirectionalMap pixelFormats = {
            PIXELFORMAT_R8_UNORM,         DkImageFormat_R8_Unorm,
            PIXELFORMAT_R16_UNORM,        DkImageFormat_R16_Unorm,
            PIXELFORMAT_RG8_UNORM,        DkImageFormat_RG8_Unorm,
            PIXELFORMAT_RGBA8_UNORM,      DkImageFormat_RGBA8_Unorm,
            PIXELFORMAT_RGB565_UNORM,     DkImageFormat_RGB565_Unorm,
            PIXELFORMAT_RGBA8_UNORM_SRGB, DkImageFormat_RGBA8_Unorm_sRGB,
            PIXELFORMAT_DXT1_UNORM,       DkImageFormat_RGBA_BC1,
            PIXELFORMAT_DXT3_UNORM,       DkImageFormat_RGBA_BC2,
            PIXELFORMAT_DXT5_UNORM,       DkImageFormat_RGBA_BC3,
            PIXELFORMAT_ETC1_UNORM,       DkImageFormat_RGB_ETC2,
            PIXELFORMAT_ETC2_RGB_UNORM,   DkImageFormat_RGB_ETC2,
            PIXELFORMAT_ETC2_RGBA1_UNORM, DkImageFormat_RGBA_ETC2,
            PIXELFORMAT_ETC2_RGBA_UNORM,  DkImageFormat_RGBA_ETC2,
            PIXELFORMAT_BC4_UNORM,        DkImageFormat_R_BC4_Unorm,
            PIXELFORMAT_BC5_UNORM,        DkImageFormat_RG_BC5_Unorm,
            PIXELFORMAT_BC7_UNORM,        DkImageFormat_RGBA_BC7_Unorm,
            PIXELFORMAT_BC7_UNORM_SRGB,   DkImageFormat_RGBA_BC7_Unorm_sRGB,
            PIXELFORMAT_ASTC_4x4,         DkImageFormat_RGBA_ASTC_4x4,
            PIXELFORMAT_ASTC_5x4,         DkImageFormat_RGBA_ASTC_5x4,
            PIXELFORMAT_ASTC_6x5,         DkImageFormat_RGBA_ASTC_6x5,
            PIXELFORMAT_ASTC_6x6,         DkImageFormat_RGBA_ASTC_6x6,
            PIXELFORMAT_ASTC_8x5,         DkImageFormat_RGBA_ASTC_8x5,
            PIXELFORMAT_ASTC_8x6,         DkImageFormat_RGBA_ASTC_8x6,
            PIXELFORMAT_ASTC_8x8,         DkImageFormat_RGBA_ASTC_8x8,
            PIXELFORMAT_ASTC_10x5,        DkImageFormat_RGBA_ASTC_10x5,
            PIXELFORMAT_ASTC_10x6,        DkImageFormat_RGBA_ASTC_10x6,
            PIXELFORMAT_ASTC_10x8,        DkImageFormat_RGBA_ASTC_10x8,
            PIXELFORMAT_ASTC_10x10,       DkImageFormat_RGBA_ASTC_10x10,
            PIXELFORMAT_ASTC_12x10,       DkImageFormat_RGBA_ASTC_12x10,
            PIXELFORMAT_ASTC_12x12,       DkImageFormat_RGBA_ASTC_12x12
        };

        static constexpr BidirectionalMap blendEquations = {
            RenderState::BLENDOP_ADD,              DkBlendOp_Add,
            RenderState::BLENDOP_SUBTRACT,         DkBlendOp_Sub,
            RenderState::BLENDOP_REVERSE_SUBTRACT, DkBlendOp_RevSub,
            RenderState::BLENDOP_MIN,              DkBlendOp_Min,
            RenderState::BLENDOP_MAX,              DkBlendOp_Max
        };

        static constexpr BidirectionalMap blendFactors = {
            RenderState::BLENDFACTOR_ZERO,                DkBlendFactor_Zero,
            RenderState::BLENDFACTOR_ONE,                 DkBlendFactor_One,
            RenderState::BLENDFACTOR_SRC_COLOR,           DkBlendFactor_SrcColor,
            RenderState::BLENDFACTOR_ONE_MINUS_SRC_COLOR, DkBlendFactor_InvSrcColor,
            RenderState::BLENDFACTOR_SRC_ALPHA,           DkBlendFactor_SrcAlpha,
            RenderState::BLENDFACTOR_ONE_MINUS_SRC_ALPHA, DkBlendFactor_InvSrcAlpha,
            RenderState::BLENDFACTOR_DST_COLOR,           DkBlendFactor_DstColor,
            RenderState::BLENDFACTOR_ONE_MINUS_DST_COLOR, DkBlendFactor_InvDstColor,
            RenderState::BLENDFACTOR_DST_ALPHA,           DkBlendFactor_DstAlpha,
            RenderState::BLENDFACTOR_ONE_MINUS_DST_ALPHA, DkBlendFactor_InvDstAlpha,
            RenderState::BLENDFACTOR_SRC_ALPHA_SATURATED, DkBlendFactor_SrcAlphaSaturate
        };

        static constexpr BidirectionalMap filterModes = {
            SamplerState::FILTER_LINEAR,  DkFilter_Linear,
            SamplerState::FILTER_NEAREST, DkFilter_Nearest
        };

        static constexpr BidirectionalMap mipmapFilterModes = {
            SamplerState::MIPMAP_FILTER_NONE,    DkMipFilter_None,
            SamplerState::MIPMAP_FILTER_LINEAR,  DkMipFilter_Linear,
            SamplerState::MIPMAP_FILTER_NEAREST, DkMipFilter_Nearest
        };

        static constexpr BidirectionalMap wrapModes = {
            SamplerState::WRAP_CLAMP,           DkWrapMode_ClampToEdge,
            SamplerState::WRAP_CLAMP_ZERO,      DkWrapMode_ClampToBorder,
            SamplerState::WRAP_REPEAT,          DkWrapMode_Repeat,
            SamplerState::WRAP_MIRRORED_REPEAT, DkWrapMode_MirroredRepeat
        };

        static constexpr BidirectionalMap cullModes = {
            vertex::CULL_NONE,  DkFace_None,
            vertex::CULL_BACK,  DkFace_Back,
            vertex::CULL_FRONT, DkFace_Front
        };

        static constexpr BidirectionalMap windingModes = {
            vertex::WINDING_CW,  DkFrontFace_CW,
            vertex::WINDING_CCW, DkFrontFace_CCW
        };

        static constexpr BidirectionalMap compareModes = {
            RenderState::COMPARE_LESS,     DkCompareOp_Less,
            RenderState::COMPARE_LEQUAL,   DkCompareOp_Lequal,
            RenderState::COMPARE_EQUAL,    DkCompareOp_Equal,
            RenderState::COMPARE_GEQUAL,   DkCompareOp_Gequal,
            RenderState::COMPARE_GREATER,  DkCompareOp_Greater,
            RenderState::COMPARE_NOTEQUAL, DkCompareOp_NotEqual,
            RenderState::COMPARE_ALWAYS,   DkCompareOp_Always,
            RenderState::COMPARE_NEVER,    DkCompareOp_Never
        };

        static constexpr BidirectionalMap primitiveModes = {
            vertex::PRIMITIVE_TRIANGLES,      DkPrimitive_Triangles,
            vertex::PRIMITIVE_TRIANGLE_FAN,   DkPrimitive_TriangleFan,
            vertex::PRIMITIVE_TRIANGLE_STRIP, DkPrimitive_TriangleStrip,
            vertex::PRIMITIVE_QUADS,          DkPrimitive_Quads,
            vertex::PRIMITIVE_POINTS,         DkPrimitive_Points
        };

        static constexpr BidirectionalMap gfxScreens = {
            (const char*)"default", Screen::SCREEN_DEFAULT
        };

      private:
        struct Transform
        {
            glm::mat4 modelView;
            glm::mat4 projection;
        };
        static constexpr int TRANSFORM_SIZE = sizeof(Transform);

        struct TransformBuffer
        {
            Transform state;
            CMemPool::Handle buffer;
        } transform;

        uint32_t firstVertex;
        vertex::Vertex* data;

        dk::UniqueDevice device;

        dk::UniqueQueue mainQueue;
        dk::UniqueQueue textureQueue;

        dk::UniqueCmdBuf commandBuffer;
        dk::UniqueSwapchain swapchain;

        struct MemPools
        {
            CMemPool image;
            CMemPool data;
            CMemPool code;
        } pools;

        struct DeviceState
        {
            dk::RasterizerState rasterizer;
            dk::ColorState color;
            dk::ColorWriteState colorWrite;
            dk::BlendState blend;
            dk::DepthStencilState depthStencil;
        } state;

        struct Framebuffer
        {
            dk::Image images[MAX_RENDERTARGETS];
            CMemPool::Handle memory[MAX_RENDERTARGETS];
            dk::ImageLayout layout;

            dk::Image depthImage;
            CMemPool::Handle depthMemory;
            dk::ImageLayout depthLayout;

            int slot = -1;
            bool dirty;

            union
            {
                int width;
                int height;
            } size;
        } framebuffers;

        struct DescriptorSets
        {
            CDescriptorSet<MAX_OBJECTS> image;
            CDescriptorSet<MAX_OBJECTS> sampler;
            bool dirty = false;
        } descriptors;

        std::unordered_map<uint32_t, dk::SamplerDescriptor> descriptorList;

        void EnsureInFrame();

        CCmdMemRing<MAX_RENDERTARGETS> commands;
        CCmdVtxRing<MAX_RENDERTARGETS> vertices;

        std::array<DkImage const*, MAX_RENDERTARGETS> rendertargets;

        BitwiseAlloc<MAX_OBJECTS> allocator;
    };
} // namespace love
