#include "deko3d/deko.h"

#include "common/pixelformat.h"
#include "deko3d/vertex.h"

namespace
{
    /* GPU & CPU Memory Pools */
    constexpr auto gpuPoolSize = 64 * 1024 * 1024;
    constexpr auto gpuFlags    = (DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image);

    constexpr auto cpuPoolSize = 1 * 1024 * 1024;
    constexpr auto cpuFlags    = (DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached);

    /* Used for Shader code */
    constexpr auto shaderPoolSize = 128 * 1024;
    constexpr auto shaderFlags =
        (DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code);

    constexpr auto framebufferLayoutFlags =
        (DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression);

    uint32_t framebufferWidth  = 1280;
    uint32_t framebufferHeight = 720;
} // namespace

deko3d::deko3d() :
    firstVertex(0),
    renderState(STATE_MAX_ENUM),
    /*
    ** Create GPU device
    ** default origin is top left
    ** Unique -- destroys automatically
    */
    device(dk::DeviceMaker {}.setFlags(DkDeviceFlags_DepthMinusOneToOne).create()),
    /*
    ** Render Queue
    ** Unique -- destroys automatically
    */
    queue(dk::QueueMaker { this->device }.setFlags(DkQueueFlags_Graphics).create()),
    pool { .images = CMemPool(this->device, gpuFlags, gpuPoolSize),
           .data   = CMemPool(this->device, cpuFlags, cpuPoolSize),
           .code   = CMemPool(this->device, shaderFlags, shaderPoolSize) },
    state(),
    textureQueue(dk::QueueMaker { this->device }.setFlags(DkQueueFlags_Graphics).create()),
    viewport { 0, 0, static_cast<int>(framebufferWidth), static_cast<int>(framebufferHeight) },
    framebuffers(),
    descriptorsDirty(false),
    depthBuffer()
{
    this->transformUniformBuffer =
        this->pool.data.allocate(sizeof(this->transformState), DK_UNIFORM_BUF_ALIGNMENT);
    this->transformState.mdlvMtx = glm::mat4(1.0f);

    this->descriptors.image.allocate(this->pool.data);
    this->descriptors.sampler.allocate(this->pool.data);

    this->cmdRing.allocate(this->pool.data, COMMAND_SIZE);
    this->vtxRing.allocate(this->pool.data, VERTEX_COMMAND_SIZE / 2);

    this->state.depthStencil.setDepthTestEnable(true);
    this->state.depthStencil.setDepthWriteEnable(true);
    this->state.depthStencil.setDepthCompareOp(DkCompareOp_Always);

    this->state.color.setBlendEnable(0, true);

    love::Texture::Filter filter;
    filter.min = filter.mag = love::Texture::FILTER_NEAREST;
    this->SetTextureFilter(filter);

    love::Texture::Wrap wrap;
    wrap.s = wrap.t = wrap.r = love::Texture::WRAP_CLAMP;
    this->SetTextureWrap(wrap);

    // Create the dynamic command buffer
    this->cmdBuf = dk::CmdBufMaker { this->device }.create();
    this->EnsureInFrame();

    this->descriptors.image.bindForImages(this->cmdBuf);
    this->descriptors.sampler.bindForSamplers(this->cmdBuf);
}

deko3d::~deko3d()
{
    this->DestroyFramebufferResources();
    this->transformUniformBuffer.destroy();
}

deko3d& deko3d::Instance()
{
    static deko3d instance;
    return instance;
}

