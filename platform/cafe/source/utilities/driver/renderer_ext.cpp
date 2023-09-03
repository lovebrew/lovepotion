#include <utilities/driver/renderer_ext.hpp>

#include <modules/keyboard_ext.hpp>

#include <objects/texture_ext.hpp>

#include <nn/swkbd.h>

#include <coreinit/memfrmheap.h>
#include <proc_ui/procui.h>

#include <malloc.h>
#include <stdlib.h>

using namespace love;

#define Keyboard() (Module::GetInstance<Keyboard<Console::CAFE>>(Module::M_KEYBOARD))

Renderer<Console::CAFE>::Renderer() :
    renderState {},
    inForeground(false),
    commandBuffer(nullptr),
    current {},
    state(nullptr),
    framebuffers {}
{
    this->commandBuffer = memalign(GX2_COMMAND_BUFFER_ALIGNMENT, GX2_COMMAND_BUFFER_SIZE);

    if (!this->commandBuffer)
        throw love::Exception("Failed to allocate command buffer.");

    // clang-format off
    uint32_t attributes[9] =
    {
        GX2_INIT_CMD_BUF_BASE, (uintptr_t)this->commandBuffer,
        GX2_INIT_CMD_BUF_POOL_SIZE, GX2_COMMAND_BUFFER_SIZE,
        GX2_INIT_ARGC, 0, GX2_INIT_ARGV, 0,
        GX2_INIT_END
    };
    // clang-format on

    GX2Init(attributes);

    for (const auto screen : love::GetScreenEnums())
        this->framebuffers[screen].Create(screen);

    this->state = (GX2ContextState*)memalign(GX2_CONTEXT_STATE_ALIGNMENT, sizeof(GX2ContextState));

    if (!this->state)
        throw love::Exception("Failed to create GX2ContextState.");

    GX2SetupContextStateEx(this->state, false);
    GX2SetContextState(this->state);

    GX2SetDepthOnlyControl(false, false, GX2_COMPARE_FUNC_ALWAYS);
    GX2SetAlphaTest(true, GX2_COMPARE_FUNC_GREATER, 0);

    GX2SetColorControl(GX2_LOGIC_OP_COPY, 0xFF, false, true);
    GX2SetSwapInterval(1);

    ProcUIRegisterCallback(PROCUI_CALLBACK_ACQUIRE, ProcUIAcquired, nullptr, 100);
    ProcUIRegisterCallback(PROCUI_CALLBACK_RELEASE, ProcUIReleased, nullptr, 100);

    if (int result = this->OnForegroundAcquired(); result != 0)
        throw love::Exception("Failed to acquire application foreground (error %d).", result);

    if (Keyboard() != nullptr)
        Keyboard()->Initialize();

    /* set up some state information */
    this->renderState.winding     = GX2_FRONT_FACE_CCW;
    this->renderState.cullBack    = true;
    this->renderState.depthTest   = false;
    this->renderState.depthWrite  = false;
    this->renderState.compareMode = GX2_COMPARE_FUNC_ALWAYS;

    m_buffer.elemCount = MAX_OBJECTS;
    m_buffer.elemSize  = vertex::VERTEX_SIZE;
    m_buffer.flags     = BUFFER_CREATE_FLAGS;

    if (bool valid = GX2RCreateBuffer(&m_buffer); !valid)
        throw love::Exception("Failed to create GX2RBuffer");

    GX2RSetAttributeBuffer(&m_buffer, 0, VERTEX_SIZE, 0);
}

uint32_t Renderer<Console::CAFE>::ProcUIAcquired(void* arg)
{
    return Renderer<Console::CAFE>::Instance().OnForegroundAcquired();
}

uint32_t Renderer<Console::CAFE>::ProcUIReleased(void* arg)
{
    return Renderer<Console::CAFE>::Instance().OnForegroundReleased();
}

