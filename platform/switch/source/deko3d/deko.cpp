#include "deko3d/deko.h"
#include "deko3d/vertexattribs.h"

#include "common/screen.h"

#include "objects/canvas/canvas.h"

using namespace love;

deko3d::deko3d() :
    firstVertex(0),
    gpuRenderState(GPU_RENDER_STATE_MAX_ENUM),
    vertexData(nullptr),
    device(dk::DeviceMaker {}.setFlags(DkDeviceFlags_DepthMinusOneToOne).create()),
    queue(dk::QueueMaker { this->device }.setFlags(DkQueueFlags_Graphics).create()),
    textureQueue(dk::QueueMaker { this->device }.setFlags(DkQueueFlags_Graphics).create()),
    commandBuffer {},
    swapchain {},
    pool { .images = CMemPool(this->device, GPU_USE_FLAGS, GPU_POOL_SIZE),
           .data   = CMemPool(this->device, CPU_USE_FLAGS, CPU_POOL_SIZE),
           .code   = CMemPool(this->device, SHADER_USE_FLAGS, SHADER_POOL_SIZE) },
    deviceState {},
    framebuffers {},
    depthBuffer {},
    transform {},
    descriptors {},
    filter {}
{
    this->transform.buffer =
        this->AllocatePool(MEMPOOL_DATA, TRANSFORM_SIZE, DK_UNIFORM_BUF_ALIGNMENT);
    this->transform.state.mdlvMtx = glm::mat4(1.0f);

    this->descriptors.image.allocate(this->pool.data);
    this->descriptors.sampler.allocate(this->pool.data);

    /* command ring */
    this->commands.allocate(this->pool.data, COMMAND_SIZE);
    this->vertices.allocate(this->pool.data, VERTEX_COMMAND_SIZE / 2);

    this->deviceState.depthStencil.setDepthTestEnable(true);
    this->deviceState.depthStencil.setDepthWriteEnable(true);
    this->deviceState.depthStencil.setDepthCompareOp(DkCompareOp_Always);

    this->deviceState.color.setBlendEnable(0, true);

    this->filter.sampler.setFilter(DkFilter_Linear, DkFilter_Linear);
    this->filter.sampler.setWrapMode(DkWrapMode_Clamp, DkWrapMode_Clamp);
    this->filter.descriptor.initialize(this->filter.sampler);

    /* create the dynamic command buffer */
    this->commandBuffer = dk::CmdBufMaker { this->device }.create();
    this->EnsureInFrame();

    this->descriptors.image.bindForImages(this->commandBuffer);
    this->descriptors.sampler.bindForSamplers(this->commandBuffer);

    this->InitRendererInfo();
}

deko3d::~deko3d()
{
    this->DestroyFramebuffers();
    this->transform.buffer.destroy();
}

deko3d& deko3d::Instance()
{
    static deko3d instance;
    return instance;
}

CMemPool& deko3d::GetMemPool(MemPoolType type)
{
    switch (type)
    {
        case MemPoolType::MEMPOOL_DATA:
            return this->pool.data;
        case MemPoolType::MEMPOOL_CODE:
            return this->pool.code;
        case MemPoolType::MEMPOOL_IMAGES:
        default:
            return this->pool.images;
    }
}

CMemPool::Handle deko3d::AllocatePool(MemPoolType type, size_t size, uint32_t alignment)
{
    return this->GetMemPool(type).allocate(size, alignment);
}

DkResHandle deko3d::RegisterDescriptor(const dk::ImageDescriptor& descriptor)
{
    this->EnsureInFrame();

    uint32_t index = this->allocator.Allocate();

    this->descriptors.image.update(this->commandBuffer, index, descriptor);
    this->descriptors.sampler.update(this->commandBuffer, index, this->filter.descriptor);

    this->descriptors.dirty = true;

    return dkMakeTextureHandle(index, index);
}

void deko3d::DeRegisterDescriptor(DkResHandle handle)
{
    this->allocator.DeAllocate(handle);
}

void deko3d::InitRendererInfo()
{
    memset(&this->rendererInfo, 0, sizeof(RendererInfo));

    this->rendererInfo.device  = RENDERER_DEVICE;
    this->rendererInfo.name    = RENDERER_NAME;
    this->rendererInfo.vendor  = RENDERER_VENDOR;
    this->rendererInfo.version = RENDERER_VERSION;
}

