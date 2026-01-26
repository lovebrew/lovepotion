#include "common/screen.hpp"

#include "driver/display/citro3d.hpp"
#include "modules/graphics/Shader.hpp"

#include "common/debug.hpp"

namespace love
{
    citro3d::citro3d() : context {}
    {
        this->targets.reserve(3);
    }

    void citro3d::initialize()
    {
        if (this->initialized)
            return;

        gfxInitDefault();

        if (!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE))
            throw love::Exception("Failed to initialize citro3d.");

        C3D_CullFace(GPU_CULL_NONE);

        this->context.depthWrites = false;
        this->context.mask        = GPU_WRITE_COLOR;

        C3D_AttrInfo* attributes = C3D_GetAttrInfo();
        AttrInfo_Init(attributes);

        AttrInfo_AddLoader(attributes, 0, GPU_FLOAT, 2); //< position
        AttrInfo_AddLoader(attributes, 1, GPU_FLOAT, 2); //< texcoord
        AttrInfo_AddLoader(attributes, 2, GPU_FLOAT, 4); //< color

        Mtx_Identity(&this->context.modelView);
        Mtx_Identity(&this->context.projection);

        this->set3DMode(true);
        this->initialized = true;
    }

    void citro3d::setTexEnvMode(TextureBase* texture, bool isFont)
    {
        TexEnvMode mode = (texture == nullptr) ? TEXENV_MODE_PRIMITIVE : TEXENV_MODE_TEXTURE;
        if (texture && isFont)
            mode = TEXENV_MODE_FONT;

        if (mode == this->context.texEnvMode)
            return;

        C3D_TexEnv* env = C3D_GetTexEnv(0);
        C3D_TexEnvInit(env);

        switch (mode)
        {
            default:
            case TEXENV_MODE_PRIMITIVE:
                C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
                break;
            case TEXENV_MODE_TEXTURE:
                C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
                break;
            case TEXENV_MODE_FONT:
            {
                C3D_TexEnvSrc(env, C3D_RGB, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);

                C3D_TexEnvSrc(env, C3D_Alpha, GPU_PRIMARY_COLOR, GPU_TEXTURE0, GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
                break;
            }
        }

        this->context.texEnvMode = mode;
    }

    void citro3d::setupContext()
    {
        for (int i = 0; i < TEXTURE_MAX_ENUM + 1; i++)
        {
            this->context.boundTextures[i].clear();
            this->context.boundTextures[i].resize(3, nullptr);
        }
    }

    void citro3d::deInitialize()
    {
        if (!this->initialized)
            return;

        this->destroyFramebuffers();

        C3D_Fini();
        gfxExit();

        this->initialized = false;
    }

    void citro3d::createFramebuffers()
    {
        const auto info              = getScreenInfo();
        const size_t numFramebuffers = info.size();

        for (size_t index = 0; index < numFramebuffers; ++index)
            this->targets[index].create(info[index]);
    }

    void citro3d::destroyFramebuffers()
    {
        for (auto& target : this->targets)
            target.destroy();
    }

    void citro3d::ensureInFrame()
    {
        if (!this->inFrame)
        {
            C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
            this->inFrame = true;
        }
    }

    bool citro3d::isDefaultFramebufferActive() const
    {
        for (size_t index = 0; index < this->targets.size(); index++)
        {
            if (this->targets[index].get() == this->context.boundFramebuffer)
                return true;
        }

        return false;
    }

    C3D_RenderTarget* citro3d::getInternalBackbuffer() const
    {
        return this->targets[love::currentScreen].get();
    }

    void citro3d::clear(const Color& color)
    {
        if (!this->inFrame || !this->context.boundFramebuffer)
            return;

        C3D_RenderTargetClear(this->getFramebuffer(), C3D_CLEAR_COLOR, color.abgr(), 0);
    }

    void citro3d::clearDepth(double value)
    {
        if (!this->inFrame || !this->context.boundFramebuffer)
            return;

        value *= LOVE_UINT32_MAX;
        C3D_RenderTargetClear(this->getFramebuffer(), C3D_CLEAR_DEPTH, 0, value);
    }

    void citro3d::clearStencil(int value)
    {}

    void citro3d::setStencilState(const StencilState& state)
    {
        const bool enabled = state.action != STENCIL_KEEP || state.compare != COMPARE_ALWAYS;

        GPU_STENCILOP stencilAction = GPU_STENCIL_KEEP;
        getConstant(state.action, stencilAction);

        GPU_TESTFUNC testFunction = GPU_ALWAYS;
        getConstant(state.compare, testFunction);

        C3D_StencilTest(enabled, testFunction, state.value, state.readMask, state.writeMask);
        C3D_StencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, stencilAction);
    }

    void citro3d::setDepthWrites(CompareMode compare, bool write)
    {
        this->context.depthWrites = compare != COMPARE_ALWAYS || write;
        getConstant(compare, this->context.testMode);

        uint8_t mask = this->context.colorMask.get();
        if (this->context.depthWrites)
            mask |= GPU_WRITE_DEPTH;

        // clang-format off
        LOG("[setDepthWrites] Mask: %02X (DepthWrite: %d, TestMode %d)", mask, this->context.depthWrites, this->context.testMode);
        // clang-format on
        C3D_DepthTest(this->context.depthWrites, this->context.testMode, (GPU_WRITEMASK)mask);
    }

    C3D_RenderTarget* citro3d::getFramebuffer()
    {
        return this->context.boundFramebuffer;
    }

    void citro3d::bindFramebuffer(C3D_RenderTarget* framebuffer)
    {
        if (!this->inFrame)
            this->ensureInFrame();

        bool bindingModified = false;

        if (this->context.boundFramebuffer != framebuffer)
        {
            bindingModified                = true;
            this->context.boundFramebuffer = framebuffer;
        }

        if (bindingModified)
            C3D_FrameDrawOn(framebuffer);
    }

    void citro3d::present()
    {
        if (this->inFrame)
        {
            Graphics::flushBatchedDrawsGlobal();

            C3D_FrameEnd(0);

            Graphics::advanceStreamBuffersGlobal();
            this->inFrame = false;
        }

        for (size_t index = this->deferred.size(); index > 0; index--)
        {
            this->deferred[index - 1]();
            this->deferred.erase(this->deferred.begin() + index - 1);
        }
    }

    void citro3d::setViewport(const Rect& v, bool tilt)
    {
        this->context.viewport = v;
        C3D_SetViewport((uint32_t)v.x, (uint32_t)v.y, (uint32_t)v.w, (uint32_t)v.h);
    }

    void citro3d::setScissor(const Rect& scissor)
    {
        if (!this->context.boundFramebuffer)
            return;

        const int width  = this->context.boundFramebuffer->frameBuf.height;
        const int height = this->context.boundFramebuffer->frameBuf.width;

        Framebuffer::calculateBounds(scissor, this->context.scissor, width, height);

        // clang-format off
        GPU_SCISSORMODE mode = (scissor != Rect::EMPTY) ? GPU_SCISSOR_NORMAL : GPU_SCISSOR_DISABLE;
        C3D_SetScissor(mode, this->context.scissor.x, this->context.scissor.y, this->context.scissor.w, this->context.scissor.h);
        // clang-format on
    }

    void citro3d::setCullMode(CullMode mode)
    {
        GPU_CULLMODE cullMode;
        if (!citro3d::getConstant(mode, cullMode))
            return;

        if (this->context.cullMode == mode)
            return;

        C3D_CullFace(cullMode);
        this->context.cullMode = mode;
    }

    void citro3d::setVertexWinding(Winding)
    {}

    void citro3d::setColorMask(ColorChannelMask mask)
    {
        this->context.colorMask = mask;

        uint8_t write = mask.get();
        if (this->context.depthWrites)
            write |= GPU_WRITE_DEPTH;

        // clang-format off
        LOG("[setColorMask  ] Mask: %02X (DepthWrite: %d, TestMode %d)", write, this->context.depthWrites, this->context.testMode);
        // clang-format on
        C3D_DepthTest(this->context.depthWrites, this->context.testMode, (GPU_WRITEMASK)write);
    }

    void citro3d::setBlendState(const BlendState& state)
    {
        if (this->context.blendState == state)
            return;

        GPU_BLENDEQUATION operationRGB;
        if (!citro3d::getConstant(state.operationRGB, operationRGB))
            return;

        GPU_BLENDEQUATION operationA;
        if (!citro3d::getConstant(state.operationA, operationA))
            return;

        GPU_BLENDFACTOR sourceColor;
        if (!citro3d::getConstant(state.srcFactorRGB, sourceColor))
            return;

        GPU_BLENDFACTOR destColor;
        if (!citro3d::getConstant(state.dstFactorRGB, destColor))
            return;

        GPU_BLENDFACTOR sourceAlpha;
        if (!citro3d::getConstant(state.srcFactorA, sourceAlpha))
            return;

        GPU_BLENDFACTOR destAlpha;
        if (!citro3d::getConstant(state.dstFactorA, destAlpha))
            return;

        this->context.blendState = state;
        C3D_AlphaBlend(operationRGB, operationA, sourceColor, destColor, sourceAlpha, destAlpha);
    }

    void citro3d::setSamplerState(C3D_Tex* texture, SamplerState state)
    {
        auto magFilter = (state.minFilter == SamplerState::FILTER_NEAREST) ? GPU_NEAREST : GPU_LINEAR;
        auto minFilter = (state.magFilter == SamplerState::FILTER_NEAREST) ? GPU_NEAREST : GPU_LINEAR;

        C3D_TexSetFilter(texture, magFilter, minFilter);

        auto wrapU = citro3d::getWrapMode(state.wrapU);
        auto wrapV = citro3d::getWrapMode(state.wrapV);

        C3D_TexSetWrap(texture, wrapU, wrapV);

        float maxBias = 0xFFF;

        if (maxBias > 0.01f)
            maxBias = 0.01f;

        state.lodBias = std::clamp(state.lodBias, -maxBias, maxBias);
        C3D_TexSetLodBias(texture, state.lodBias);
    }

    void citro3d::prepareDraw(GraphicsBase* graphics)
    {
        // clang-format off
        if (Shader::current != nullptr)
        {
            Rect viewport = this->context.viewport;
            ((Shader*)Shader::current)->updateBuiltinUniforms(graphics, viewport.w, viewport.h);
        }
        // clang-format on
    }

    void citro3d::bindTextureToUnit(TextureType target, C3D_Tex* texture, int unit)
    {
        if (this->context.boundTexture != texture)
        {
            this->context.boundTexture = texture;
            C3D_TexBind(0, texture);
        }
    }

    void citro3d::bindTextureToUnit(TextureBase* texture, int unit)
    {
        if (!texture)
            return;

        auto textureType = texture->getTextureType();
        auto* handle     = (C3D_Tex*)texture->getHandle();

        this->bindTextureToUnit(textureType, handle, unit);
    }

    GPU_FORMATS citro3d::getVertexComponents(DataFormat type, int& components)
    {
        switch (type)
        {
            case DATAFORMAT_FLOAT:
                components = 1;
                return GPU_FLOAT;
            case DATAFORMAT_FLOAT_VEC2:
                components = 2;
                return GPU_FLOAT;
            case DATAFORMAT_FLOAT_VEC3:
                components = 3;
                return GPU_FLOAT;
            case DATAFORMAT_FLOAT_VEC4:
                components = 4;
                return GPU_FLOAT;
            default:
                throw love::Exception("Unsupported vertex attribute format: {:d}.", (int)type);
        }
    }

    void citro3d::setVertexAttributes(const VertexAttributes& attributes, const BufferBindings& buffers)
    {
        uint32_t i       = 0;
        uint32_t allBits = attributes.enableBits | 3;

        C3D_AttrInfo info {};
        AttrInfo_Init(&info);

        while (allBits)
        {
            uint32_t bit = 1u << i;
            if (attributes.enableBits & bit)
            {
                const auto& attribute = attributes.attributes[i];

                int components    = 0;
                const auto format = this->getVertexComponents(attribute.getFormat(), components);

                AttrInfo_AddLoader(&info, i, format, components);
            }
            i++;
            allBits >>= 1u;
        }

        C3D_SetAttrInfo(&info);
        C3D_SetBufInfo((C3D_BufInfo*)buffers.info[0].buffer->getHandle());
    }

    GPU_TEXTURE_MODE_PARAM citro3d::getTextureType(TextureType type)
    {
        switch (type)
        {
            case TEXTURE_2D:
            default:
                return GPU_TEX_2D;
            case TEXTURE_CUBE:
                return GPU_TEX_CUBE_MAP;
        }
    }

    GPU_TEXTURE_WRAP_PARAM citro3d::getWrapMode(SamplerState::WrapMode mode)
    {
        switch (mode)
        {
            case SamplerState::WRAP_CLAMP:
                return GPU_CLAMP_TO_EDGE;
            case SamplerState::WRAP_REPEAT:
                return GPU_REPEAT;
            case SamplerState::WRAP_MIRRORED_REPEAT:
                return GPU_MIRRORED_REPEAT;
            default:
                return GPU_CLAMP_TO_EDGE;
        }
    }

    GPU_Primitive_t citro3d::getPrimitiveType(PrimitiveType type)
    {
        switch (type)
        {
            case PRIMITIVE_TRIANGLES:
                return GPU_TRIANGLES;
            case PRIMITIVE_TRIANGLE_FAN:
                return GPU_TRIANGLE_FAN;
            case PRIMITIVE_TRIANGLE_STRIP:
                return GPU_TRIANGLE_STRIP;
            default:
                break;
        }

        throw love::Exception("Invalid primitive type: {:d}.", (int)type);
    }

    int citro3d::getTextureUnit(GPU_TEXUNIT unit)
    {
        switch (unit)
        {
            case GPU_TEXUNIT0:
            default:
                return 0;
            case GPU_TEXUNIT1:
                return 1;
            case GPU_TEXUNIT2:
                return 2;
        }

        throw love::Exception("Invalid texture unit: {:d}.", (int)unit);
    }

    citro3d c3d;
} // namespace love
