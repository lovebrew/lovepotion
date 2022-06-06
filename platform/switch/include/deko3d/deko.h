#pragma once

#include "common/bitalloc.h"
#include "common/lmath.h"

#include "common/pixelformat.h"
#include "common/render/renderer.h"
#include "common/render/renderstate.h"

#include "deko3d/common.h"

#include "deko3d/CCmdMemRing.h"
#include "deko3d/CCmdVtxRing.h"
#include "deko3d/CDescriptorSet.h"
#include "deko3d/CImage.h"
#include "deko3d/CMemPool.h"
#include "deko3d/CShader.h"

#include "modules/window/window.h"

/* Enforces GLSL std140/std430 alignment rules for glm types */
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
/* Enables usage of SIMD CPU instructions (requiring the above as well) */
#define GLM_FORCE_INTRINSICS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define MAX_ANISOTROPY 16

using namespace love;

class deko3d : public Renderer
{
  private:
    deko3d();

    static constexpr const char* RENDERER_NAME    = "deko3d";
    static constexpr const char* RENDERER_VERSION = "0.4.0";
    static constexpr const char* RENDERER_VENDOR  = "devkitPro";
    static constexpr const char* RENDERER_DEVICE  = "NVIDIA Tegra X1";

    static constexpr unsigned MAX_FRAMEBUFFERS = 2;

    static constexpr unsigned COMMAND_SIZE      = 0x100000;
    static constexpr size_t VERTEX_COMMAND_SIZE = 0x100000;

    static constexpr size_t MAX_OBJECTS = 0x250;

    static constexpr float Z_NEAR = -10.0f;
    static constexpr float Z_FAR  = 10.0f;

    static constexpr int GPU_POOL_SIZE  = 64 * 1024 * 1024;
    static constexpr auto GPU_USE_FLAGS = (DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image);

    static constexpr int CPU_POOL_SIZE  = 1 * 1024 * 1024;
    static constexpr auto CPU_USE_FLAGS = (DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached);

    /* Used for Shader code */
    static constexpr int SHADER_POOL_SIZE = 128 * 1024;
    static constexpr auto SHADER_USE_FLAGS =
        (DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code);

    static constexpr auto FRAMEBUFFER_USE_FLAGS =
        (DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression);

  public:
    static deko3d& Instance();

    ~deko3d();

    enum MemPoolType
    {
        MEMPOOL_IMAGES,
        MEMPOOL_CODE,
        MEMPOOL_DATA,
        MEMPOOL_MAX_ENUM
    };

    enum GpuRenderState
    {
        GPU_RENDER_STATE_PRIMITIVE,
        GPU_RENDER_STATE_TEXTURE,
        GPU_RENDER_STATE_VIDEO,
        GPU_RENDER_STATE_MAX_ENUM
    };

    virtual void DestroyFramebuffers() override;

    virtual void CreateFramebuffers() override;

    virtual void Clear(const Colorf& color) override;

    virtual void ClearDepthStencil(int stencil, uint8_t mask, double depth) override;

    virtual void SetBlendColor(const Colorf& color) override;

    virtual void SetBlendMode(const RenderState::BlendState& blend) override;

    virtual void BindFramebuffer(love::Canvas* canvas = nullptr) override;

    virtual void Present() override;

    virtual void SetViewport(const Rect& viewport) override;

    virtual Rect GetViewport() const override;

    virtual void SetScissor(bool enable, const Rect& scissor, bool canvasActive) override;

    virtual void SetStencil(RenderState::CompareMode mode, int value) override;

    virtual void SetMeshCullMode(Vertex::CullMode mode) override;

    virtual void SetVertexWinding(Vertex::Winding winding) override;

    virtual void SetSamplerState(love::Texture* texture, SamplerState& state) override;

    virtual void SetColorMask(const RenderState::ColorMask& mask) override;

    virtual void SetLineWidth(float lineWidth) override;

    virtual void SetLineStyle(Graphics::LineStyle style) override;

    virtual void SetPointSize(float pointSize) override;

    /* helper functions */

    dk::Device GetDevice()
    {
        return this->device;
    }

