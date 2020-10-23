#pragma once

#include "deko3d/common.h"
#include "deko3d/CMemPool.h"

#include "objects/texture/texture.h"
#include "common/mmath.h"

class deko3d
{
    public:
        static constexpr unsigned MAX_FRAMEBUFFERS = 2;

        static constexpr uint32_t FRAMEBUFFER_WIDTH  = 1280;
        static constexpr uint32_t FRAMEBUFFER_HEIGHT = 720;

        static constexpr unsigned COMMAND_SIZE = 0x1000;

        deko3d();

        ~deko3d();

        void SetFilter(const love::Texture::Filter & filter);

        void Clear(std::optional<Colorf> color, std::optional<int> stencil, std::optional<double> depth);

        void ClearDepth(double depth);

        void SetViewport(const love::Rect & view);

        love::Rect GetViewport();

        void SetScissor(const love::Rect & scissor, bool canvasActive);

        template <typename T>
        void CommandBuffer(const T & func)
        {
            func(cmdbuf);
        }

    private:
        struct
        {
            std::optional<CMemPool> images;
            std::optional<CMemPool> data;
        } pool;

        dk::UniqueDevice device;
        dk::UniqueQueue queue;
        dk::UniqueCmdBuf cmdbuf;

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
        } framebuffers;

        dk::UniqueSwapchain swapchain;

        DkCmdList render_cmdlist;

        void CreateResources();
};

extern ::deko3d dk3d;