int Renderer<Console::CAFE>::OnForegroundAcquired()
{
    this->inForeground = true;

    auto foregroundHeap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG);
    auto memOneHeap     = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);

    /* allocate tv scan buffer */
    if (!this->framebuffers[Screen::TV].AllocateScanBuffer(foregroundHeap))
        return -1;

    /* allocate gamepad scan buffer */
    if (!this->framebuffers[Screen::GAMEPAD].AllocateScanBuffer(foregroundHeap))
        return -2;

    /* invalidate tv color buffer */
    if (!this->framebuffers[Screen::TV].InvalidateColorBuffer(memOneHeap))
        return -4;

    /* invalidate gamepad color buffer */
    if (!this->framebuffers[Screen::GAMEPAD].InvalidateColorBuffer(memOneHeap))
        return -5;

    /* invalidate tv depth buffer */
    if (!this->framebuffers[Screen::TV].InvalidateDepthBuffer(memOneHeap))
        return -6;

    /* invalidate gamepad depth buffer */
    if (!this->framebuffers[Screen::GAMEPAD].InvalidateDepthBuffer(memOneHeap))
        return -7;

    return 0;
}

int Renderer<Console::CAFE>::OnForegroundReleased()
{
    auto foregroundHeap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG);
    auto memOneHeap     = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);

    MEMFreeToFrmHeap(foregroundHeap, MEM_FRM_HEAP_FREE_ALL);
    MEMFreeToFrmHeap(memOneHeap, MEM_FRM_HEAP_FREE_ALL);

    this->inForeground = false;

    return 0;
}

Renderer<Console::CAFE>::~Renderer()
{
    if (this->inForeground)
        this->OnForegroundReleased();

    GX2RDestroyBufferEx(&m_buffer, GX2R_RESOURCE_BIND_NONE);

    GX2Shutdown();

    free(this->state);
    this->state = nullptr;

    free(this->commandBuffer);
    this->commandBuffer = nullptr;
}

void Renderer<Console::CAFE>::EnsureInFrame()
{
    if (!this->inFrame)
    {
        this->cpuTickReference = OSGetSystemTick();
        this->inFrame          = true;
    }

    this->current = &this->framebuffers[love::GetActiveScreen()];
    GX2SetContextState(this->state);
}

void Renderer<Console::CAFE>::Clear(const Color& color)
{
    GX2ClearColor(&this->current->GetBuffer(), color.r, color.g, color.b, color.a);
    GX2SetContextState(this->state);

    if (Shader<Console::CAFE>::current)
        this->UseProgram(Shader<Console::CAFE>::current->GetGroup());
}

void Renderer<Console::CAFE>::SetDepthWrites(bool enable)
{
    GX2SetDepthOnlyControl(this->renderState.depthTest, enable, this->renderState.compareMode);
    this->renderState.depthWrite = enable;
}

void Renderer<Console::CAFE>::ClearDepthStencil(int stencil, uint8_t mask, double depth)
{
    if (!this->renderState.depthWrite)
        this->SetDepthWrites(true);

    GX2ClearDepthStencilEx(&this->current->GetDepthBuffer(), depth, stencil, GX2_CLEAR_FLAGS_BOTH);
    GX2SetContextState(this->state);
}

void Renderer<Console::CAFE>::SetBlendColor(const Color& color)
{
    GX2SetBlendConstantColor(color.r, color.g, color.b, color.a);
    GX2SetContextState(this->state);
}

void Renderer<Console::CAFE>::SetBlendMode(const RenderState::BlendState& state)
{
    std::optional<GX2BlendCombineMode> opRGB;
    if (!(opRGB = Renderer::blendEquations.Find(state.operationRGB)))
        return;

    std::optional<GX2BlendCombineMode> opAlpha;
    if (!(opAlpha = Renderer::blendEquations.Find(state.operationA)))
        return;

    std::optional<GX2BlendMode> srcColor;
    if (!(srcColor = Renderer::blendFactors.Find(state.srcFactorRGB)))
        return;

    std::optional<GX2BlendMode> dstColor;
    if (!(dstColor = Renderer::blendFactors.Find(state.dstFactorRGB)))
        return;

    std::optional<GX2BlendMode> srcAlpha;
    if (!(srcAlpha = Renderer::blendFactors.Find(state.srcFactorA)))
        return;

    std::optional<GX2BlendMode> dstAlpha;
    if (!(dstAlpha = Renderer::blendFactors.Find(state.dstFactorA)))
        return;

    GX2SetBlendControl(GX2_RENDER_TARGET_0, *srcColor, *dstColor, *opRGB, true, *srcAlpha,
                       *dstAlpha, *opAlpha);
}

