#include "common/runtime.h"
#include "modules/graphics/graphics.h"
#include "modules/window/window.h"

#include "modules/math/mathmodule.h"

using namespace love;

#define WINDOW_MODULE() (Module::GetInstance<Window>(Module::M_WINDOW))

/* Gamma Correction */

bool Graphics::gammaCorrectColor = false;

void Graphics::SetGammaCorrect(bool enable)
{
    Graphics::gammaCorrectColor = enable;
}

bool Graphics::IsGammaCorrect()
{
    return Graphics::gammaCorrectColor;
}

void Graphics::GammaCorrectColor(Colorf & c)
{
    if (Graphics::IsGammaCorrect())
    {
        c.r = Math::GammaToLinear(c.r);
        c.g = Math::GammaToLinear(c.g);
        c.b = Math::GammaToLinear(c.b);
    }
}

Colorf Graphics::GammaCorrectColor(const Colorf & c)
{
    Colorf r = c;
    Graphics::GammaCorrectColor(r);

    return r;
}

void Graphics::UnGammaCorrectColor(Colorf & c)
{
    if (Graphics::IsGammaCorrect())
    {
        c.r = Math::LinearToGamma(c.r);
        c.g = Math::LinearToGamma(c.g);
        c.b = Math::LinearToGamma(c.b);
    }
}

Colorf Graphics::UnGammaCorrectColor(const Colorf & c)
{
    Colorf r = c;
    Graphics::UnGammaCorrectColor(r);

    return r;
}

/* End */

Graphics::Graphics()
{
    this->states.reserve(10);
    this->states.push_back(DisplayState());

    this->transformStack.reserve(16);
    this->transformStack.push_back(Matrix4());

    auto window = Module::GetInstance<Window>(M_WINDOW);

    if (window != nullptr)
        window->SetGraphics(this);
}

Graphics::~Graphics()
{
    this->states.clear();
    this->defaultFont.Set(nullptr);
}

void Graphics::SetDepth(float depth)
{
    #if defined (__SWITCH__)
        throw love::Exception("setDepth not supported on this console");
    #endif

    this->stereoDepth = std::clamp(depth, -4.0f, 4.0f);
}

Colorf Graphics::GetColor() const
{
    return this->states.back().foreground;
}

void Graphics::SetColor(Colorf color)
{
    this->states.back().foreground = color;
}

Colorf Graphics::GetBackgroundColor() const
{
    return this->states.back().background;
}

void Graphics::SetBackgroundColor(const Colorf & color)
{
    this->states.back().background = color;
}

float Graphics::GetLineWidth() const
{
    return this->states.back().lineWidth;
}

void Graphics::SetLineWidth(float width)
{
    this->states.back().lineWidth = width;
}


#if defined(__SWITCH__)
    Shader * Graphics::GetShader() const
    {
        return states.back().shader.Get();
    }

    void Graphics::SetShader(Shader * shader)
    {
        if (shader == nullptr)
            return this->SetShader();

        shader->Attach();
        states.back().shader.Set(shader);
    }

    void Graphics::SetShader()
    {
        Shader::AttachDefault(Shader::STANDARD_DEFAULT);
        states.back().shader.Set(nullptr);
    }
#endif

bool Graphics::GetScissor(Rect & scissor) const
{
    const DisplayState & state = states.back();
    scissor = state.scissorRect;

    return state.scissor;
}

void Graphics::SetCanvas(Canvas * canvas)
{
    WINDOW_MODULE()->SetRenderer(canvas);
}

void Graphics::SetDefaultFilter(const Texture::Filter & filter)
{
    Texture::defaultFilter = filter;
    this->states.back().defaultFilter = filter;
}

const Texture::Filter & Graphics::GetDefaultFilter() const
{
    return Texture::defaultFilter;
}

void Graphics::GetDimensions(int * width, int * height)
{
    auto window_sizes = WINDOW_MODULE()->GetFullscreenModes();
    int currentDisplay = WINDOW_MODULE()->GetDisplay();

    std::pair<int, int> size = window_sizes[currentDisplay];

    if (width)
        *width = size.first;

    if (height)
        *height = size.second;
}

void Graphics::Origin()
{
    auto & transform = this->transformStack.back();
    transform.SetIdentity();

    #if defined(_3DS)
        C2D_ViewRestore(&transform.GetElements());
    #endif
}

void Graphics::Push()
{
    if (this->transformStack.size() == MAX_USER_STACK_DEPTH)
        throw Exception("Maximum stack depth reached (more pushes than pops?)");

    this->transformStack.push_back(transformStack.back());
}

