#include "common/runtime.h"
#include "modules/graphics/graphics.h"
#include "common/backend/primitives.h"
#include "modules/window/window.h"

using namespace love;

Graphics::Graphics()
{
    this->states.reserve(10);
    this->states.push_back(DisplayState());

    auto window = Module::GetInstance<Window>(M_WINDOW);

    if (window != nullptr)
        window->SetGraphics(this);
}

Graphics::~Graphics()
{
    this->states.clear();
    this->defaultFont.Set(nullptr);
}

Color Graphics::GetColor()
{
    return this->states.back().foreground;
}

void Graphics::SetColor(const Color & color)
{
    this->AdjustColor(color, &this->states.back().foreground);
}

Color Graphics::GetBackgroundColor()
{
    return this->states.back().background;
}

void Graphics::SetBackgroundColor(const Color & color)
{
    this->AdjustColor(color, &this->states.back().background);
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

void Graphics::SetScissor(int x, int y, int width, int height)
{
    this->states.back().scissorRect = {x, y, width, height};
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

/* Objects */

Image * Graphics::NewImage(const std::string & path)
{
    return new Image(path);
}

Font * Graphics::NewFont(float size)
{
    return new Font(size);
}

Font * Graphics::NewFont(const std::string & path, float size)
{
    return new Font(path, size);
}

Quad * Graphics::NewQuad(Quad::Viewport viewport, double sw, double sh)
{
    return new Quad(viewport, sw, sh);
}

void Graphics::Draw(Drawable * drawable, const DrawArgs & args)
{
    drawable->Draw(args, this->states.back().foreground);
}

void Graphics::Draw(Texture * texture, Quad * quad, const DrawArgs & args)
{
    texture->Draw(args, quad, this->states.back().foreground);
}

void Graphics::Print(const std::vector<Font::ColoredString> & strings, const DrawArgs & args)
{
    this->CheckSetDefaultFont();

    if (this->states.back().font.Get() != nullptr)
        this->Print(strings, this->states.back().font.Get(), args);
}

void Graphics::Print(const std::vector<Font::ColoredString> & strings, Font * font, const DrawArgs & args)
{
    font->Print(strings, args, nullptr, this->states.back().foreground);
}

/* End Objects */

void Graphics::Rectangle(const std::string & mode, float x, float y, float width, float height)
{
    Primitives::Rectangle(mode, x, y, width, height, 0, 0, this->GetLineWidth(), this->GetColor());
}

void Graphics::Reset()
{
    DisplayState blankState;
    this->RestoreState(blankState);
    // Origin
}

/* Private */

void Graphics::RestoreState(const DisplayState & state)
{
    this->SetColor(state.foreground);
    this->SetBackgroundColor(state.background);
    this->SetFont(state.font);
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

void Graphics::AdjustColor(const Color & in, Color * out)
{
    float mul = 255.0f;

    #if defined(_3DS)
        mul = 1.0f;
    #endif

    out->r = std::clamp(in.r, 0.0f, 1.0f) * mul;
    out->g = std::clamp(in.g, 0.0f, 1.0f) * mul;
    out->b = std::clamp(in.b, 0.0f, 1.0f) * mul;
    out->a = std::clamp(in.a, 0.0f, 1.0f) * mul;
}