    dk::Queue GetTextureQueue()
    {
        return this->textureQueue;
    }

    DkResHandle RegisterDescriptor(const dk::ImageDescriptor& descriptor);

    void DeRegisterDescriptor(DkResHandle handle);

    CMemPool& GetMemPool(MemPoolType type);

    CMemPool::Handle AllocatePool(MemPoolType type, size_t size, uint32_t alignment = 0);

    void OnOperationMode(Window::DisplaySize& size);

    static bool IsHandheldMode();

    /* rendering */

    void SetGPURenderState(GpuRenderState state);

    void SetShader(Shader* shader);

    bool RenderPolygon(const Vertex::PrimitiveVertex* points, size_t count);

    bool RenderPolyline(DkPrimitive mode, const Vertex::PrimitiveVertex* points, size_t count);

    bool RenderPoints(const Vertex::PrimitiveVertex* points, size_t count);

    bool RenderTexture(const DkResHandle handle, const Vertex::PrimitiveVertex* points,
                       size_t count);

    bool RenderVideo(const DkResHandle handles[3], const Vertex::PrimitiveVertex* points,
                     size_t count);

    /* internal functions */

    static bool GetConstant(PixelFormat in, DkImageFormat& out);
    static bool GetConstant(DkImageFormat in, PixelFormat& out);

    static bool GetConstant(RenderState::BlendOperation in, DkBlendOp& out);

    static bool GetConstant(RenderState::BlendFactor in, DkBlendFactor& out);

    static bool GetConstant(SamplerState::FilterMode in, DkFilter& out);

    static bool GetConstant(SamplerState::WrapMode in, DkWrapMode& out);

    static bool GetConstant(Vertex::CullMode in, DkFace& out);

    static bool GetConstant(Vertex::Winding in, DkFrontFace& out);

    static bool GetConstant(RenderState::CompareMode in, DkCompareOp& out);

  private:
    void EnsureInFrame();

    void EnsureHasSlot();

    virtual void InitRendererInfo() override;

    struct Transformation
    {
        glm::mat4 mdlvMtx;
        glm::mat4 projMtx;
    };
    static constexpr auto TRANSFORM_SIZE = sizeof(Transformation);

    uint32_t firstVertex = 0;
    GpuRenderState gpuRenderState;
    Vertex::PrimitiveVertex* vertexData;

    dk::UniqueDevice device;

    dk::UniqueQueue queue;
    dk::UniqueQueue textureQueue;

    dk::UniqueCmdBuf commandBuffer;
    dk::UniqueSwapchain swapchain;

    struct
    {
        CMemPool images;
        CMemPool data;
        CMemPool code;
    } pool;

    struct
    {
        dk::RasterizerState rasterizer;
        dk::ColorState color;
        dk::ColorWriteState colorWrite;
        dk::BlendState blendState;
        dk::DepthStencilState depthStencil;
    } deviceState;

    struct
    {
        dk::Image images[MAX_FRAMEBUFFERS];
        CMemPool::Handle memory[MAX_FRAMEBUFFERS];

        bool inFrame = false;
        int slot     = -1;
        bool dirty   = false;
    } framebuffers;

    struct
    {
        dk::ImageLayout layout;
        dk::Image image;
        CMemPool::Handle memory;
    } depthBuffer;

    struct
    {
        Transformation state;
        CMemPool::Handle buffer;
    } transform;

    struct
    {
        CDescriptorSet<MAX_OBJECTS> image;
        CDescriptorSet<MAX_OBJECTS> sampler;
        bool dirty = false;
    } descriptors;

    struct
    {
        dk::Sampler sampler;
        dk::SamplerDescriptor descriptor;
    } filter;

    CCmdMemRing<MAX_FRAMEBUFFERS> commands;
    CCmdVtxRing<MAX_FRAMEBUFFERS> vertices;

    dk::ImageLayout layoutFramebuffer;
    std::array<DkImage const*, MAX_FRAMEBUFFERS> framebufferArray;

    BitwiseAlloc<MAX_OBJECTS> allocator;
};