void deko3d::OnOperationMode(Window::DisplaySize& size)
{
    /* destroy resources */
    this->DestroyFramebuffers();

    /* recreate them, Screen will auto-determine the sizes */
    this->CreateFramebuffers();

    size = { Screen::Instance().GetWidth(), Screen::Instance().GetHeight() };
}

bool deko3d::IsHandheldMode()
{
    return appletGetOperationMode() == AppletOperationMode_Handheld;
}

void deko3d::DestroyFramebuffers()
{
    /* return early if we have nothing to destroy */
    if (!this->swapchain)
        return;

    /* make sure the queue is idle before destroying anything */
    this->queue.waitIdle();

    this->textureQueue.waitIdle();

    /* clear the command buffer */
    this->commandBuffer.clear();

    /* Destroy the swapchain */
    this->swapchain.destroy();

    // Destroy the framebuffers
    for (unsigned i = 0; i < MAX_FRAMEBUFFERS; i++)
        this->framebuffers.memory[i].destroy();

    this->depthBuffer.memory.destroy();
}

void deko3d::CreateFramebuffers()
{
    /* initialize depth buffer */
    dk::ImageLayout layoutDepthBuffer;
    dk::ImageLayoutMaker { this->device }
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_Z24S8)
        .setDimensions(Screen::Instance().GetWidth(), Screen::Instance().GetHeight())
        .initialize(layoutDepthBuffer);

    this->depthBuffer.memory = this->AllocatePool(MEMPOOL_IMAGES, layoutDepthBuffer.getSize(),
                                                  layoutDepthBuffer.getAlignment());
    this->depthBuffer.image.initialize(layoutDepthBuffer, this->depthBuffer.memory.getMemBlock(),
                                       this->depthBuffer.memory.getOffset());

    /* create a layout for the normal framebuffers */
    dk::ImageLayoutMaker { this->device }
        .setFlags(FRAMEBUFFER_USE_FLAGS)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(Screen::Instance().GetWidth(), Screen::Instance().GetHeight())
        .initialize(this->layoutFramebuffer);

    uint64_t framebufferSize  = this->layoutFramebuffer.getSize();
    uint32_t framebufferAlign = this->layoutFramebuffer.getAlignment();

    for (unsigned i = 0; i < MAX_FRAMEBUFFERS; i++)
    {
        // Allocate a framebuffer
        this->framebuffers.memory[i] =
            this->AllocatePool(MEMPOOL_IMAGES, framebufferSize, framebufferAlign);
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

void deko3d::Clear(const Colorf& color)
{
    this->EnsureInFrame();
    this->commandBuffer.clearColor(0, DkColorMask_RGBA, color.r, color.g, color.b, color.a);
}

void deko3d::ClearDepthStencil(int stencil, uint8_t mask, double depth)
{
    this->EnsureInFrame();
    this->commandBuffer.clearDepthStencil(true, depth, mask, stencil);
}

void deko3d::SetBlendColor(const Colorf& color)
{
    this->EnsureInFrame();
    this->commandBuffer.setBlendConst(color.r, color.g, color.b, color.a);
}

void deko3d::EnsureInFrame()
{
    if (!this->framebuffers.inFrame)
    {
        this->firstVertex       = 0;
        this->descriptors.dirty = false;
        this->commands.begin(this->commandBuffer);

        this->framebuffers.inFrame = true;
    }
}

void deko3d::EnsureHasSlot()
{
    if (this->framebuffers.slot < 0)
        this->framebuffers.slot = this->queue.acquireImage(this->swapchain);
}

void deko3d::BindFramebuffer(Canvas* canvas)
{
    if (!this->swapchain)
        return;

    this->EnsureInFrame();
    this->EnsureHasSlot();

    if (this->framebuffers.dirty)
        this->commandBuffer.barrier(DkBarrier_Fragments, 0);

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
        this->SetViewport({ 0, 0, Screen::Instance().GetWidth(), Screen::Instance().GetHeight() });
    }

    this->commandBuffer.bindRenderTargets(&target);
    this->commandBuffer.pushConstants(this->transform.buffer.getGpuAddr(),
                                      this->transform.buffer.getSize(), 0, TRANSFORM_SIZE,
                                      &this->transform.state);

    /* begin the vertex ring */
    std::pair<void*, DkGpuAddr> data = this->vertices.begin();
    this->vertexData                 = (Vertex::PrimitiveVertex*)data.first;

    this->commandBuffer.bindRasterizerState(this->deviceState.rasterizer);
    this->commandBuffer.bindColorState(this->deviceState.color);
    this->commandBuffer.bindColorWriteState(this->deviceState.colorWrite);
    this->commandBuffer.bindBlendStates(0, this->deviceState.blendState);
    // this->cmdBuf.bindDepthStencilState(this->state.depthStencil);

    // Bind the current slice's GPU address to the buffer
    this->commandBuffer.bindVtxBuffer(0, data.second, this->vertices.getSize());
}

