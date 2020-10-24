#pragma once

#include "deko3d/common.h"

#include "deko3d/CMemPool.h"
#include "deko3d/CCmdMemRing.h"
#include "deko3d/CShader.h"

#include "objects/texture/texture.h"
#include "common/mmath.h"

class deko3d
{
    public:
        static constexpr unsigned MAX_FRAMEBUFFERS = 2;

        static constexpr uint32_t FRAMEBUFFER_WIDTH  = 1280;
        static constexpr uint32_t FRAMEBUFFER_HEIGHT = 720;

        static constexpr unsigned COMMAND_SIZE = 0x10000;

        deko3d();

        ~deko3d();

        void BindFramebuffer();

        void SetFilter(const love::Texture::Filter & filter);

        void Clear(std::optional<Colorf> color, std::optional<int> stencil, std::optional<double> depth);

        void ClearDepth(double depth);

        void SetViewport(const love::Rect & view);

        void UseProgram(std::pair<CShader, CShader> program);

        love::Rect GetViewport();

        void SetScissor(const love::Rect & scissor, bool canvasActive);

        void Present();

        dk::UniqueDevice & GetDevice();

        std::optional<CMemPool> & GetCode();

    private:
        struct
        {
            std::optional<CMemPool> images;
            std::optional<CMemPool> data;
            std::optional<CMemPool> code;
        } pool;

        dk::UniqueDevice device;
        dk::UniqueQueue queue;
        dk::UniqueCmdBuf cmdBuf;

        CCmdMemRing<2> cmdRing;

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
            DkCmdList        cmdlists[MAX_FRAMEBUFFERS];
            CMemPool::Handle memory[MAX_FRAMEBUFFERS];

            bool inFrame     = false;
            bool hasSlot     = false;

            int  slot        = -1;
        } framebuffers;

        dk::ImageLayout layoutFramebuffer;
        std::array<DkImage const *, MAX_FRAMEBUFFERS> framebufferArray;

        dk::UniqueSwapchain swapchain;

        DkCmdList render_cmdlist;

        void CreateResources();

        void EnsureInFrame();

        void EnsureHasSlot();
};

extern ::deko3d dk3d;