void deko3d::CreateFramebufferResources()
{
    /* initialize depth buffer */
    dk::ImageLayout layout_depthbuffer;
    dk::ImageLayoutMaker { this->device }
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_Z24S8)
        .setDimensions(framebufferWidth, framebufferHeight)
        .initialize(layout_depthbuffer);

    this->depthBuffer.memory =
        this->pool.images.allocate(layout_depthbuffer.getSize(), layout_depthbuffer.getAlignment());
    this->depthBuffer.image.initialize(layout_depthbuffer, this->depthBuffer.memory.getMemBlock(),
                                       this->depthBuffer.memory.getOffset());

    /* Create a layout for the normal framebuffers */
    dk::ImageLayoutMaker { this->device }
        .setFlags(framebufferLayoutFlags)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(framebufferWidth, framebufferHeight)
        .initialize(this->layoutFramebuffer);

    uint64_t framebufferSize  = this->layoutFramebuffer.getSize();
    uint32_t framebufferAlign = this->layoutFramebuffer.getAlignment();

    for (unsigned i = 0; i < MAX_FRAMEBUFFERS; i++)
    {
        // Allocate a framebuffer
        this->framebuffers.memory[i] =
            this->pool.images.allocate(framebufferSize, framebufferAlign);
        this->framebuffers.images[i].initialize(this->layoutFramebuffer,
                                                this->framebuffers.memory[i].getMemBlock(),
                                                this->framebuffers.memory[i].getOffset());

        // Fill in the array for use later by the swapchain creation code
        this->framebufferArray[i] = &this->framebuffers.images[i];
    }

    // Create the swapchain using the framebuffers
    this->swapchain =
        dk::SwapchainMaker { this->device, nwindowGetDefault(), this->framebufferArray }.create();
}

std::pair<uint32_t, uint32_t> deko3d::OnOperationMode(AppletOperationMode mode)
{
    this->DestroyFramebufferResources();

    switch (mode)
    {
        default:
        case AppletOperationMode_Handheld:
        {
            framebufferWidth  = 1280;
            framebufferHeight = 720;
            break;
        }
        case AppletOperationMode_Console:
        {
            framebufferWidth  = 1920;
            framebufferHeight = 1080;
            break;
        }
    }

    this->CreateFramebufferResources();

    return { framebufferWidth, framebufferHeight };
}

void deko3d::DestroyFramebufferResources()
{
    // Return early if we have nothing to destroy
    if (!this->swapchain)
        return;

    // Make sure the queue is idle before destroying anything
    this->queue.waitIdle();

    this->textureQueue.waitIdle();

    // Clear the cmdbuf
    this->cmdBuf.clear();

    // Destroy the swapchain
    this->swapchain.destroy();

    // Destroy the framebuffers
    for (unsigned i = 0; i < MAX_FRAMEBUFFERS; i++)
        this->framebuffers.memory[i].destroy();

    this->depthBuffer.memory.destroy();
}

// Ensure we have begun our frame
void deko3d::EnsureInFrame()
{
    if (!this->framebuffers.inFrame)
    {
        this->firstVertex      = 0;
        this->descriptorsDirty = false;
        this->cmdRing.begin(this->cmdBuf);
        this->framebuffers.inFrame = true;
    }
}

void deko3d::EnsureInState(State state)
{
    if (this->renderState != state && state != State::STATE_MAX_ENUM)
        this->renderState = state;

    if (this->renderState == STATE_PRIMITIVE)
    {
        love::Shader::standardShaders[love::Shader::STANDARD_DEFAULT]->Attach();

        this->cmdBuf.bindVtxAttribState(vertex::attributes::PrimitiveAttribState);
        this->cmdBuf.bindVtxBufferState(vertex::attributes::PrimitiveBufferState);
    }
    else if (this->renderState == STATE_TEXTURE)
    {
        love::Shader::standardShaders[love::Shader::STANDARD_TEXTURE]->Attach();

        this->cmdBuf.bindVtxAttribState(vertex::attributes::TextureAttribState);
        this->cmdBuf.bindVtxBufferState(vertex::attributes::TextureBufferState);
    }
}

/*
** Acquire a framebuffer from the swapchain
** (and wait for it to be available)
*/
void deko3d::EnsureHasSlot()
{
    if (this->framebuffers.slot < 0)
        this->framebuffers.slot = this->queue.acquireImage(this->swapchain);
}

void deko3d::SetBlendColor(const Colorf& color)
{
    this->cmdBuf.setBlendConst(color.r, color.g, color.b, color.a);
}

/*
** First thing that happens to start the frame
** Clear the screen to a specified color
*/
void deko3d::ClearColor(const Colorf& color)
{
    this->EnsureInFrame();

    this->cmdBuf.clearColor(0, DkColorMask_RGBA, color.r, color.g, color.b, color.a);
}

