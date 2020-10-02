#include "common/runtime.h"
#include "modules/graphics/graphics.h"
#include "common/backend/primitives.h"
#include "modules/window/window.h"

using namespace love;

#define WINDOW_MODULE() (Module::GetInstance<Window>(Module::M_WINDOW))

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

Color Graphics::GetColor()
{
    return this->states.back().foreground;
}

void Graphics::SetColor(const Color & color)
{
    this->states.back().foreground = color;
}

Color Graphics::GetBackgroundColor()
{
    return this->states.back().background;
}

void Graphics::SetBackgroundColor(const Color & color)
{
    this->states.back().background = color;
}

float Graphics::GetLineWidth()
{
    return this->states.back().lineWidth;
}

void Graphics::SetLineWidth(float width)
{
    this->states.back().lineWidth = width;
}

Rect Graphics::GetScissor()
{
    return this->states.back().scissorRect;
}

void Graphics::SetScissor()
{
    #if defined(_3DS)
        if (this->states.back().scissor)
            C2D_Flush();
    #endif

    this->states.back().scissor = false;
    Rect rect = this->states.back().scissorRect;

    Primitives::Scissor(false, rect.x, rect.y, rect.w, rect.h);
}

void Graphics::SetScissor(const Rect & rect)
{
    #if defined(_3DS)
        C2D_Flush();
    #endif

    this->states.back().scissor = true;
    this->states.back().scissorRect = rect;

    Primitives::Scissor(true, rect.x, rect.y, rect.w, rect.h);
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

Graphics::RendererInfo Graphics::GetRendererInfo()
{
    return this->rendererInfo;
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
    font->Print(this, strings, 0, this->AdjustColor(this->states.back().foreground), Font::ALIGN_LEFT, localTransform);
}

void Graphics::PrintF(const std::vector<Font::ColoredString> & strings, float wrap, Font::AlignMode align, const Matrix4 & localTransform)
{
    this->CheckSetDefaultFont();

    if (this->states.back().font.Get() != nullptr)
        this->PrintF(strings, this->states.back().font.Get(), wrap, align, localTransform);
}

void Graphics::PrintF(const std::vector<Font::ColoredString> & strings, Font * font, float wrap, Font::AlignMode align, const Matrix4 & localTransform)
{
    font->Print(this, strings, wrap, this->AdjustColor(this->states.back().foreground), align, localTransform);
}

/* End Objects */

void Graphics::Polygon(const std::string & mode, std::vector<Graphics::Point> points)
{
    SetViewMatrix(this->transformStack.back());
    Primitives::Polygon(mode, points, this->GetLineWidth(), this->AdjustColor(this->states.back().foreground));
}

void Graphics::Arc(const std::string & mode, float x, float y, float radius, float startAngle, float endAngle)
{
    SetViewMatrix(this->transformStack.back());
    Primitives::Arc(mode, x, y, radius, startAngle, endAngle, this->AdjustColor(this->states.back().foreground));
}

void Graphics::Ellipse(const std::string & mode, float x, float y, float radiusX, float radiusY)
{
    SetViewMatrix(this->transformStack.back());
    Primitives::Ellipse(mode, x, y, radiusX, radiusY, this->AdjustColor(this->states.back().foreground));
}

void Graphics::Line(float startx, float starty, float endx, float endy)
{
    SetViewMatrix(this->transformStack.back());
    Primitives::Line(startx, starty, endx, endy, this->GetLineWidth(), this->AdjustColor(this->states.back().foreground));
}

void Graphics::Circle(float x, float y, float radius)
{
    SetViewMatrix(this->transformStack.back());
    Primitives::Circle("fill", x, y, radius, this->GetLineWidth(), this->AdjustColor(this->states.back().foreground));
}

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
    this->SetFont(state.font);

    if (state.scissor)
        this->SetScissor(state.scissorRect);
    else
        this->SetScissor();
}

/* Font */

void Graphics::CheckSetDefaultFont()
{
    // We don't create or set the default Font if an existing font is in use.
    if (this->states.back().font.Get() != nullptr)
        return;

    // Create a new default font if we don't have one yet.
    if (!this->defaultFont.Get())
        this->defaultFont.Set(this->NewFont(), Acquire::NORETAIN);

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

/* End Font */

void Graphics::AdjustColor(Color * in)
{
    float mul = 255.0f;

    #if defined(_3DS)
        mul = 1.0f;
    #endif

    in->r = std::clamp(in->r, 0.0f, 1.0f) * mul;
    in->g = std::clamp(in->g, 0.0f, 1.0f) * mul;
    in->b = std::clamp(in->b, 0.0f, 1.0f) * mul;
    in->a = std::clamp(in->a, 0.0f, 1.0f) * mul;
}

Color Graphics::AdjustColor(const Color & in)
{
    float mul = 255.0f;
    Color ret = {0, 0, 0, 0};

    #if defined(_3DS)
        mul = 1.0f;
    #endif

    ret.r = std::clamp(in.r, 0.0f, 1.0f) * mul;
    ret.g = std::clamp(in.g, 0.0f, 1.0f) * mul;
    ret.b = std::clamp(in.b, 0.0f, 1.0f) * mul;
    ret.a = std::clamp(in.a, 0.0f, 1.0f) * mul;

    return ret;
}