void deko3d::CheckDescriptorsDirty()
{
    if (this->descriptors.dirty)
    {
        this->commandBuffer.barrier(DkBarrier_Primitives, DkInvalidateFlags_Descriptors);
        this->descriptors.dirty = false;
    }
}

void deko3d::SetAttributes(const VertexAttributes::Attribs& attributes)
{
    this->commandBuffer.bindVtxAttribState(attributes.attributeState);
    this->commandBuffer.bindVtxBufferState(attributes.bufferState);
}

void deko3d::SetShader(Shader* shader)
{
    this->EnsureInFrame();

    const Shader::Program& program = shader->GetProgram();

    this->commandBuffer.bindShaders(DkStageFlag_GraphicsMask,
                                    { *program.vertex, *program.fragment });

    this->commandBuffer.bindUniformBuffer(DkStage_Vertex, 0, this->transform.buffer.getGpuAddr(),
                                          this->transform.buffer.getSize());

    deko3d::shaderSwitches++;
}

void deko3d::Render(const StreamDrawState& state)
{
    if (state.count > (this->vertices.getSize() - this->firstVertex))
        return;

    DkPrimitive primitive;
    ::deko3d::GetConstant(state.primitveMode, primitive);

    if (!Shader::IsActive(state.shaderType))
        Shader::standardShaders[state.shaderType]->Attach();

    VertexAttributes::Attribs attributes {};
    VertexAttributes::GetAttributes(state.vertexFormat, attributes);

    this->SetAttributes(attributes);

    if (!state.textureHandles.empty())
        this->commandBuffer.bindTextures(DkStage_Fragment, 0, state.textureHandles);

    memcpy(this->vertexData + this->firstVertex, state.vertices.data(), state.size);
    this->commandBuffer.draw(primitive, state.count, 1, this->firstVertex, 0);

    this->firstVertex += state.count;
    deko3d::drawCalls++;
}

void deko3d::Present()
{
    if (!this->swapchain)
        return;

    if (this->framebuffers.inFrame)
    {
        this->vertices.end();
        this->queue.submitCommands(this->commands.end(this->commandBuffer));
        this->queue.presentImage(this->swapchain, this->framebuffers.slot);

        this->framebuffers.inFrame = false;
    }

    deko3d::shaderSwitches = 0;
    deko3d::drawCalls      = 0;

    this->framebuffers.slot = -1;
}

void deko3d::SetViewport(const Rect& viewport)
{
    this->EnsureInFrame();

    this->commandBuffer.setViewports(0, { { (float)viewport.x, (float)viewport.y, (float)viewport.w,
                                            (float)viewport.h, Z_NEAR, Z_FAR } });

    this->transform.state.projMtx =
        glm::ortho(0.0f, (float)viewport.w, (float)viewport.h, 0.0f, Z_NEAR, Z_FAR);

    this->viewport = viewport;
}

Rect deko3d::GetViewport() const
{
    return this->viewport;
}

void deko3d::SetScissor(bool enable, const love::Rect& scissor, bool canvasActive)
{
    this->EnsureInFrame();

    this->commandBuffer.setScissors(0, { { (uint32_t)scissor.x, (uint32_t)scissor.y,
                                           (uint32_t)scissor.w, (uint32_t)scissor.h } });
}

/* todo: missing something? */
void deko3d::SetStencil(RenderState::CompareMode compare, int value)
{
    bool enabled = (compare == RenderState::COMPARE_ALWAYS) ? false : true;

    DkCompareOp operation;
    ::deko3d::GetConstant(compare, operation);

    this->deviceState.depthStencil.setDepthTestEnable(enabled);
    this->deviceState.depthStencil.setDepthCompareOp(operation);
}

void deko3d::SetMeshCullMode(Vertex::CullMode mode)
{
    DkFace cullMode;
    ::deko3d::GetConstant(mode, cullMode);

    this->deviceState.rasterizer.setCullMode(cullMode);
}

void deko3d::SetVertexWinding(Vertex::Winding winding)
{
    DkFrontFace frontFace;
    ::deko3d::GetConstant(winding, frontFace);

    this->deviceState.rasterizer.setFrontFace(frontFace);
}

