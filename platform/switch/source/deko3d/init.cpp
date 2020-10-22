#include "common/runtime.h"
#include "deko3d/init.h"

using namespace deko3d;

void deko3d::Initialize()
{
    device = dk::DeviceMaker{}.create();
    queue = dk::QueueMaker{device}.setFlags(DkQueueFlags_Graphics).create();

    // Create the memory pools
    pool_images.emplace(device, DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image, 16 * 1024 * 1024);
    pool_data.emplace(device, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached, 1 * 1024 * 1024);

    // Create the static command buffer and feed it freshly allocated memory
    cmdbuf = dk::CmdBufMaker{device}.create();
    CMemPool::Handle cmdmem = pool_data->allocate(COMMAND_SIZE);
    cmdbuf.addMemory(cmdmem.getMemBlock(), cmdmem.getOffset(), cmdmem.getSize());

    deko3d::CreateResources();
}

void deko3d::CreateResources()
{
    // Create layout for the framebuffers
    dk::ImageLayout layout_framebuffer;
    dk::ImageLayoutMaker{device}
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT)
        .initialize(layout_framebuffer);

    // Create the framebuffers
    std::array<DkImage const *, MAX_FRAMEBUFFERS> fb_array;

    uint64_t fb_size  = layout_framebuffer.getSize();
    uint32_t fb_align = layout_framebuffer.getAlignment();

    for (unsigned i = 0; i < MAX_FRAMEBUFFERS; i ++)
    {
        // Allocate a framebuffer
        framebuffers_mem[i] = pool_images->allocate(fb_size, fb_align);
        framebuffers[i].initialize(layout_framebuffer, framebuffers_mem[i].getMemBlock(), framebuffers_mem[i].getOffset());

        // Generate a command list that binds it
        dk::ImageView colorTarget { framebuffers[i] };
        cmdbuf.bindRenderTargets(&colorTarget);

        framebuffer_cmdlists[i] = cmdbuf.finishList();

        // Fill in the array for use later by the swapchain creation code
        fb_array[i] = &framebuffers[i];
    }

    // Create the swapchain using the framebuffers
    swapchain = dk::SwapchainMaker{device, nwindowGetDefault(), fb_array}.create();
}

void deko3d::DeInitialize()
{
    // Return early if we have nothing to destroy
    if (!swapchain)
        return;

    // Make sure the queue is idle before destroying anything
    queue.waitIdle();

    // Clear the static cmdbuf, destroying the static cmdlists in the process
    cmdbuf.clear();

    // Destroy the swapchain
    swapchain.destroy();

    // Destroy the framebuffers
    for (unsigned i = 0; i < MAX_FRAMEBUFFERS; i ++)
        framebuffers_mem[i].destroy();
}