void deko3d::ClearDepthStencil(double depth, int stencil)
{
    // this->cmdBuf.clearDepthStencil(true, depth, 0xFF, stencil);
}

void deko3d::SetDekoBarrier(DkBarrier barrier, uint32_t flags)
{
    this->EnsureInFrame();
    this->cmdBuf.barrier(barrier, flags);
}

/*
** Binds a Framebuffer we have allocated
** It ensures that there's a "slot" from @EnsureHasSlot
** This is used to access the current Framebuffer image
** TODO: Add depth/stencil images
*/
void deko3d::BindFramebuffer(love::Canvas* canvas)
{
    if (!this->swapchain)
        return;

    this->EnsureInFrame();
    this->EnsureHasSlot();

    if (this->framebuffers.dirty)
        this->SetDekoBarrier(DkBarrier_Fragments, 0);

    dk::ImageView target { this->framebuffers.images[this->framebuffers.slot] };

    if (canvas != nullptr)
    {
        target = { canvas->GetImage() };
        this->SetViewport({ 0, 0, canvas->GetWidth(), canvas->GetHeight() });

        this->framebuffers.dirty = true;
    }
    else
    {
        this->framebuffers.dirty = false;
        this->SetViewport({ 0, 0, this->viewport.w, this->viewport.h });
    }

    this->cmdBuf.bindRenderTargets(&target);

    this->cmdBuf.pushConstants(this->transformUniformBuffer.getGpuAddr(),
                               this->transformUniformBuffer.getSize(), 0, sizeof(transformState),
                               &transformState);

    this->BeginFrame();
}

void deko3d::BeginFrame()
{
    std::pair<void*, DkGpuAddr> data = this->vtxRing.begin();

    this->vertexData = (vertex::Vertex*)data.first;

    this->cmdBuf.bindRasterizerState(this->state.rasterizer);
    this->cmdBuf.bindColorState(this->state.color);
    this->cmdBuf.bindColorWriteState(this->state.colorWrite);
    this->cmdBuf.bindBlendStates(0, this->state.blendState);
    // this->cmdBuf.bindDepthStencilState(this->state.depthStencil);

    // Bind the current slice's GPU address to the buffer
    this->cmdBuf.bindVtxBuffer(0, data.second, this->vtxRing.getSize());
}

/*
** Presents the current Framebuffer to the screen
** and submits all our commands from the buffer
** to the queue
*/
void deko3d::Present()
{
    if (!this->swapchain)
        return;

    if (this->framebuffers.inFrame)
    {
        this->vtxRing.end();
        this->queue.submitCommands(this->cmdRing.end(this->cmdBuf));
        this->queue.presentImage(this->swapchain, this->framebuffers.slot);

        this->framebuffers.inFrame = false;
    }

    this->framebuffers.slot = -1;
}

void deko3d::SetStencil(DkStencilOp op, DkCompareOp compare, int value)
{
    bool enabled = (compare == DkCompareOp_Always) ? false : true;

    this->state.depthStencil.setStencilTestEnable(enabled);

    // Front

    this->state.depthStencil.setStencilFrontCompareOp(compare);

    this->state.depthStencil.setStencilFrontDepthFailOp(DkStencilOp_Keep);
    this->state.depthStencil.setStencilFrontFailOp(DkStencilOp_Keep);
    this->state.depthStencil.setStencilFrontPassOp(DkStencilOp_Keep);

    // Back

    this->state.depthStencil.setStencilBackCompareOp(compare);

    this->state.depthStencil.setStencilBackDepthFailOp(DkStencilOp_Keep);
    this->state.depthStencil.setStencilBackFailOp(DkStencilOp_Keep);
    this->state.depthStencil.setStencilBackPassOp(DkStencilOp_Keep);

    this->cmdBuf.setStencil(DkFace_FrontAndBack, 0xFF, value, 0xFF);
}

void deko3d::UnRegisterResHandle(DkResHandle handle)
{
    this->allocator.DeAllocate(handle);
}