void deko3d::SetLineWidth(float lineWidth)
{
    this->EnsureInFrame();
    this->commandBuffer.setLineWidth(lineWidth);
}

void deko3d::SetLineStyle(Graphics::LineStyle style)
{
    bool smooth = (style == Graphics::LineStyle::LINE_SMOOTH);
    this->deviceState.rasterizer.setPolygonSmoothEnable(smooth);
}

void deko3d::SetPointSize(float pointSize)
{
    this->EnsureInFrame();
    this->commandBuffer.setPointSize(pointSize);
}

void deko3d::SetSamplerState(Texture* texture, SamplerState& state)
{
    this->EnsureInFrame();

    DkFilter min;
    ::deko3d::GetConstant(state.minFilter, min);

    DkFilter mag;
    ::deko3d::GetConstant(state.magFilter, mag);

    DkMipFilter mipFilter;

    if (state.mipmapFilter != SamplerState::MIPMAP_FILTER_NONE)
    {
        if (state.minFilter == SamplerState::FILTER_NEAREST &&
            state.mipmapFilter == SamplerState::MIPMAP_FILTER_NEAREST)
        {
            mipFilter = DkMipFilter_Nearest;
        }
        else if (state.minFilter == SamplerState::FILTER_NEAREST &&
                 state.mipmapFilter == SamplerState::MIPMAP_FILTER_LINEAR)
        {
            mipFilter = DkMipFilter_Linear;
        }
        else if (state.magFilter == SamplerState::FILTER_LINEAR &&
                 state.mipmapFilter == SamplerState::MIPMAP_FILTER_NEAREST)
        {
            mipFilter = DkMipFilter_Nearest;
        }
        else if (state.magFilter == SamplerState::FILTER_LINEAR &&
                 state.mipmapFilter == SamplerState::MIPMAP_FILTER_LINEAR)
        {
            mipFilter = DkMipFilter_Linear;
        }
    }

    this->filter.sampler.setFilter(min, mag, mipFilter);

    float anisotropy = std::clamp((float)state.maxAnisotropy, 1.0f, (float)MAX_ANISOTROPY);
    this->filter.sampler.setMaxAnisotropy(anisotropy);

    this->filter.descriptor.initialize(this->filter.sampler);

    if (SamplerState::IsClampZeroOrOne(state.wrapU))
        state.wrapU = SamplerState::WRAP_CLAMP;

    if (SamplerState::IsClampZeroOrOne(state.wrapV))
        state.wrapV = SamplerState::WRAP_CLAMP;

    DkWrapMode wrapU;
    ::deko3d::GetConstant(state.wrapU, wrapU);

    DkWrapMode wrapV;
    ::deko3d::GetConstant(state.wrapV, wrapV);

    DkWrapMode wrapW;
    ::deko3d::GetConstant(state.wrapW, wrapW);

    this->filter.sampler.setWrapMode(wrapU, wrapV, wrapW);
    this->filter.descriptor.initialize(this->filter.sampler);
}

void deko3d::SetColorMask(const RenderState::ColorMask& mask)
{
    this->deviceState.colorWrite.setMask(0, mask.GetColorMask());
}

void deko3d::SetBlendMode(const RenderState::BlendState& state)
{
    DkBlendOp opRGB;
    deko3d::GetConstant(state.operationRGB, opRGB);

    DkBlendOp opAlpha;
    deko3d::GetConstant(state.operationA, opAlpha);

    DkBlendFactor srcColor;
    deko3d::GetConstant(state.srcFactorRGB, srcColor);

    DkBlendFactor srcAlpha;
    deko3d::GetConstant(state.srcFactorA, srcAlpha);

    DkBlendFactor dstColor;
    deko3d::GetConstant(state.dstFactorRGB, dstColor);

    DkBlendFactor dstAlpha;
    deko3d::GetConstant(state.dstFactorA, dstAlpha);

    this->deviceState.blendState.setColorBlendOp(opRGB);
    this->deviceState.blendState.setAlphaBlendOp(opAlpha);

    // Blend factors
    this->deviceState.blendState.setSrcColorBlendFactor(srcColor);
    this->deviceState.blendState.setSrcAlphaBlendFactor(srcAlpha);

    this->deviceState.blendState.setDstColorBlendFactor(dstColor);
    this->deviceState.blendState.setDstAlphaBlendFactor(dstAlpha);
}

