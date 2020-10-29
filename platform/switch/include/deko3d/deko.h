#pragma once

#include "deko3d/common.h"

#include "deko3d/CMemPool.h"
#include "deko3d/CCmdMemRing.h"
#include "deko3d/CCmdVtxRing.h"
#include "deko3d/CShader.h"

#include "objects/texture/texture.h"
#include "common/mmath.h"

#include "deko3d/shaderstage.h"
#include "deko3d/vertex.h"

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES // Enforces GLSL std140/std430 alignment rules for glm types
#define GLM_FORCE_INTRINSICS               // Enables usage of SIMD CPU instructions (requiring the above as well)
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

        static constexpr unsigned COMMAND_SIZE = 0x10000;
        static constexpr size_t VERTEX_COMMAND_SIZE = 0x10000;
        static constexpr size_t MAX_VERTICES = 0x10000;

        deko3d();

        ~deko3d();

        void BindFramebuffer();

        void SetFilter(const love::Texture::Filter & filter);

        void ClearColor(const Colorf & color);

        void ClearStencil(int stencil);

        void ClearDepth(double depth);

        void SetViewport(const love::Rect & view);

        void SetDepthWrites(bool enable);

        void SetBlendMode(DkBlendOp func, DkBlendFactor srcColor, DkBlendFactor srcAlpha,
                          DkBlendFactor dstColor, DkBlendFactor dstAlpha);

        void UseProgram(const std::pair<CShader, CShader> & program);

        void SetColorMask(bool r, bool g, bool b, bool a);

        float GetPointSize();

        void SetPointSize(float size);

        void SetFrontFaceWinding(DkFrontFace face);

        void SetCullMode(DkFace face);

        love::Rect GetViewport();

        void SetScissor(const love::Rect & scissor, bool canvasActive);

        void Present();

        void SetBlendColor(const Colorf & color);

        dk::UniqueDevice & GetDevice();

        std::optional<CMemPool> & GetCode();

        /* Primitives Rendering */

        enum DrawMode
        {
            DRAW_FILL,
            DRAW_LINE,
            DRAW_MAX_ENUM
        };

        bool RenderPolygon(bool fill, const vertex::Vertex * points, size_t count, bool skipLastFilledVertex = true);

        bool RenderPolyline(const vertex::Vertex * points, size_t count);

    private:
        vertex::Vertex * vertexData;
        uint32_t firstVertex = 0;

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
            bool hasSlot     = false;

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