#include "modules/graphics/graphics.h"
#include "modules/window/window.h"

#include "common/bidirectionalmap.h"
#include "modules/math/mathmodule.h"

using namespace love;

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

void Graphics::GammaCorrectColor(Colorf& c)
{
    if (Graphics::IsGammaCorrect())
    {
        c.r = Math::GammaToLinear(c.r);
        c.g = Math::GammaToLinear(c.g);
        c.b = Math::GammaToLinear(c.b);
    }
}

Colorf Graphics::GammaCorrectColor(const Colorf& c)
{
    Colorf r = c;
    Graphics::GammaCorrectColor(r);

    return r;
}

void Graphics::UnGammaCorrectColor(Colorf& c)
{
    if (Graphics::IsGammaCorrect())
    {
        c.r = Math::LinearToGamma(c.r);
        c.g = Math::LinearToGamma(c.g);
        c.b = Math::LinearToGamma(c.b);
    }
}

Colorf Graphics::UnGammaCorrectColor(const Colorf& c)
{
    Colorf r = c;
    Graphics::UnGammaCorrectColor(r);

    return r;
}

/* End */

Graphics::Graphics() : width(0), height(0), active(true), created(false)
{
    this->states.reserve(10);
    this->states.push_back(DisplayState());

    this->transformStack.reserve(16);
    this->transformStack.push_back(Matrix4());

    this->pixelScaleStack.reserve(16);
    this->pixelScaleStack.push_back(1);

    auto window = Module::GetInstance<Window>(M_WINDOW);

    if (window != nullptr)
    {
        window->SetGraphics(this);

        if (window->IsOpen())
        {
            int width, height = 0;
            window->GetWindow(width, height);

            this->SetMode(width, height);
        }
    }
}

Graphics::~Graphics()
{
    this->states.clear();
    this->defaultFont.Set(nullptr);
}

bool Graphics::IsCreated() const
{
    return this->created;
}

bool Graphics::IsActive() const
{
    auto window = Module::GetInstance<Window>(M_WINDOW);

    return this->active && this->IsCreated() && window != nullptr && window->IsOpen();
}

bool Graphics::SetMode(int width, int height)
{
    this->width  = width;
    this->height = height;

    this->created = true;

    return true;
}

/* Screen Stuff */
void Graphics::SetActiveScreen(RenderScreen screen)
{
    Graphics::ACTIVE_SCREEN = screen;
}

const RenderScreen Graphics::GetActiveScreen() const
{
    return Graphics::ACTIVE_SCREEN;
}

const int Graphics::GetWidth(RenderScreen screen) const
{
    return Screen::Instance().GetWidth(screen);
}

const int Graphics::GetHeight() const
{
    return Screen::Instance().GetHeight();
}

std::vector<const char*> Graphics::GetScreens() const
{
    return Screen::Instance().GetConstants(Graphics::ACTIVE_SCREEN);
}

/* End Screen Stuff */

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

void Graphics::SetBackgroundColor(const Colorf& color)
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
Shader* Graphics::GetShader() const
{
    return states.back().shader.Get();
}

void Graphics::SetShader(Shader* shader)
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

bool Graphics::IsCanvasActive(Canvas* canvas) const
{
    const auto target = this->states.back().canvas;

    return target.Get() == canvas;
}

Video* Graphics::NewVideo(VideoStream* stream, float dpiscale)
{
    return new Video(this, stream, dpiscale);
}

bool Graphics::IsCanvasActive() const
{
    return this->states.back().canvas != nullptr;
}

bool Graphics::GetScissor(Rect& scissor) const
{
    const DisplayState& state = states.back();
    scissor                   = state.scissorRect;

    return state.scissor;
}

void Graphics::SetDefaultFilter(const Texture::Filter& filter)
{
    Texture::defaultFilter            = filter;
    this->states.back().defaultFilter = filter;
}

const Texture::Filter& Graphics::GetDefaultFilter() const
{
    return Texture::defaultFilter;
}

void Graphics::Origin()
{
    auto& transform = this->transformStack.back();
    transform.SetIdentity();

    this->pixelScaleStack.back() = 1;
}

void Graphics::Push(StackType type)
{
    if (this->transformStack.size() == MAX_USER_STACK_DEPTH)
        throw Exception("Maximum stack depth reached (more pushes than pops?)");

    this->PushTransform();
    this->pixelScaleStack.push_back(pixelScaleStack.back());

    if (type == STACK_ALL)
        states.push_back(states.back());

    this->stackTypeStack.push_back(type);
}

