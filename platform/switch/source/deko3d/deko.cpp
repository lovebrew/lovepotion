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
    this->queue = dk::QueueMaker{this->device}
                        .setFlags(DkQueueFlags_Graphics)
                        .create();

    // Create GPU & CPU Memory Pools
    this->pool.images.emplace(this->device, DkMemBlockFlags_GpuCached   | DkMemBlockFlags_Image,                            64 * 1024 * 1024);
    this->pool.data.emplace(this->device,   DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached,                        1  * 1024 * 1024);
    this->pool.code.emplace(this->device,   DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code, 128 * 1024);

    // Create the static command buffer
    this->cmdBuf = dk::CmdBufMaker{this->device}
                        .create();

    this->cmdRing.allocate(*this->pool.data, COMMAND_SIZE);

    this->CreateResources();

    this->SetViewport({0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT});
    this->SetScissor({0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT}, false);
}

void deko3d::CreateResources()
{
    // Create a Layout for the Device
    dk::ImageLayoutMaker {this->device}
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT)
        .initialize(this->layoutFramebuffer);

    uint64_t framebufferSize  = this->layoutFramebuffer.getSize();
    uint32_t framebufferAlign = this->layoutFramebuffer.getAlignment();

    for (unsigned i = 0; i < MAX_FRAMEBUFFERS; i++)
    {
        // Allocate a framebuffer
        this->framebuffers.memory[i] = this->pool.images->allocate(framebufferSize, framebufferAlign);
        this->framebuffers.images[i].initialize(this->layoutFramebuffer,
                                                this->framebuffers.memory[i].getMemBlock(),
                                                this->framebuffers.memory[i].getOffset());

        // Fill in the array for use later by the swapchain creation code
        this->framebufferArray[i] = &this->framebuffers.images[i];
    }

    // Create the swapchain using the framebuffers
    this->swapchain = dk::SwapchainMaker{this->device,
                                         nwindowGetDefault(),
                                         this->framebufferArray}.create();
}

void deko3d::EnsureInFrame()
{
    // Ensure we have begun our frame

    if (!this->framebuffers.inFrame)
    {
        this->cmdRing.begin(this->cmdBuf);
        this->framebuffers.inFrame = true;
    }
}

void deko3d::EnsureHasSlot()
{
    /*
    ** Acquire a framebuffer
    ** from the swapchain
    ** (and wait for it to be available)
    */
    if (!this->framebuffers.hasSlot)
    {
        this->framebuffers.slot = this->queue.acquireImage(this->swapchain);
        this->framebuffers.hasSlot = true;
    }
}

void deko3d::Clear(std::optional<Colorf> color, std::optional<int> stencil, std::optional<double> depth)
{
    if (color.has_value() || stencil.has_value() || depth.has_value())
        this->EnsureInFrame();

    if (color.has_value())
        this->cmdBuf.clearColor(0, DkColorMask_RGBA, color.value().r, color.value().g, color.value().b, color.value().a);

    // Figure out wtf this should be
    // if (stencil.has_value())
    //     this->cmdBuf.clearDepthStencil(false, depth.value(), DkStencilOp_Replace, stencil.value());

    // if (depth.has_value())
    // {
    //     // if (!hadDepthWrites)
    //         // gl.setDepthWrites(true);

    //     this->cmdBuf.clearDepthStencil(true, depth.value(), DkStencilOp_Replace, stencil.value());
    // }
}

dk::UniqueDevice & deko3d::GetDevice()
{
    return this->device;
}

std::optional<CMemPool> & deko3d::GetCode()
{
    return this->pool.code;
}

void deko3d::BindFramebuffer()
{
    this->EnsureHasSlot();

    // Generate a command list that binds it
    dk::ImageView colorTarget { this->framebuffers.images[this->framebuffers.slot] };
    this->cmdBuf.bindRenderTargets(&colorTarget);
}

void deko3d::Present()
{
    this->BindFramebuffer();

    // Now that we are done rendering, present it to the screen
    if (this->framebuffers.inFrame)
    {
        DkCmdList commands = this->cmdRing.end(this->cmdBuf);

        // Run the main rendering command list
        this->queue.submitCommands(commands);

        this->queue.presentImage(this->swapchain, this->framebuffers.slot);

        this->framebuffers.inFrame = false;
        this->framebuffers.hasSlot = false;
    }
}

void deko3d::UseProgram(std::pair<CShader, CShader> program)
{
    this->cmdBuf.bindShaders(DkStageFlag_GraphicsMask, {program.first, program.second});
}

void deko3d::SetFilter(const love::Texture::Filter & filter)
{
    // Set the global filter mode for textures

    // this->filter.sampler.setFilter(DkMipFilter_Linear, DkMipFilter_Linear);
    // this->filter.descriptor.initialize(this->filter.sampler);
}

void deko3d::ClearDepth(double depth)
{
    // this->cmdbuf.clearDepthStencil
}

void deko3d::SetScissor(const love::Rect & scissor, bool canvasActive)
{
    this->EnsureInFrame();

    this->scissor = scissor;
    this->cmdBuf.setScissors(0, { {scissor.x, scissor.y, scissor.w, scissor.h} });
}

void deko3d::SetViewport(const love::Rect & view)
{
    this->EnsureInFrame();

    this->viewport = view;
    this->cmdBuf.setViewports(0, { {view.x, view.y, view.w, view.h, 0.0f, 1.0f} });
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
    this->cmdBuf.clear();

    // Destroy the swapchain
    this->swapchain.destroy();

    // Destroy the framebuffers
    for (unsigned i = 0; i < MAX_FRAMEBUFFERS; i ++)
        this->framebuffers.memory[i].destroy();
}

// Singleton
deko3d dk3d;