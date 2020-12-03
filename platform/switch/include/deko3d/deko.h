#pragma once

#include "deko3d/common.h"

#include "deko3d/CMemPool.h"
#include "deko3d/CCmdMemRing.h"
#include "deko3d/CCmdVtxRing.h"
#include "deko3d/CShader.h"
#include "deko3d/shader.h"
#include "deko3d/CImage.h"

#include "objects/texture/texture.h"
#include "objects/font/font.h"
#include "objects/canvas/canvas.h"

#include "common/mmath.h"
#include "deko3d/vertex.h"

#include "deko3d/CDescriptorSet.h"

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES // Enforces GLSL std140/std430 alignment rules for glm types
#define GLM_FORCE_INTRINSICS               // Enables usage of SIMD CPU instructions (requiring the above as well)
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define MAX_ANISOTROPY 16

namespace love
{
    class Graphics;
}

class deko3d
{
    public:
        static constexpr unsigned MAX_FRAMEBUFFERS = 2;

        static constexpr uint32_t FRAMEBUFFER_WIDTH  = 1280;
        static constexpr uint32_t FRAMEBUFFER_HEIGHT = 720;

        static constexpr unsigned COMMAND_SIZE      = 0x100000;
        static constexpr size_t VERTEX_COMMAND_SIZE = 0x100000;

        static constexpr size_t MAX_OBJECTS = 0x250;

        deko3d();

        ~deko3d();

        void BindFramebuffer(love::Canvas * canvas = nullptr);

        void SetTextureFilter(love::Texture::Filter & filter);

        void SetTextureWrap(love::Texture::Wrap & wrap);

        void ClearColor(const Colorf & color);

        void ClearDepthStencil(double depth, int stencil);

        void BeginFrame();

        void SetViewport(const love::Rect & view);

        void SetDepthWrites(bool enable);

        void SetBlendMode(DkBlendOp func, DkBlendFactor srcColor, DkBlendFactor srcAlpha,
                          DkBlendFactor dstColor, DkBlendFactor dstAlpha);

        void UseProgram(const love::Shader::Program & program);

        void SetColorMask(bool r, bool g, bool b, bool a);

        float GetPointSize();

        void SetPointSize(float size);

        void SetLineWidth(float width);

        void SetLineStyle(bool smooth);

        void SetFrontFaceWinding(DkFrontFace face);

        void SetCullMode(DkFace face);

        love::Rect GetViewport();

        void SetScissor(const love::Rect & scissor, bool canvasActive);

        void SetScissor();

        void Present();

        void SetBlendColor(const Colorf & color);

        void SetStencil(DkStencilOp op, DkCompareOp compare, int value);

        dk::UniqueDevice & GetDevice();

        dk::Queue & GetTextureQueue();

        std::optional<CMemPool> & GetImages();

        std::optional<CMemPool> & GetCode();

        std::optional<CMemPool> & GetData();

        void RegisterResHandle(const dk::ImageDescriptor & descriptor, love::Texture * texture);

        void UnRegisterResHandle(love::Texture * texture);

        void LoadTextureBuffer(CImage & image, void * buffer, size_t size, love::Texture * texture, DkImageFormat format);

        bool RenderTexture(const DkResHandle handle, const vertex::Vertex * points, size_t size, size_t count);

        /* Primitives Rendering */

        bool RenderPolygon(const vertex::Vertex * points, size_t size, size_t count);

        bool RenderPolyline(const vertex::Vertex * points, size_t size, size_t count);

        bool RenderPoints(const vertex::Vertex * points, size_t size, size_t count);

        static DkWrapMode GetDekoWrapMode(love::Texture::WrapMode wrap);

    private:
        vertex::Vertex * vertexData;

        uint32_t firstVertex = 0;
        uint32_t textureIDs = 0;

        enum State
        {
            STATE_PRIMITIVE,
            STATE_TEXTURE,
            STATE_MAX_ENUM
        };

        State renderState = State::STATE_PRIMITIVE;

        void EnsureInState(State state);

        std::unordered_map<love::Drawable *, uint32_t> textureResIDs;

        struct
        {
            CDescriptorSet<MAX_OBJECTS> image;
            CDescriptorSet<MAX_OBJECTS> sampler;
        } descriptors;

        struct Transformation
        {
            glm::mat4 mdlvMtx;
            glm::mat4 projMtx;
        };

        struct
        {
            std::optional<CMemPool> images;
            std::optional<CMemPool> data;
            std::optional<CMemPool> code;
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

        dk::UniqueDevice device;
        dk::UniqueQueue queue;
        dk::UniqueCmdBuf cmdBuf;

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
            dk::Image        images[MAX_FRAMEBUFFERS];
            CMemPool::Handle memory[MAX_FRAMEBUFFERS];

            bool inFrame     = false;
            int  slot        = -1;
        } framebuffers;

        struct
        {
            dk::ImageLayout layout;
            dk::Image image;
            CMemPool::Handle memory;
        } depthBuffer;

        Transformation transformState;
        CMemPool::Handle transformUniformBuffer;

        dk::ImageLayout layoutFramebuffer;
        std::array<DkImage const *, MAX_FRAMEBUFFERS> framebufferArray;

        dk::UniqueSwapchain swapchain;

        DkCmdList render_cmdlist;

        void CreateResources();

        void EnsureInFrame();

        void EnsureHasSlot();
};

extern ::deko3d dk3d;