void Graphics::Translate(float offsetX, float offsetY)
{
    auto& transform = this->transformStack.back();
    transform.Translate(offsetX, offsetY);
}

void Graphics::Rotate(float rotation)
{
    auto& transform = this->transformStack.back();
    transform.Rotate(rotation);
}

void Graphics::Scale(float x, float y)
{
    auto& transform = this->transformStack.back();
    transform.Scale(x, y);

    pixelScaleStack.back() *= (fabs(x) + fabs(y)) / 2.0;
}

void Graphics::Shear(float kx, float ky)
{
    auto& transform = this->transformStack.back();
    transform.Shear(kx, ky);
}

void Graphics::Pop()
{
    if (this->transformStack.size() < 1)
        throw Exception("Minimum stack depth reached (more pops than pushes?)");

    this->PopTransform();
    this->pixelScaleStack.pop_back();

    if (this->stackTypeStack.back() == STACK_ALL)
    {
        DisplayState& newstate = states[states.size() - 2];

        this->RestoreStateChecked(newstate);

        // The last two states in the stack should be equal now.
        states.pop_back();
    }

    this->stackTypeStack.pop_back();
}

Vector2 Graphics::TransformPoint(Vector2 point)
{
    Vector2 p;
    this->transformStack.back().TransformXY(&p, &point, 1);

    return p;
}

Vector2 Graphics::InverseTransformPoint(Vector2 point)
{
    Vector2 p;
    // TODO: We should probably cache the inverse transform so we don't have to
    // re-calculate it every time this is called.
    transformStack.back().Inverse().TransformXY(&p, &point, 1);

    return p;
}

/* Objects */

Image* Graphics::NewImage(Texture::TextureType t, PixelFormat format, int width, int height,
                          int slices)
{
    return new Image(t, format, width, height, slices);
}

Image* Graphics::NewImage(const Image::Slices& data)
{
    return new Image(data);
}

Quad* Graphics::NewQuad(Quad::Viewport viewport, double sw, double sh)
{
    return new Quad(viewport, sw, sh);
}

Text* Graphics::NewText(Font* font, const std::vector<Font::ColoredString>& text)
{
    return new Text(font, text);
}

Canvas* Graphics::NewCanvas(const Canvas::Settings& settings)
{
    return new Canvas(settings);
}

/* ------ */

void Graphics::Draw(Drawable* drawable, const Matrix4& matrix)
{
    drawable->Draw(this, matrix);
}

void Graphics::Draw(Texture* texture, Quad* quad, const Matrix4& matrix)
{
    texture->Draw(this, quad, matrix);
}

void Graphics::Print(const std::vector<Font::ColoredString>& strings, const Matrix4& localTransform)
{
    this->CheckSetDefaultFont();

    if (this->states.back().font.Get() != nullptr)
        this->Print(strings, this->states.back().font.Get(), localTransform);
}

void Graphics::Print(const std::vector<Font::ColoredString>& strings, Font* font,
                     const Matrix4& localTransform)
{
    font->Print(this, strings, localTransform, this->states.back().foreground);
}

void Graphics::PrintF(const std::vector<Font::ColoredString>& strings, float wrap,
                      Font::AlignMode align, const Matrix4& localTransform)
{
    this->CheckSetDefaultFont();

    if (this->states.back().font.Get() != nullptr)
        this->PrintF(strings, this->states.back().font.Get(), wrap, align, localTransform);
}

void Graphics::PrintF(const std::vector<Font::ColoredString>& strings, Font* font, float wrap,
                      Font::AlignMode align, const Matrix4& localTransform)
{
    font->Printf(this, strings, wrap, align, localTransform, this->states.back().foreground);
}

/* End Objects */

void Graphics::PushTransform()
{
    this->transformStack.push_back(this->transformStack.back());
}

void Graphics::PopTransform()
{
    this->transformStack.pop_back();
}

void Graphics::Reset()
{
    DisplayState blankState;
    this->RestoreState(blankState);
    this->Origin();
}

/* Private */

void Graphics::RestoreState(const DisplayState& state)
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
    this->SetCanvas(state.canvas.Get());
    this->SetDefaultFilter(state.defaultFilter);
}