// clang-format off
constexpr auto pixelFormats = BidirectionalMap<>::Create(
    PIXELFORMAT_R8_UNORM,         DkImageFormat_R8_Unorm,
    PIXELFORMAT_R16_UNORM,        DkImageFormat_R16_Unorm,
    PIXELFORMAT_RG8_UNORM,        DkImageFormat_RG8_Unorm,
    PIXELFORMAT_RGBA8_UNORM,      DkImageFormat_RGBA8_Unorm,
    PIXELFORMAT_RGB565_UNORM,     DkImageFormat_RGB565_Unorm,
    PIXELFORMAT_RGBA8_UNORM_SRGB, DkImageFormat_RGBA8_Unorm_sRGB,
    PIXELFORMAT_DXT1_UNORM,       DkImageFormat_RGBA_BC1,
    PIXELFORMAT_DXT3_UNORM,       DkImageFormat_RGBA_BC2,
    PIXELFORMAT_DXT5_UNORM,       DkImageFormat_RGBA_BC3,
    PIXELFORMAT_ETC1_UNORM,       DkImageFormat_RGB_ETC2,
    PIXELFORMAT_ETC2_RGB_UNORM,   DkImageFormat_RGB_ETC2,
    PIXELFORMAT_ETC2_RGBA1_UNORM, DkImageFormat_RGBA_ETC2,
    PIXELFORMAT_ETC2_RGBA_UNORM,  DkImageFormat_RGBA_ETC2,
    PIXELFORMAT_BC4_UNORM,        DkImageFormat_R_BC4_Unorm,
    PIXELFORMAT_BC5_UNORM,        DkImageFormat_RG_BC5_Unorm,
    PIXELFORMAT_BC7_UNORM,        DkImageFormat_RGBA_BC7_Unorm,
    PIXELFORMAT_BC7_UNORM_SRGB,   DkImageFormat_RGBA_BC7_Unorm_sRGB,
    PIXELFORMAT_ASTC_4x4,         DkImageFormat_RGBA_ASTC_4x4,
    PIXELFORMAT_ASTC_5x4,         DkImageFormat_RGBA_ASTC_5x4,
    PIXELFORMAT_ASTC_6x5,         DkImageFormat_RGBA_ASTC_6x5,
    PIXELFORMAT_ASTC_6x6,         DkImageFormat_RGBA_ASTC_6x6,
    PIXELFORMAT_ASTC_8x5,         DkImageFormat_RGBA_ASTC_8x5,
    PIXELFORMAT_ASTC_8x6,         DkImageFormat_RGBA_ASTC_8x6,
    PIXELFORMAT_ASTC_8x8,         DkImageFormat_RGBA_ASTC_8x8,
    PIXELFORMAT_ASTC_10x5,        DkImageFormat_RGBA_ASTC_10x5,
    PIXELFORMAT_ASTC_10x6,        DkImageFormat_RGBA_ASTC_10x6,
    PIXELFORMAT_ASTC_10x8,        DkImageFormat_RGBA_ASTC_10x8,
    PIXELFORMAT_ASTC_10x10,       DkImageFormat_RGBA_ASTC_10x10,
    PIXELFORMAT_ASTC_12x10,       DkImageFormat_RGBA_ASTC_12x10,
    PIXELFORMAT_ASTC_12x12,       DkImageFormat_RGBA_ASTC_12x12
);

constexpr auto blendEquations = BidirectionalMap<>::Create(
    RenderState::BLENDOP_ADD,              DkBlendOp_Add,
    RenderState::BLENDOP_SUBTRACT,         DkBlendOp_Sub,
    RenderState::BLENDOP_REVERSE_SUBTRACT, DkBlendOp_RevSub,
    RenderState::BLENDOP_MIN,              DkBlendOp_Min,
    RenderState::BLENDOP_MAX,              DkBlendOp_Max
);

constexpr auto blendFactors = BidirectionalMap<>::Create(
    RenderState::BLENDFACTOR_ZERO,                DkBlendFactor_Zero,
    RenderState::BLENDFACTOR_ONE,                 DkBlendFactor_One,
    RenderState::BLENDFACTOR_SRC_COLOR,           DkBlendFactor_SrcColor,
    RenderState::BLENDFACTOR_ONE_MINUS_SRC_COLOR, DkBlendFactor_InvSrcColor,
    RenderState::BLENDFACTOR_SRC_ALPHA,           DkBlendFactor_SrcAlpha,
    RenderState::BLENDFACTOR_ONE_MINUS_SRC_ALPHA, DkBlendFactor_InvSrcAlpha,
    RenderState::BLENDFACTOR_DST_COLOR,           DkBlendFactor_DstColor,
    RenderState::BLENDFACTOR_ONE_MINUS_DST_COLOR, DkBlendFactor_InvDstColor,
    RenderState::BLENDFACTOR_DST_ALPHA,           DkBlendFactor_DstAlpha,
    RenderState::BLENDFACTOR_ONE_MINUS_DST_ALPHA, DkBlendFactor_InvDstAlpha,
    RenderState::BLENDFACTOR_SRC_ALPHA_SATURATED, DkBlendFactor_SrcAlphaSaturate
);

