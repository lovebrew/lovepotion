#pragma once

#include "deko3d/common.h"
#include "deko3d/CMemPool.h"

namespace deko3d
{
    static constexpr unsigned MAX_FRAMEBUFFERS = 2;

    static constexpr uint32_t FRAMEBUFFER_WIDTH  = 1280;
    static constexpr uint32_t FRAMEBUFFER_HEIGHT = 720;

    static constexpr unsigned COMMAND_SIZE = 0x1000;

    static dk::UniqueDevice device;
    static dk::UniqueQueue queue;

    static std::optional<CMemPool> pool_images;
    static std::optional<CMemPool> pool_data;

    static dk::UniqueCmdBuf cmdbuf;

    static CMemPool::Handle framebuffers_mem[MAX_FRAMEBUFFERS];
    static dk::Image framebuffers[MAX_FRAMEBUFFERS];
    static DkCmdList framebuffer_cmdlists[MAX_FRAMEBUFFERS];
    static dk::UniqueSwapchain swapchain;

    static DkCmdList render_cmdlist;

    void Initialize();

    void CreateResources();

    void DeInitialize();
}