void Graphics::Translate(float offsetX, float offsetY)
{
    auto & transform = this->transformStack.back();
    transform.Translate(offsetX, offsetY);
}

void Graphics::Rotate(float rotation)
{
    auto & transform = this->transformStack.back();
    transform.Rotate(rotation);
}

void Graphics::Scale(float scalarX, float scalarY)
{
    auto & transform = this->transformStack.back();
    transform.Scale(scalarX, scalarY);
}

void Graphics::Shear(float kx, float ky)
{
    auto & transform = this->transformStack.back();
    transform.Shear(kx, ky);
}

void Graphics::Pop()
{
    if (this->transformStack.size() < 1)
        throw Exception("Minimum stack depth reached (more pops than pushes?)");

    this->transformStack.pop_back();
}

/* Objects */

Image * Graphics::NewImage(Data * data)
{
    return new Image(data);
}

Font * Graphics::NewFont(float size)
{
    return new Font(size);
}

Font * Graphics::NewFont(Font::SystemFontType type, float size)
{
    return new Font(type, size);
}

Font * Graphics::NewFont(Data * data, float size)
{
    return new Font(data, size);
}

Quad * Graphics::NewQuad(Quad::Viewport viewport, double sw, double sh)
{
    return new Quad(viewport, sw, sh);
}

Canvas * Graphics::NewCanvas(const Canvas::Settings & settings)
{
    return new Canvas(settings);
}

/* ------ */

void Graphics::Draw(Drawable * drawable, const Matrix4 & matrix)
{
    drawable->Draw(this, matrix);
}

void Graphics::Draw(Texture * texture, Quad * quad, const Matrix4 & matrix)
{
    texture->Draw(this, quad, matrix);
}

void Graphics::Print(const std::vector<Font::ColoredString> & strings, const Matrix4 & localTransform)
{
    this->CheckSetDefaultFont();

    if (this->states.back().font.Get() != nullptr)
        this->Print(strings, this->states.back().font.Get(), localTransform);
}

void Graphics::Print(const  std::vector<Font::ColoredString> & strings, Font * font, const Matrix4 & localTransform)
{
    font->Print(this, strings, localTransform, this->states.back().foreground);
}

void Graphics::PrintF(const std::vector<Font::ColoredString> & strings, float wrap, Font::AlignMode align, const Matrix4 & localTransform)
{
    this->CheckSetDefaultFont();

    if (this->states.back().font.Get() != nullptr)
        this->PrintF(strings, this->states.back().font.Get(), wrap, align, localTransform);
}

void Graphics::PrintF(const std::vector<Font::ColoredString> & strings, Font * font, float wrap, Font::AlignMode align, const Matrix4 & localTransform)
{
    font->Printf(this, strings, wrap, align, localTransform, this->states.back().foreground);
}

/* End Objects */

void Graphics::Reset()
{
    DisplayState blankState;
    this->RestoreState(blankState);
    this->Origin();
}

void Graphics::SetViewMatrix(const Matrix4 & matrix)
{
    #if defined (_3DS)
        C2D_ViewRestore(&matrix.GetElements());
    #elif defined (__SWITCH__)

    #endif
}

/* Private */

void Graphics::RestoreState(const DisplayState & state)
{
    this->SetColor(state.foreground);
    this->SetBackgroundColor(state.background);

    this->SetBlendMode(state.blendMode, state.blendAlphaMode);

    this->SetLineWidth(state.lineWidth);
    this->SetLineStyle(state.lineStyle);
    this->SetLineJoin(state.lineJoin);

    this->SetPointSize(state.pointSize);

    if (state.scissor)
        this->SetScissor(state.scissorRect);
    else
        this->SetScissor();

    #if defined(__SWITCH__)
        this->SetMeshCullMode(state.meshCullMode);
        this->SetFrontFaceWinding(state.winding);

        this->SetShader(state.shader.Get());
    #endif

    this->SetFont(state.font.Get());

    this->SetColorMask(state.colorMask);
    this->SetDefaultFilter(state.defaultFilter);
}

/* Font */

void Graphics::CheckSetDefaultFont()
{
    // We don't create or set the default Font if an existing font is in use.
    if (this->states.back().font.Get() != nullptr)
        return;

    // Create a new default font if we don't have one yet.
    if (!this->defaultFont.Get())
    {
        #if defined(__SWITCH__)
            this->defaultFont.Set(this->NewDefaultFont(12, TrueTypeRasterizer::HINTING_NORMAL), Acquire::NORETAIN);
        #else
            this->defaultFont.Set(this->NewFont(), Acquire::NORETAIN);
        #endif
    }

    this->states.back().font.Set(this->defaultFont.Get());
}