Graphics::BlendMode Graphics::GetBlendMode(Graphics::BlendAlpha& alphaMode)
{
    alphaMode = this->states.back().blendAlphaMode;
    return this->states.back().blendMode;
}

void Graphics::RestoreStateChecked(const DisplayState& state)
{
    const DisplayState& current = states.back();

    if (state.foreground != current.foreground)
        this->SetColor(state.foreground);

    this->SetBackgroundColor(state.background);

    if (state.blendMode != current.blendMode || state.blendAlphaMode != current.blendAlphaMode)
        this->SetBlendMode(state.blendMode, state.blendAlphaMode);

    // These are just simple assignments.
    this->SetLineWidth(state.lineWidth);
    this->SetLineStyle(state.lineStyle);
    this->SetLineJoin(state.lineJoin);

    if (state.pointSize != current.pointSize)
        this->SetPointSize(state.pointSize);

    if (state.scissor != current.scissor ||
        (state.scissor && !(state.scissorRect == current.scissorRect)))
    {
        if (state.scissor)
            this->SetScissor(state.scissorRect);
        else
            this->SetScissor();
    }

#if defined(__SWITCH__)
    this->SetMeshCullMode(state.meshCullMode);

    if (state.winding != current.winding)
        this->SetFrontFaceWinding(state.winding);

    this->SetShader(state.shader.Get());
#endif

    this->SetFont(state.font.Get());

    if (state.colorMask != current.colorMask)
        this->SetColorMask(state.colorMask);

    if (state.canvas.Get() != current.canvas.Get())
        this->SetCanvas(state.canvas.Get());

    this->SetDefaultFilter(state.defaultFilter);
    this->SetDefaultMipmapFilter(state.defaultMipmapFilter, state.defaultMipmapSharpness);
}

Canvas* Graphics::GetCanvas() const
{
    return states.back().canvas.Get();
}

Graphics::ColorMask Graphics::GetColorMask() const
{
    return this->states.back().colorMask;
}

void Graphics::SetDefaultMipmapFilter(Texture::FilterMode filter, float sharpness)
{
    Texture::defaultMipmapFilter    = filter;
    Texture::defaultMipmapSharpness = sharpness;

    this->states.back().defaultMipmapFilter    = filter;
    this->states.back().defaultMipmapSharpness = sharpness;
}

void Graphics::IntersectScissor(const Rect& rect)
{
    Rect currect = states.back().scissorRect;

    if (!states.back().scissor)
    {
        currect.x = 0;
        currect.y = 0;
        currect.w = std::numeric_limits<int>::max();
        currect.h = std::numeric_limits<int>::max();
    }

    int x1 = std::max(currect.x, rect.x);
    int y1 = std::max(currect.y, rect.y);

    int x2 = std::min(currect.x + currect.w, rect.x + rect.w);
    int y2 = std::min(currect.y + currect.h, rect.y + rect.h);

    Rect newrect = { x1, y1, std::max(0, x2 - x1), std::max(0, y2 - y1) };
    this->SetScissor(newrect);
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
        this->defaultFont.Set(this->NewDefaultFont(12, TrueTypeRasterizer::HINTING_NORMAL),
                              Acquire::NORETAIN);
#else
        this->defaultFont.Set(this->NewDefaultFont(24), Acquire::NORETAIN);
#endif
    }

    this->states.back().font.Set(this->defaultFont.Get());
}

void Graphics::SetFont(Font* font)
{
    DisplayState& state = this->states.back();
    state.font.Set(font);
}

Font* Graphics::GetFont()
{
    this->CheckSetDefaultFont();
    return this->states.back().font.Get();
}

void Graphics::ApplyTransform(Transform* transform)
{
    Matrix4& m = transformStack.back();
    m *= transform->GetMatrix();

    float sx, sy;
    m.GetApproximateScale(sx, sy);
    this->pixelScaleStack.back() = (sx + sy) / 2.0;
}

