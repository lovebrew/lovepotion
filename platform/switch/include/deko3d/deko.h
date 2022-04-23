#pragma once

#include "deko3d/common.h"

#include "common/bitalloc.h"
#include "deko3d/CCmdMemRing.h"
#include "deko3d/CCmdVtxRing.h"
#include "deko3d/CImage.h"
#include "deko3d/CMemPool.h"
#include "deko3d/CShader.h"
#include "deko3d/shader.h"

#include "objects/canvas/canvas.h"
#include "objects/font/font.h"
#include "objects/texture/texture.h"

#include "common/lmath.h"
#include "deko3d/vertex.h"
#include "graphics/graphics.h"

#include "deko3d/CDescriptorSet.h"

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES // Enforces GLSL std140/std430 alignment rules for glm
                                           // types
#define GLM_FORCE_INTRINSICS // Enables usage of SIMD CPU instructions (requiring the above as well)
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define MAX_ANISOTROPY 16

namespace love
{
    class Graphics;
}

class deko3d
{
  private:
    deko3d();

  public:
    static constexpr unsigned MAX_FRAMEBUFFERS = 2;

    static constexpr unsigned COMMAND_SIZE      = 0x100000;
    static constexpr size_t VERTEX_COMMAND_SIZE = 0x100000;

    static constexpr size_t MAX_OBJECTS = 0x250;

    static deko3d& Instance();

    ~deko3d();

    void CreateFramebufferResources();

    void DestroyFramebufferResources();

    void BindFramebuffer(love::Canvas* canvas = nullptr);

    void SetTextureFilter(const love::Texture::Filter& filter);

    void SetTextureFilter(love::Texture* texture, const love::Texture::Filter& filter);

    void SetTextureWrap(const love::Texture::Wrap& wrap);

    void SetTextureWrap(love::Texture* texture, const love::Texture::Wrap& filter);

    void ClearColor(const Colorf& color);

    void ClearDepthStencil(double depth, int stencil);

    void BeginFrame();

    void SetViewport(const love::Rect& view);

    void SetDepthWrites(bool enable);

    void SetBlendMode(DkBlendOp func, DkBlendFactor srcColor, DkBlendFactor srcAlpha,
                      DkBlendFactor dstColor, DkBlendFactor dstAlpha);

    void UseProgram(const love::Shader::Program& program);

    void SetColorMask(const love::Graphics::ColorMask& mask);

    float GetPointSize();

    void SetPointSize(float size);

    void SetLineWidth(float width);

    void SetLineStyle(bool smooth);

    void SetFrontFaceWinding(DkFrontFace face);

    void SetCullMode(DkFace face);

    love::Rect GetViewport();

    void SetScissor(const love::Rect& scissor, bool canvasActive);

    void SetScissor();

    void Present();

    void SetBlendColor(const Colorf& color);

    void SetStencil(DkStencilOp op, DkCompareOp compare, int value);

    void OnOperationMode(std::pair<uint32_t, uint32_t>& size);

    bool IsHandheldMode()
    {
        AppletOperationMode mode = appletGetOperationMode();

        return mode == AppletOperationMode::AppletOperationMode_Handheld;
    }

    dk::Device GetDevice()
    {
        return this->device;
    }

    dk::Queue GetTextureQueue()
    {
        return this->textureQueue;
    }

    CMemPool& GetImages()
    {
        return this->pool.images;
    }

    CMemPool& GetCode()
    {
        return this->pool.code;
    }

    CMemPool& GetData()
    {
        return this->pool.data;
    }

    DkResHandle RegisterResHandle(const dk::ImageDescriptor& descriptor);

    void UnRegisterResHandle(DkResHandle handle);

    bool RenderTexture(const DkResHandle handle, const vertex::Vertex* points, size_t count);

    bool RenderVideo(const DkResHandle handles[3], const vertex::Vertex* points, size_t count);

    /* Primitives Rendering */

    bool RenderPolygon(const vertex::Vertex* points, size_t count);

    bool RenderPolyline(DkPrimitive mode, const vertex::Vertex* points, size_t count);

    bool RenderPoints(const vertex::Vertex* points, size_t count);

    static DkWrapMode GetDekoWrapMode(love::Texture::WrapMode wrap);

    static bool GetConstant(PixelFormat in, DkImageFormat& out);

    static bool GetConstant(DkImageFormat in, PixelFormat& out);

    void SetDekoBarrier(DkBarrier barrier, uint32_t flags);

  private:
    vertex::Vertex* vertexData;

    uint32_t firstVertex = 0;
    BitwiseAlloc<MAX_OBJECTS> allocator;

    enum State
    {
        STATE_PRIMITIVE,
        STATE_TEXTURE,
        STATE_VIDEO,
        STATE_MAX_ENUM
    };

    static constexpr float Z_NEAR = -10.0f;
    static constexpr float Z_FAR  = 10.0f;

    State renderState;

    void EnsureInState(State state);

    struct
    {
        CDescriptorSet<MAX_OBJECTS> image;
        CDescriptorSet<MAX_OBJECTS> sampler;
        bool dirty = false;
    } descriptors;

    struct Transformation
    {
        glm::mat4 mdlvMtx;
        glm::mat4 projMtx;
    };

    dk::UniqueDevice device;
    dk::UniqueQueue queue;
    dk::UniqueCmdBuf cmdBuf;

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

        float pointSize;
    } state;

    dk::UniqueQueue textureQueue;

    CCmdMemRing<MAX_FRAMEBUFFERS> cmdRing;
    CCmdVtxRing<MAX_FRAMEBUFFERS> vtxRing;

    love::Rect viewport;
    love::Rect scissor;

    struct
    {
        dk::Sampler sampler;
        dk::SamplerDescriptor descriptor;
    } filter;

    struct
    {
        dk::Image images[MAX_FRAMEBUFFERS];
        CMemPool::Handle memory[MAX_FRAMEBUFFERS];

        bool inFrame = false;
        int slot     = -1;
        bool dirty   = false;
    } framebuffers;

    bool descriptorsDirty;

    struct
    {
        dk::ImageLayout layout;
        dk::Image image;
        CMemPool::Handle memory;
    } depthBuffer;

    Transformation transformState;
    CMemPool::Handle transformUniformBuffer;

    dk::ImageLayout layoutFramebuffer;
    std::array<DkImage const*, MAX_FRAMEBUFFERS> framebufferArray;

    dk::UniqueSwapchain swapchain;

    void EnsureInFrame();

    void EnsureHasSlot();
};