DkResHandle deko3d::RegisterResHandle(const dk::ImageDescriptor& descriptor)
{
    this->EnsureInFrame();

    uint32_t index = this->allocator.Allocate();

    this->descriptors.image.update(this->cmdBuf, index, descriptor);
    this->descriptors.sampler.update(this->cmdBuf, index, this->filter.descriptor);

    this->descriptorsDirty = true;

    return dkMakeTextureHandle(index, index);
}

bool deko3d::RenderTexture(const DkResHandle handle, const vertex::Vertex* points, size_t count)
{
    if (count > (this->vtxRing.getSize() - this->firstVertex) || points == nullptr)
        return false;

    this->EnsureInState(STATE_TEXTURE);

    if (this->descriptorsDirty)
    {
        this->cmdBuf.barrier(DkBarrier_Primitives, DkInvalidateFlags_Descriptors);
        this->descriptorsDirty = false;
    }

    this->cmdBuf.bindTextures(DkStage_Fragment, 0, handle);

    memcpy(this->vertexData + this->firstVertex, points, count * sizeof(vertex::Vertex));

    this->cmdBuf.draw(DkPrimitive_Quads, count, 1, this->firstVertex, 0);

    this->firstVertex += count;

    return true;
}

bool deko3d::RenderPolyline(DkPrimitive mode, const vertex::Vertex* points, size_t count)
{
    if (count > (this->vtxRing.getSize() - this->firstVertex) || points == nullptr)
        return false;

    this->EnsureInState(STATE_PRIMITIVE);

    memcpy(this->vertexData + this->firstVertex, points, count * sizeof(vertex::Vertex));

    this->cmdBuf.draw(mode, count, 1, this->firstVertex, 0);

    this->firstVertex += count;

    return true;
}

bool deko3d::RenderPolygon(const vertex::Vertex* points, size_t count)
{
    if (count > (this->vtxRing.getSize() - this->firstVertex) || points == nullptr)
        return false;

    this->EnsureInState(STATE_PRIMITIVE);

    memcpy(this->vertexData + this->firstVertex, points, count * sizeof(vertex::Vertex));

    this->cmdBuf.draw(DkPrimitive_TriangleFan, count, 1, this->firstVertex, 0);

    this->firstVertex += count;

    return true;
}

bool deko3d::RenderPoints(const vertex::Vertex* points, size_t count)
{
    if (count > (this->vtxRing.getSize() - this->firstVertex) || points == nullptr)
        return false;

    this->EnsureInState(STATE_PRIMITIVE);

    memcpy(this->vertexData + this->firstVertex, points, count * sizeof(vertex::Vertex));

    this->cmdBuf.draw(DkPrimitive_Points, count, 1, this->firstVertex, 0);

    this->firstVertex += count;

    return true;
}

void deko3d::SetPointSize(float size)
{
    this->EnsureInFrame();
    this->cmdBuf.setPointSize(size);
}

void deko3d::SetLineWidth(float width)
{
    this->EnsureInFrame();
    this->cmdBuf.setLineWidth(width);
}

void deko3d::SetLineStyle(bool smooth)
{
    this->state.rasterizer.setPolygonSmoothEnable(smooth);
}

float deko3d::GetPointSize()
{
    return this->state.pointSize;
}

void deko3d::SetColorMask(const love::Graphics::ColorMask& mask)
{
    this->state.colorWrite.setMask(0, mask.GetColorMask());
}

void deko3d::SetBlendMode(DkBlendOp func, DkBlendFactor srcColor, DkBlendFactor srcAlpha,
                          DkBlendFactor dstColor, DkBlendFactor dstAlpha)
{
    this->state.blendState.setColorBlendOp(func);
    this->state.blendState.setAlphaBlendOp(func);

    // Blend factors
    this->state.blendState.setSrcColorBlendFactor(srcColor);
    this->state.blendState.setSrcAlphaBlendFactor(srcAlpha);

    this->state.blendState.setDstColorBlendFactor(dstColor);
    this->state.blendState.setDstAlphaBlendFactor(dstAlpha);
}

void deko3d::SetFrontFaceWinding(DkFrontFace face)
{
    this->state.rasterizer.setFrontFace(face);
}

void deko3d::SetCullMode(DkFace face)
{
    this->state.rasterizer.setCullMode(face);
}