void Graphics::SetFont(Font * font)
{
    DisplayState & state = this->states.back();
    state.font.Set(font);
}

Font * Graphics::GetFont()
{
    this->CheckSetDefaultFont();
    return this->states.back().font.Get();
}

void Graphics::SetLineJoin(LineJoin join)
{
    this->states.back().lineJoin = join;
}

void Graphics::SetLineStyle(LineStyle style)
{
    this->states.back().lineStyle = style;
}

float Graphics::GetPointSize() const
{
    return this->states.back().pointSize;
}

Graphics::LineStyle Graphics::GetLineStyle() const
{
    return this->states.back().lineStyle;
}

Graphics::LineJoin Graphics::GetLineJoin() const
{
    return this->states.back().lineJoin;
}

/* Constants */

bool Graphics::GetConstant(const char * in, DrawMode & out)
{
    return drawModes.Find(in, out);
}

bool Graphics::GetConstant(DrawMode in, const char *& out)
{
    return drawModes.Find(in, out);
}

std::vector<std::string> Graphics::GetConstants(DrawMode)
{
    return drawModes.GetNames();
}

bool Graphics::GetConstant(const char * in, ArcMode & out)
{
    return arcModes.Find(in, out);
}

bool Graphics::GetConstant(ArcMode in, const char *&out)
{
    return arcModes.Find(in, out);
}

std::vector<std::string> Graphics::GetConstants(ArcMode)
{
    return arcModes.GetNames();
}

bool Graphics::GetConstant(const char *in, BlendMode & out)
{
    return blendModes.Find(in, out);
}

bool Graphics::GetConstant(BlendMode in, const char *& out)
{
    return blendModes.Find(in, out);
}

std::vector<std::string> Graphics::GetConstants(BlendMode)
{
    return blendModes.GetNames();
}

bool Graphics::GetConstant(const char * in, BlendAlpha & out)
{
    return blendAlphaModes.Find(in, out);
}

bool Graphics::GetConstant(BlendAlpha in, const char *& out)
{
    return blendAlphaModes.Find(in, out);
}

std::vector<std::string> Graphics::GetConstants(BlendAlpha)
{
    return blendAlphaModes.GetNames();
}


StringMap<Graphics::BlendMode, Graphics::BLEND_MAX_ENUM>::Entry Graphics::blendModeEntries[] =
{
    { "alpha",    BLEND_ALPHA    },
    { "add",      BLEND_ADD      },
    { "subtract", BLEND_SUBTRACT },
    { "multiply", BLEND_MULTIPLY },
    { "lighten",  BLEND_LIGHTEN  },
    { "darken",   BLEND_DARKEN   },
    { "screen",   BLEND_SCREEN   },
    { "replace",  BLEND_REPLACE  },
    { "none",     BLEND_NONE     },
};

StringMap<Graphics::BlendMode, Graphics::BLEND_MAX_ENUM> Graphics::blendModes(Graphics::blendModeEntries, sizeof(Graphics::blendModeEntries));

StringMap<Graphics::DrawMode, Graphics::DRAW_MAX_ENUM>::Entry Graphics::drawModeEntries[] =
{
    { "line", DRAW_LINE },
    { "fill", DRAW_FILL },
};

StringMap<Graphics::DrawMode, Graphics::DRAW_MAX_ENUM> Graphics::drawModes(Graphics::drawModeEntries, sizeof(Graphics::drawModeEntries));

StringMap<Graphics::ArcMode, Graphics::ARC_MAX_ENUM>::Entry Graphics::arcModeEntries[] =
{
    { "open",   ARC_OPEN   },
    { "closed", ARC_CLOSED },
    { "pie",    ARC_PIE    },
};

StringMap<Graphics::ArcMode, Graphics::ARC_MAX_ENUM> Graphics::arcModes(Graphics::arcModeEntries, sizeof(Graphics::arcModeEntries));

StringMap<Graphics::BlendAlpha, Graphics::BLENDALPHA_MAX_ENUM>::Entry Graphics::blendAlphaEntries[] =
{
    { "alphamultiply", BLENDALPHA_MULTIPLY      },
    { "premultiplied", BLENDALPHA_PREMULTIPLIED },
};

StringMap<Graphics::BlendAlpha, Graphics::BLENDALPHA_MAX_ENUM> Graphics::blendAlphaModes(Graphics::blendAlphaEntries, sizeof(Graphics::blendAlphaEntries));