void Renderer<Console::CAFE>::SetMeshCullMode(vertex::CullMode mode)
{
    bool enabled = (mode != vertex::CULL_NONE);

    this->renderState.cullFront = (enabled && mode == vertex::CULL_FRONT);
    this->renderState.cullBack  = (enabled && mode == vertex::CULL_BACK);

    GX2SetCullOnlyControl(this->renderState.winding, this->renderState.cullFront,
                          this->renderState.cullBack);
}

void Renderer<Console::CAFE>::SetVertexWinding(vertex::Winding winding)
{
    std::optional<GX2FrontFace> face;
    if (!(face = Renderer::windingModes.Find(winding)))
        return;

    bool front = this->renderState.cullFront;
    bool back  = this->renderState.cullBack;

    GX2SetCullOnlyControl(*face, front, back);
    this->renderState.winding = *face;
}

void Renderer<Console::CAFE>::BindFramebuffer(Texture<Console::ALL>* texture)
{
    this->EnsureInFrame();
    FlushVertices();

    if (texture && texture->IsRenderTarget())
    {
        auto* _texture = (Texture<Console::CAFE>*)texture;
        GX2SetColorBuffer(_texture->GetFramebuffer(), GX2_RENDER_TARGET_0);

        this->SetViewport({ 0, 0, texture->GetPixelWidth(), texture->GetPixelHeight() });
        this->SetScissor({ 0, 0, texture->GetPixelWidth(), texture->GetPixelHeight() }, true);
    }
    else
    {
        GX2SetColorBuffer(&this->current->GetBuffer(), GX2_RENDER_TARGET_0);
        GX2SetDepthBuffer(&this->current->GetDepthBuffer());

        this->SetViewport(this->current->GetViewport());
        this->SetScissor(this->current->GetScissor(), false);
    }

    this->current->UseProjection();
}

void Renderer<Console::CAFE>::FlushVertices()
{
    auto* vertices = (Vertex*)GX2RLockBufferEx(&m_buffer, GX2R_RESOURCE_BIND_NONE);

    size_t writeOffset = m_vertexOffset;
    for (const auto& command : m_commands)
    {
        if (command.count + m_vertexOffset > MAX_OBJECTS)
            m_vertexOffset = 0;

        std::memcpy(vertices + writeOffset, command.Vertices().get(), command.size);
        writeOffset += command.count;
    }

    GX2RUnlockBufferEx(&m_buffer, GX2R_RESOURCE_BIND_NONE);

    size_t readOffset = m_vertexOffset;
    for (const auto& command : m_commands)
    {
        std::optional<GX2PrimitiveMode> primitive;
        if (!(primitive = primitiveModes.Find(command.type)))
            throw love::Exception("Invalid primitive mode");

        ++drawCallsBatched;
        GX2DrawEx(*primitive, command.count, readOffset, 1);

        readOffset += command.count;
    }

    m_vertexOffset   = readOffset;
    gpuTickReference = OSGetSystemTick();

    m_commands.clear();
}

bool Renderer<Console::CAFE>::TexturesChanged(std::vector<Texture<Console::CAFE>*> handles)
{
    if (handles.size() != this->currentTextures.size())
        return true;

    for (size_t index = 0; index < handles.size(); index++)
    {
        if (this->currentTextures[index] != handles[index])
            return true;
    }

    return false;
}

bool Renderer<Console::CAFE>::Render(DrawCommand<Console::CAFE>& command)
{
    Shader<Console::CAFE>::defaults[command.shader]->Attach();

    // todo: check for duplicate texture?
    if (command.handles.empty() ||
        (command.handles.size() > 0 && !this->TexturesChanged(command.handles)))
    {
        ++drawCalls;
        m_commands.push_back(command.Clone());
        return true;
    }
    else
    {
        FlushVertices();

        if (!command.handles.empty())
        {
            for (size_t index = 0; index < command.handles.size(); index++)
            {
                auto* texture = command.handles[index]->GetHandle();
                auto* sampler = &command.handles[index]->GetSampler();

                Shader<Console::CAFE>::current->BindTexture(index, texture, sampler);
            }
        }

        ++drawCalls;
        m_commands.push_back(command.Clone());
        return true;
    }

    return false;
}