/* Encapsulation and Abstraction - fincs */

/*
** Equivalent to LÖVE's OpenGL::UseProgram function
** We need to make sure that we're *in* a frame before attaching the Shader
** LÖVE attaches a default Shader on Graphics creation, which is not in a frame
** although OpenGL *hides* this from the user, so we have to deal with it in deko3d
*/
void deko3d::UseProgram(const love::Shader::Program& program)
{
    this->EnsureInFrame();

    this->cmdBuf.bindShaders(DkStageFlag_GraphicsMask, { *program.vertex, *program.fragment });
    this->cmdBuf.bindUniformBuffer(DkStage_Vertex, 0, this->transformUniformBuffer.getGpuAddr(),
                                   this->transformUniformBuffer.getSize());
}

void deko3d::SetDepthWrites(bool enable)
{
    this->state.rasterizer.setDepthClampEnable(enable);
}

// Set the global filter mode for textures
void deko3d::SetTextureFilter(const love::Texture::Filter& filter)
{
    DkFilter min =
        (filter.min == love::Texture::FILTER_NEAREST) ? DkFilter_Nearest : DkFilter_Linear;
    DkFilter mag =
        (filter.min == love::Texture::FILTER_NEAREST) ? DkFilter_Nearest : DkFilter_Linear;

    DkMipFilter mipFilter = DkMipFilter_Linear;
    if (filter.mipmap != love::Texture::FILTER_NONE)
    {
        if (filter.min == love::Texture::FILTER_NEAREST &&
            filter.mipmap == love::Texture::FILTER_NEAREST)
            mipFilter = DkMipFilter_Nearest;
        else if (filter.min == love::Texture::FILTER_NEAREST &&
                 filter.mipmap == love::Texture::FILTER_LINEAR)
            mipFilter = DkMipFilter_Linear;
        else if (filter.min == love::Texture::FILTER_LINEAR &&
                 filter.mipmap == love::Texture::FILTER_NEAREST)
            mipFilter = DkMipFilter_Nearest;
        else if (filter.min == love::Texture::FILTER_LINEAR &&
                 filter.mipmap == love::Texture::FILTER_LINEAR)
            mipFilter = DkMipFilter_Linear;
        else
            mipFilter = DkMipFilter_Linear;
    }

    this->filter.sampler.setFilter(min, mag, mipFilter);

    float anisotropy = std::clamp(filter.anisotropy, 1.0f, (float)MAX_ANISOTROPY);
    this->filter.sampler.setMaxAnisotropy(anisotropy);

    this->filter.descriptor.initialize(this->filter.sampler);
}

void deko3d::SetTextureFilter(love::Texture* texture, const love::Texture::Filter& filter)
{
    this->EnsureInFrame();

    this->SetTextureFilter(filter);

    uint32_t handleID = this->allocator.Find(texture->GetHandle());
    this->descriptors.sampler.update(this->cmdBuf, handleID, this->filter.descriptor);

    this->descriptorsDirty = true;
}

void deko3d::SetTextureWrap(const love::Texture::Wrap& wrap)
{
    DkWrapMode u = deko3d::GetDekoWrapMode(wrap.s);
    DkWrapMode v = deko3d::GetDekoWrapMode(wrap.t);

    this->filter.sampler.setWrapMode(u, v);

    this->filter.descriptor.initialize(this->filter.sampler);
}

void deko3d::SetTextureWrap(love::Texture* texture, const love::Texture::Wrap& wrap)
{
    this->EnsureInFrame();

    this->SetTextureWrap(wrap);

    uint32_t handleID = this->allocator.Find(texture->GetHandle());
    this->descriptors.sampler.update(this->cmdBuf, handleID, this->filter.descriptor);

    this->descriptorsDirty = true;
}

DkWrapMode deko3d::GetDekoWrapMode(love::Texture::WrapMode wrap)
{
    switch (wrap)
    {
        case love::Texture::WRAP_CLAMP:
        default:
            return DkWrapMode_ClampToEdge;
        case love::Texture::WRAP_CLAMP_ZERO:
            return DkWrapMode_ClampToBorder;
        case love::Texture::WRAP_REPEAT:
            return DkWrapMode_Repeat;
        case love::Texture::WRAP_MIRRORED_REPEAT:
            return DkWrapMode_MirroredRepeat;
    }
}

