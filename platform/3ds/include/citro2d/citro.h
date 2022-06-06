#pragma once

#include "common/lmath.h"

#include <citro2d.h>

#include "common/pixelformat.h"
#include "common/render/renderer.h"

#include <array>

using namespace love;

class citro2d : public Renderer
{
  private:
    citro2d();

    static constexpr size_t MAX_RENDERTARGETS = 3;

    static constexpr const char* RENDERER_NAME    = "citro3d";
    static constexpr const char* RENDERER_VERSION = "1.7.0";
    static constexpr const char* RENDERER_VENDOR  = "devkitPro";
    static constexpr const char* RENDERER_DEVICE  = "DMP PICA200";

    C2D_ImageTint texForeground;

  public:
    static citro2d& Instance()
    {
        static citro2d instance;
        return instance;
    };

    ~citro2d();

    virtual void DestroyFramebuffers() override;

    virtual void CreateFramebuffers() override;

    virtual void Clear(const Colorf& color) override;

    virtual void ClearDepthStencil(int stencil, uint8_t mask, double depth) override;

    virtual void SetBlendColor(const Colorf& color) override;

    virtual void SetBlendMode(const RenderState::BlendState& blend) override;

    virtual void BindFramebuffer(love::Canvas* canvas = nullptr) override;

    virtual void Present() override;

    virtual void SetViewport(const Rect& viewport) override;

    virtual Rect GetViewport() const override;

    virtual void SetScissor(bool enable, const Rect& scissor, bool canvasActive) override;

    virtual void SetStencil(RenderState::CompareMode mode, int value) override;

    virtual void SetMeshCullMode(Vertex::CullMode mode) override;

    /* winding is only ccw, but can be changed by inverting mesh cull */
    virtual void SetVertexWinding(Vertex::Winding winding) override;

    virtual void SetSamplerState(love::Texture* texture, SamplerState& state) override;

    virtual void SetColorMask(const RenderState::ColorMask& mask) override;

    virtual void SetLineWidth(float lineWidth) override;

    virtual void SetLineStyle(Graphics::LineStyle style) override;

    virtual void SetPointSize(float pointSize) override;

    /* internal functions */

    static bool GetConstant(PixelFormat in, GPU_TEXCOLOR& out);
    static bool GetConstant(GPU_TEXCOLOR in, PixelFormat& out);

    static bool GetConstant(RenderState::BlendOperation in, GPU_BLENDEQUATION& out);

    static bool GetConstant(RenderState::BlendFactor in, GPU_BLENDFACTOR& out);

    static bool GetConstant(SamplerState::FilterMode in, GPU_TEXTURE_FILTER_PARAM& out);

    static bool GetConstant(SamplerState::WrapMode in, GPU_TEXTURE_WRAP_PARAM& out);

    static bool GetConstant(Vertex::CullMode in, GPU_CULLMODE& out);

    static bool GetConstant(RenderState::CompareMode in, GPU_TESTFUNC& out);

    template<typename T>
    void ModeChange(const T& func)
    {
        this->DestroyFramebuffers();
        func();
        this->CreateFramebuffers();
    }

    void SetWideMode(bool enable)
    {
        this->ModeChange([enable]() { gfxSetWide(enable); });
    }

    void Set3D(bool enable)
    {
        this->ModeChange([enable]() { gfxSet3D(enable); });
    }

    const bool Get3D() const
    {
        return gfxIs3D();
    }

    const bool GetWide() const
    {
        return gfxIsWide();
    }

    void DeferCallToEndOfFrame(std::function<void()>&& func)
    {
        this->deferredFunctions.emplace_back(std::move(func));
    }

  private:
    void EnsureInFrame();

    virtual void InitRendererInfo() override;

    std::vector<std::function<void()>> deferredFunctions;
    std::array<C3D_RenderTarget*, MAX_RENDERTARGETS> targets;

    C3D_RenderTarget* current;
};