constexpr auto filterModes = BidirectionalMap<>::Create(
    SamplerState::FILTER_LINEAR,  DkFilter_Linear,
    SamplerState::FILTER_NEAREST, DkFilter_Nearest
);

constexpr auto wrapModes = BidirectionalMap<>::Create(
    SamplerState::WRAP_CLAMP,           DkWrapMode_ClampToEdge,
    SamplerState::WRAP_CLAMP_ZERO,      DkWrapMode_ClampToBorder,
    SamplerState::WRAP_REPEAT,          DkWrapMode_Repeat,
    SamplerState::WRAP_MIRRORED_REPEAT, DkWrapMode_MirroredRepeat
);

constexpr auto cullModes = BidirectionalMap<>::Create(
    Vertex::CULL_NONE,  DkFace_None,
    Vertex::CULL_BACK,  DkFace_Back,
    Vertex::CULL_FRONT, DkFace_Front
);

constexpr auto windingModes = BidirectionalMap<>::Create(
    Vertex::WINDING_CW,  DkFrontFace_CW,
    Vertex::WINDING_CCW, DkFrontFace_CCW
);

constexpr auto compareModes = BidirectionalMap<>::Create(
    RenderState::COMPARE_LESS,     DkCompareOp_Less,
    RenderState::COMPARE_LEQUAL,   DkCompareOp_Lequal,
    RenderState::COMPARE_EQUAL,    DkCompareOp_Equal,
    RenderState::COMPARE_GEQUAL,   DkCompareOp_Gequal,
    RenderState::COMPARE_GREATER,  DkCompareOp_Greater,
    RenderState::COMPARE_NOTEQUAL, DkCompareOp_NotEqual,
    RenderState::COMPARE_ALWAYS,   DkCompareOp_Always,
    RenderState::COMPARE_NEVER,    DkCompareOp_Never
);

constexpr auto primitiveModes = BidirectionalMap<>::Create(
    Vertex::PRIMITIVE_TRIANGLES,      DkPrimitive_Triangles,
    Vertex::PRIMITIVE_TRIANGLE_FAN,   DkPrimitive_TriangleFan,
    Vertex::PRIMITIVE_TRIANGLE_STRIP, DkPrimitive_TriangleStrip,
    Vertex::PRIMITIVE_QUADS,          DkPrimitive_Quads,
    Vertex::PRIMITIVE_POINTS,         DkPrimitive_Points
);
// clang-format on

bool deko3d::GetConstant(PixelFormat in, DkImageFormat& out)
{
    return pixelFormats.Find(in, out);
}

bool deko3d::GetConstant(DkImageFormat in, PixelFormat& out)
{
    return pixelFormats.ReverseFind(in, out);
}

bool deko3d::GetConstant(RenderState::BlendOperation in, DkBlendOp& out)
{
    return blendEquations.Find(in, out);
}

bool deko3d::GetConstant(RenderState::BlendFactor in, DkBlendFactor& out)
{
    return blendFactors.Find(in, out);
}

bool deko3d::GetConstant(SamplerState::FilterMode in, DkFilter& out)
{
    return filterModes.Find(in, out);
}

bool deko3d::GetConstant(SamplerState::WrapMode in, DkWrapMode& out)
{
    return wrapModes.Find(in, out);
}

bool deko3d::GetConstant(Vertex::CullMode in, DkFace& out)
{
    return cullModes.Find(in, out);
}

bool deko3d::GetConstant(Vertex::Winding in, DkFrontFace& out)
{
    return windingModes.Find(in, out);
}

bool deko3d::GetConstant(RenderState::CompareMode in, DkCompareOp& out)
{
    return compareModes.Find(in, out);
}

bool deko3d::GetConstant(Vertex::PrimitiveType in, DkPrimitive& out)
{
    return primitiveModes.Find(in, out);
}

bool deko3d::GetConstant(DkPrimitive in, Vertex::PrimitiveType& out)
{
    return primitiveModes.ReverseFind(in, out);
}