void Graphics::ReplaceTransform(Transform* transform)
{
    const Matrix4& m      = transform->GetMatrix();
    transformStack.back() = m;

    float sx, sy;
    m.GetApproximateScale(sx, sy);
    this->pixelScaleStack.back() = (sx + sy) / 2.0;
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

// clang-format off
constexpr auto blendModes = BidirectionalMap<>::Create(
    "alpha",     Graphics::BlendMode::BLEND_ALPHA,
    "add",       Graphics::BlendMode::BLEND_ADD,
    "subtract",  Graphics::BlendMode::BLEND_SUBTRACT,
    "multiply",  Graphics::BlendMode::BLEND_MULTIPLY,
    "lighten",   Graphics::BlendMode::BLEND_LIGHTEN,
    "darken",    Graphics::BlendMode::BLEND_DARKEN,
    "screen",    Graphics::BlendMode::BLEND_SCREEN,
    "replace",   Graphics::BlendMode::BLEND_REPLACE,
    "none",      Graphics::BlendMode::BLEND_NONE
);

constexpr auto drawModes = BidirectionalMap<>::Create(
    "line", Graphics::DrawMode::DRAW_LINE,
    "fill", Graphics::DrawMode::DRAW_FILL
);

constexpr auto arcModes = BidirectionalMap<>::Create(
    "open",   Graphics::ArcMode::ARC_OPEN,
    "closed", Graphics::ArcMode::ARC_CLOSED,
    "pie",    Graphics::ArcMode::ARC_PIE
);

constexpr auto blendAlphaModes = BidirectionalMap<>::Create(
    "alphamultiply", Graphics::BlendAlpha::BLENDALPHA_MULTIPLY,
    "premultiplied", Graphics::BlendAlpha::BLENDALPHA_PREMULTIPLIED
);

constexpr auto stackTypes = BidirectionalMap<>::Create(
    "all",       Graphics::StackType::STACK_ALL,
    "transform", Graphics::StackType::STACK_TRANSFORM
);

constexpr auto lineJoins = BidirectionalMap<>::Create(
    "none",  Graphics::LINE_JOIN_NONE,
    "miter", Graphics::LINE_JOIN_MITER,
    "bevel", Graphics::LINE_JOIN_BEVEL
);

constexpr auto lineStyles = BidirectionalMap<>::Create(
    "smooth", Graphics::LINE_SMOOTH,
    "rough",  Graphics::LINE_ROUGH
);
// clang-format on

/* Constants */

bool Graphics::GetConstant(const char* in, DrawMode& out)
{
    return drawModes.Find(in, out);
}

bool Graphics::GetConstant(DrawMode in, const char*& out)
{
    return drawModes.ReverseFind(in, out);
}

std::vector<const char*> Graphics::GetConstants(DrawMode)
{
    return drawModes.GetNames();
}

bool Graphics::GetConstant(const char* in, ArcMode& out)
{
    return arcModes.Find(in, out);
}

bool Graphics::GetConstant(ArcMode in, const char*& out)
{
    return arcModes.ReverseFind(in, out);
}

std::vector<const char*> Graphics::GetConstants(ArcMode)
{
    return arcModes.GetNames();
}

bool Graphics::GetConstant(const char* in, BlendMode& out)
{
    return blendModes.Find(in, out);
}

bool Graphics::GetConstant(BlendMode in, const char*& out)
{
    return blendModes.ReverseFind(in, out);
}

std::vector<const char*> Graphics::GetConstants(BlendMode)
{
    return blendModes.GetNames();
}

bool Graphics::GetConstant(const char* in, BlendAlpha& out)
{
    return blendAlphaModes.Find(in, out);
}

bool Graphics::GetConstant(BlendAlpha in, const char*& out)
{
    return blendAlphaModes.ReverseFind(in, out);
}

std::vector<const char*> Graphics::GetConstants(BlendAlpha)
{
    return blendAlphaModes.GetNames();
}

bool Graphics::GetConstant(const char* in, StackType& out)
{
    return stackTypes.Find(in, out);
}

bool Graphics::GetConstant(StackType in, const char*& out)
{
    return stackTypes.ReverseFind(in, out);
}

std::vector<const char*> Graphics::GetConstants(StackType)
{
    return stackTypes.GetNames();
}

bool Graphics::GetConstant(const char* in, LineJoin& out)
{
    return lineJoins.Find(in, out);
}

bool Graphics::GetConstant(LineJoin in, const char*& out)
{
    return lineJoins.ReverseFind(in, out);
}

std::vector<const char*> Graphics::GetConstants(LineJoin)
{
    return lineJoins.GetNames();
}

bool Graphics::GetConstant(const char* in, LineStyle& out)
{
    return lineStyles.Find(in, out);
}

bool Graphics::GetConstant(LineStyle in, const char*& out)
{
    return lineStyles.ReverseFind(in, out);
}

std::vector<const char*> Graphics::GetConstants(LineStyle)
{
    return lineStyles.GetNames();
}
