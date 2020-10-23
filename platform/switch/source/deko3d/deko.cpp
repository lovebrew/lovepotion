#include "common/runtime.h"
#include "deko3d/deko.h"

deko3d::deko3d()
{
    /*
    ** Create GPU device
    ** default origin is top left
    ** Unique -- destroys automatically
    */
    this->device = dk::DeviceMaker{}
                        .create();

    /*
    ** Render Queue
    ** Unique -- destroys automatically
    */
    this->queue = dk::QueueMaker{device}
                        .setFlags(DkQueueFlags_Graphics)
                        .create();

    // Create GPU & CPU Memory Pools
    this->pool.images.emplace(device, DkMemBlockFlags_GpuCached   | DkMemBlockFlags_Image,     16 * 1024 * 1024);
    this->pool.data.emplace(device,   DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached, 1  * 1024 * 1024);

    // Create the static command buffer
    this->cmdbuf = dk::CmdBufMaker{device}
                    .create();

    // Feed the command buffer some new memory
    CMemPool::Handle cmdmem = this->pool.data->allocate(COMMAND_SIZE);
    cmdbuf.addMemory(cmdmem.getMemBlock(), cmdmem.getOffset(), cmdmem.getSize());

    this->CreateResources();
}

void deko3d::CreateResources()
{
    // Create layout for the framebuffers
    dk::ImageLayout layoutFramebuffer;

    // Create a Layout for the Device
    dk::ImageLayoutMaker {device}
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT)
        .initialize(layoutFramebuffer);

    // Create the framebuffers
    std::array<DkImage const *, MAX_FRAMEBUFFERS> framebufferArray;

    uint64_t framebufferSize  = layoutFramebuffer.getSize();
    uint32_t framebufferAlign = layoutFramebuffer.getAlignment();

    for (unsigned i = 0; i < MAX_FRAMEBUFFERS; i ++)
    {
        // Allocate a framebuffer
        this->framebuffers.memory[i] = this->pool.images->allocate(framebufferSize, framebufferAlign);
        this->framebuffers.images[i].initialize(layoutFramebuffer, this->framebuffers.memory[i].getMemBlock(), this->framebuffers.memory[i].getOffset());

        // Generate a command list that binds it
        dk::ImageView colorTarget { this->framebuffers.images[i] };
        cmdbuf.bindRenderTargets(&colorTarget);

        this->framebuffers.cmdlists[i] = cmdbuf.finishList();

        // Fill in the array for use later by the swapchain creation code
        framebufferArray[i] = &this->framebuffers.images[i];
    }

    // Create the swapchain using the framebuffers
    swapchain = dk::SwapchainMaker{device, nwindowGetDefault(), framebufferArray}.create();
}

void deko3d::SetFilter(const love::Texture::Filter & filter)
{
    // Set the global filter mode for textures

    // this->filter.sampler.setFilter(DkMipFilter_Linear, DkMipFilter_Linear);
    // this->filter.descriptor.initialize(this->filter.sampler);
}

void deko3d::ClearDepth(double depth)
{}

void deko3d::SetScissor(const love::Rect & scissor, bool canvasActive)
{}

void deko3d::SetViewport(const love::Rect & view)
{
    this->viewport = view;
    this->cmdbuf.setViewports(0, { {view.x, view.y, view.w, view.h} });
}

Rect deko3d::GetViewport()
{
    return this->viewport;
}

deko3d::~deko3d()
{
    // Return early if we have nothing to destroy
    if (!this->swapchain)
        return;

    // Make sure the queue is idle before destroying anything
    this->queue.waitIdle();

    // Clear the static cmdbuf, destroying the static cmdlists in the process
    this->cmdbuf.clear();

    // Destroy the swapchain
    this->swapchain.destroy();

    // Destroy the framebuffers
    for (unsigned i = 0; i < MAX_FRAMEBUFFERS; i ++)
        this->framebuffers.memory[i].destroy();
}

// Singleton
deko3d dk3d;