void Renderer<Console::CAFE>::Present()
{
    FlushVertices();
    GX2DrawDone();

    if (Keyboard()->IsShowing())
    {
        nn::swkbd::DrawDRC();
        this->ClearDepthStencil(0, 0xFF, 1.0);

        GX2SetContextState(this->state);
    }

    /* copy our color buffers to their scan buffers */
    for (auto& framebuffer : this->framebuffers)
        framebuffer.second.CopyScanBuffer();

    /* swap scan buffers */
    GX2SwapScanBuffers();

    const auto nanoSecSystem = OSTicksToNanoseconds(OSGetSystemTick() - this->cpuTickReference);
    std::chrono::nanoseconds cpuNanoSec(nanoSecSystem);
    Renderer<Console::CAFE>::cpuTime = std::chrono::duration<float, std::milli>(cpuNanoSec).count();

    this->inFrame = false;

    const auto nanoSecTicks = OSTicksToNanoseconds(OSGetSystemTick() - this->gpuTickReference);
    std::chrono::nanoseconds gpuNanoSec(nanoSecTicks);
    Renderer<Console::CAFE>::gpuTime = std::chrono::duration<float, std::milli>(gpuNanoSec).count();

    /*
    ** flush again as GX2WaitForFlip
    ** will block the CPU
    */
    GX2Flush();
    GX2WaitForFlip();

    m_vertexOffset = 0;
}

void Renderer<Console::CAFE>::SetSamplerState(Texture<Console::CAFE>* texture, SamplerState& state)
{
    auto& sampler = texture->GetSampler();

    std::optional<GX2TexXYFilterMode> minFilter;
    if (!(minFilter = Renderer::filterModes.Find(state.minFilter)))
        return;

    std::optional<GX2TexXYFilterMode> magFilter;
    if (!(magFilter = Renderer::filterModes.Find(state.magFilter)))
        return;

    GX2InitSamplerXYFilter(&sampler, *magFilter, *minFilter, GX2_TEX_ANISO_RATIO_NONE);

    std::optional<GX2TexClampMode> wrapU;
    if (!(wrapU = Renderer::wrapModes.Find(state.wrapU)))
        return;

    std::optional<GX2TexClampMode> wrapV;
    if (!(wrapV = Renderer::wrapModes.Find(state.wrapV)))
        return;

    std::optional<GX2TexClampMode> wrapW;
    if (!(wrapW = Renderer::wrapModes.Find(state.wrapW)))
        return;

    GX2InitSamplerClamping(&sampler, *wrapU, *wrapV, *wrapW);
    GX2InitSamplerLOD(&sampler, state.minLod, state.maxLod, state.lodBias);
}

void Renderer<Console::CAFE>::UseProgram(const WHBGfxShaderGroup& group)
{
    GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);
    GX2SetFetchShader(&group.fetchShader);
    GX2SetVertexShader(group.vertexShader);
    GX2SetPixelShader(group.pixelShader);

    this->current->UseProjection();
}

void Renderer<Console::CAFE>::SetColorMask(const RenderState::ColorMask& mask)
{
    auto channelMask =
        GX2ChannelMask((mask.r * GX2_CHANNEL_MASK_R) + (mask.g * GX2_CHANNEL_MASK_G) +
                       (mask.b * GX2_CHANNEL_MASK_B) + (mask.a * GX2_CHANNEL_MASK_A));

    GX2ChannelMask NONE = (GX2ChannelMask)0;
    GX2SetTargetChannelMasks(channelMask, NONE, NONE, NONE, NONE, NONE, NONE, NONE);
}

void Renderer<Console::CAFE>::SetLineWidth(float width)
{
    GX2SetLineWidth(width);
}

void Renderer<Console::CAFE>::SetLineStyle(RenderState::LineStyle style)
{}

void Renderer<Console::CAFE>::SetPointSize(float size)
{
    GX2SetPointSize(size, size);
}

void Renderer<Console::CAFE>::SetViewport(const Rect& viewport)
{
    this->EnsureInFrame();

    float width  = viewport.w;
    float height = viewport.h;

    if (viewport == Rect::EMPTY)
    {
        width  = (float)this->current->GetWidth();
        height = (float)this->current->GetHeight();
    }

    this->current->SetViewport(viewport);

    auto ortho = glm::ortho(0.0f, width, height, 0.0f, Z_NEAR, Z_FAR);
    this->current->SetProjection(ortho);
}

void Renderer<Console::CAFE>::SetScissor(const Rect& scissor, bool canvasActive)
{
    this->EnsureInFrame();
    this->current->SetScissor(scissor);
}