bool deko3d::GetConstant(PixelFormat in, DkImageFormat& out)
{
    return pixelFormats.Find(in, out);
}

bool deko3d::GetConstant(DkImageFormat in, PixelFormat& out)
{
    return pixelFormats.Find(in, out);
}

// clang-format off
constexpr EnumMap<love::PixelFormat, DkImageFormat, PIXELFORMAT_MAX_ENUM>::Entry formatEntries[] =
{
    { PIXELFORMAT_RGBA8,      DkImageFormat_RGBA8_Unorm     },
    { PIXELFORMAT_DXT1,       DkImageFormat_RGBA_BC1        },
    { PIXELFORMAT_DXT3,       DkImageFormat_RGBA_BC2        },
    { PIXELFORMAT_DXT5,       DkImageFormat_RGBA_BC3        },
    { PIXELFORMAT_ETC1,       DkImageFormat_RGB_ETC2        },
    { PIXELFORMAT_ETC2_RGB,   DkImageFormat_RGB_ETC2        },
    { PIXELFORMAT_ETC2_RGBA1, DkImageFormat_RGBA_ETC2       },
    { PIXELFORMAT_ETC2_RGBA,  DkImageFormat_RGBA_ETC2       },
    { PIXELFORMAT_ASTC_4x4,   DkImageFormat_RGBA_ASTC_4x4   },
    { PIXELFORMAT_ASTC_5x4,   DkImageFormat_RGBA_ASTC_5x4   },
    { PIXELFORMAT_ASTC_6x5,   DkImageFormat_RGBA_ASTC_6x5   },
    { PIXELFORMAT_ASTC_6x6,   DkImageFormat_RGBA_ASTC_6x6   },
    { PIXELFORMAT_ASTC_8x5,   DkImageFormat_RGBA_ASTC_8x5   },
    { PIXELFORMAT_ASTC_8x6,   DkImageFormat_RGBA_ASTC_8x6   },
    { PIXELFORMAT_ASTC_8x8,   DkImageFormat_RGBA_ASTC_8x8   },
    { PIXELFORMAT_ASTC_10x5,  DkImageFormat_RGBA_ASTC_10x5  },
    { PIXELFORMAT_ASTC_10x6,  DkImageFormat_RGBA_ASTC_10x6  },
    { PIXELFORMAT_ASTC_10x8,  DkImageFormat_RGBA_ASTC_10x8  },
    { PIXELFORMAT_ASTC_10x10, DkImageFormat_RGBA_ASTC_10x10 },
    { PIXELFORMAT_ASTC_12x10, DkImageFormat_RGBA_ASTC_12x10 },
    { PIXELFORMAT_ASTC_12x12, DkImageFormat_RGBA_ASTC_12x12 },
};

constinit const EnumMap<love::PixelFormat, DkImageFormat, PIXELFORMAT_MAX_ENUM> deko3d::pixelFormats(formatEntries);
// clang-format on

/*
** Set the Scissor region to clip
** Anything drawn outside of this will not be rendered
*/
void deko3d::SetScissor(const love::Rect& scissor, bool canvasActive)
{
    this->EnsureInFrame();

    this->scissor = scissor;
    this->cmdBuf.setScissors(0, { { (uint32_t)scissor.x, (uint32_t)scissor.y, (uint32_t)scissor.w,
                                    (uint32_t)scissor.h } });
}

/*
** Set the viewing screen space for rendering
** This sets up the actual bounds we can see
*/
void deko3d::SetViewport(const love::Rect& view)
{
    this->EnsureInFrame();

    this->viewport = view;
    this->cmdBuf.setViewports(
        0, { { (float)view.x, (float)view.y, (float)view.w, (float)view.h, Z_NEAR, Z_FAR } });

    this->transformState.projMtx =
        glm::ortho(0.0f, (float)view.w, (float)view.h, 0.0f, Z_NEAR, Z_FAR);
}

love::Rect deko3d::GetViewport()
{
    return this->viewport;
}
