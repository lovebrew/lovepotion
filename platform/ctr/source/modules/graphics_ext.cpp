#include <modules/graphics_ext.hpp>

#include <objects/rasterizer_ext.hpp>

#include <modules/fontmodule_ext.hpp>
#include <modules/window_ext.hpp>

#include <objects/font_ext.hpp>
#include <objects/textbatch_ext.hpp>
#include <objects/texture_ext.hpp>

using Renderer = love::Renderer<love::Console::CTR>;
using namespace love;

Graphics<Console::CTR>::Graphics()
{
    auto* window = Module::GetInstance<Window<Console::Which>>(Module::M_WINDOW);

    if (window != nullptr)
    {
        window->SetGraphics(this);

        if (window->IsOpen())
            window->SetWindow();
    }

    this->CheckSetDefaultFont();
}

/* objects */

Font<Console::CTR>* Graphics<Console::CTR>::NewDefaultFont(int size) const
{
    auto fontModule = Module::GetInstance<FontModule<Console::CTR>>(M_FONT);

    if (!fontModule)
        throw love::Exception("Font module has not been loaded.");

    StrongReference<Rasterizer<Console::CTR>> rasterizer(fontModule->NewBCFNTRasterizer(size),
                                                         Acquire::NORETAIN);

    return this->NewFont(rasterizer.Get());
}

void Graphics<Console::CTR>::CheckSetDefaultFont()
{
    if (this->states.back().font.Get() != nullptr)
        return;

    if (!this->defaultFont.Get())
        this->defaultFont.Set(this->NewDefaultFont(12), Acquire::NORETAIN);

    this->states.back().font.Set(this->defaultFont.Get());
}

Font<Console::CTR>* Graphics<Console::CTR>::NewFont(Rasterizer<Console::CTR>* data) const
{
    return new Font<Console::CTR>(data, this->states.back().defaultSamplerState);
}

TextBatch<Console::CTR>* Graphics<Console::CTR>::NewTextBatch(Font<Console::CTR>* font,
                                                              const Font<>::ColoredStrings& text)
{
    return new TextBatch<Console::CTR>(font, text);
}

void Graphics<Console::CTR>::Print(const Font<>::ColoredStrings& strings,
                                   const Matrix4<Console::CTR>& matrix)
{
    this->CheckSetDefaultFont();

    if (this->states.back().font.Get() != nullptr)
        this->Print(strings, this->states.back().font.Get(), matrix);
}

void Graphics<Console::CTR>::Print(const Font<>::ColoredStrings& strings, Font<Console::CTR>* font,
                                   const Matrix4<Console::CTR>& matrix)
{
    font->Print(*this, strings, matrix, this->states.back().foreground);
}

void Graphics<Console::CTR>::Printf(const Font<>::ColoredStrings& strings, float wrap,
                                    Font<>::AlignMode align, const Matrix4<Console::CTR>& matrix)
{
    this->CheckSetDefaultFont();

    if (this->states.back().font.Get() != nullptr)
        this->Printf(strings, this->states.back().font.Get(), wrap, align, matrix);
}

void Graphics<Console::CTR>::Printf(const Font<>::ColoredStrings& strings, Font<Console::CTR>* font,
                                    float wrap, Font<>::AlignMode align,
                                    const Matrix4<Console::CTR>& matrix)
{
    font->Printf(*this, strings, wrap, align, matrix, this->states.back().foreground);
}

void Graphics<Console::CTR>::Draw(Drawable* drawable, const Matrix4<Console::CTR>& matrix)
{
    drawable->Draw(*this, matrix);
}

void Graphics<Console::CTR>::Draw(Texture<Console::CTR>* texture, Quad* quad,
                                  const Matrix4<Console::CTR>& matrix)
{
    texture->Draw(*this, quad, matrix);
}

Texture<Console::CTR>* Graphics<Console::CTR>::NewTexture(const Texture<>::Settings& settings,
                                                          const Texture<>::Slices* slices) const
{
    return new Texture<Console::CTR>(this, settings, slices);
}

/* primitives - we need these for lined stuff */

inline const auto normalizeAngle = [](float angle) {
    angle = fmodf(angle, LOVE_M_TAU);

    if (angle < 0)
        angle += LOVE_M_TAU;

    return angle;
};

inline std::vector<Vector2> GenerateOutline(const Vector2* points, size_t count, float lineWidth)
{
    std::vector<Vector2> innerPoints(count);

    for (size_t startPoint = 0; startPoint < count; startPoint++)
    {
        const auto& middle = points[startPoint];
        const auto& after  = points[(startPoint + 1) % count];
        const auto& before = points[startPoint == 0 ? count - 1 : startPoint - 1];

        const float theta = normalizeAngle(atan2f(middle.y - after.y, middle.x - after.x));
        const float phi   = normalizeAngle(atan2f(middle.y - before.y, middle.x - before.x));

        const float angleWithinPolygon   = normalizeAngle(phi - theta);
        const float angleOfRightTriangle = (LOVE_M_PI - angleWithinPolygon) / 2;

        const float lengthOfKite = lineWidth * (1 / cosf(angleOfRightTriangle));

        const float offsetX = cosf(theta + (LOVE_M_PI_2 - angleOfRightTriangle)) * lengthOfKite;
        const float offsetY = sinf(theta + (LOVE_M_PI_2 - angleOfRightTriangle)) * lengthOfKite;

        innerPoints[startPoint] = Vector2(middle.x - offsetX, middle.y - offsetY);
    }

    return innerPoints;
}

/* actual primitives */

void Graphics<Console::CTR>::Points(const Vector2* points, size_t count, const Color* colors,
                                    size_t colorCount)
{
    const auto& transform = this->GetTransform();
    C2D_ViewRestore(&transform.GetElements());
    const auto pointSize = this->GetPointSize();

    for (size_t index = 0; index < count; index++)
    {
        Color color         = colors[0];
        const Vector2 point = points[index];

        if (index < colorCount)
            color = colors[index];

        u32 pointColor = C2D_Color32f(color.r, color.g, color.b, color.a);
        C2D_DrawCircleSolid(point.x, point.y, Graphics::CURRENT_DEPTH, pointSize, pointColor);
    }
}

void Graphics<Console::CTR>::Polyfill(const Vector2* points, size_t count, u32 color, float depth)
{
    for (size_t index = 2; index < count; index++)
    {
        C2D_DrawTriangle(points[0].x, points[0].y, color, points[index - 1].x, points[index - 1].y,
                         color, points[index].x, points[index].y, color, depth);
    }
}

void Graphics<Console::CTR>::Polygon(DrawMode mode, const Vector2* points, size_t count)
{
    const auto color      = this->GetColor();
    const auto& transform = this->GetTransform();

    C2D_ViewRestore(&transform.GetElements());

    if (mode == DRAW_LINE)
        this->Polyline(points, count);
    else
        this->Polyfill(points, count, color.rgba(), Graphics::CURRENT_DEPTH);
}

void Graphics<Console::CTR>::Polyline(const Vector2* points, size_t count)
{
    // Generate the outline and draw it
    const auto lineWidth         = this->GetLineWidth();
    std::vector<Vector2> outline = GenerateOutline(points, count, lineWidth);

    const auto depth = this->PushCurrentDepth();
    this->Polyfill(outline.data(), outline.size(), TRANSPARENCY, depth);

    // Draw our filled polygon
    this->Polygon(DRAW_FILL, points, count);
    Graphics::CURRENT_DEPTH = this->PushCurrentDepth(1.0f);
}

void Graphics<Console::CTR>::Rectangle(DrawMode mode, float x, float y, float width, float height)
{
    const Vector2 points[4] = {
        {        x,          y},
        {x + width,          y},
        {x + width, y + height},
        {        x, y + height}
    };

    this->Polygon(mode, points, 4);
}

void Graphics<Console::CTR>::Rectangle(DrawMode mode, float x, float y, float width, float height,
                                       float rx, float ry)
{
    if (rx == 0 && ry == 0)
    {
        this->Rectangle(mode, x, y, width, height);
        return;
    }

    const auto color      = this->GetColor().rgba();
    const auto& transform = this->GetTransform();

    C2D_ViewRestore(&transform.GetElements());

    /* Offset the radii *properly* */
    Vector2 offset(x + rx, y + ry);
    Vector2 size(rx * 2, ry * 2);

    /*
    ** Ellipse Drawing Order
    ** 1 - 4
    ** |   |
    ** 2 - 3
    */

    if (mode == DRAW_FILL)
    {
        /* Draw Ellipses first on Fill mode */
        auto depth = this->PushCurrentDepth(2.0f);
        C2D_DrawEllipseSolid(x, y, depth, size.x, size.y, color);
        C2D_DrawEllipseSolid(x, y + (height - size.y), depth, size.x, size.y, color);
        C2D_DrawEllipseSolid(x + (width - size.x), y + (height - size.y), depth, size.x, size.y,
                             color);
        C2D_DrawEllipseSolid(x + (width - size.x), y, depth, size.x, size.y, color);

        /* Draw Rectangles */

        C2D_DrawRectSolid(offset.x, y, this->PushCurrentDepth(), width - size.x, height, color);
        C2D_DrawRectSolid(x, offset.y, Graphics::CURRENT_DEPTH, width, height - size.y, color);

        Graphics::CURRENT_DEPTH = this->PushCurrentDepth(2.0f);
    }
    else
    {
        const auto& lineWidth = this->GetLineWidth();
        Vector2 innerDiameter((rx - lineWidth) * 2, (ry - lineWidth) * 2);

        if (innerDiameter.x <= 0 || innerDiameter.y <= 0)
        {
            innerDiameter.x = 0;
            innerDiameter.y = 0;
        }

        /* normal rect offset + line width */
        Vector2 lineOffset((x + rx) + lineWidth, (y + ry) + lineWidth);

        /* normal radii size - line width */
        Vector2 lineSize((rx * 2) - lineWidth, (ry * 2) - lineWidth);

        /* normal radii pos + line width */
        Vector2 linePos(x + lineWidth, y + lineWidth);

        /* normal rect size - line width */
        Vector2 rectSize(width - (lineWidth * 2), height - (lineWidth * 2));

        /* Transparent rectangles first */

        auto depth = this->PushCurrentDepth(3.0f);
        C2D_DrawRectSolid(x + innerDiameter.x / 2 + lineWidth, y + lineWidth, depth,
                          width - (lineWidth * 2 + innerDiameter.x), height - lineWidth * 2,
                          TRANSPARENCY);

        C2D_DrawRectSolid(x + lineWidth, y + innerDiameter.y / 2 + lineWidth, depth,
                          width - (lineWidth * 2), height - (lineWidth * 2 + innerDiameter.y),
                          TRANSPARENCY);

        /* Transparent ellipses second, if they aren't nonexistent */

        if (innerDiameter.x > 0 && innerDiameter.y > 0)
        {
            C2D_DrawEllipseSolid(x + lineWidth, y + lineWidth, depth, innerDiameter.x,
                                 innerDiameter.y, TRANSPARENCY);

            C2D_DrawEllipseSolid(x + lineWidth, y + height - ry - innerDiameter.y / 2, depth,
                                 innerDiameter.x, innerDiameter.y, TRANSPARENCY);

            C2D_DrawEllipseSolid(x + width - rx - innerDiameter.x / 2,
                                 y + height - ry - innerDiameter.y / 2, depth, innerDiameter.x,
                                 innerDiameter.y, TRANSPARENCY);

            C2D_DrawEllipseSolid(x + width - rx - innerDiameter.x / 2, y + lineWidth, depth,
                                 innerDiameter.x, innerDiameter.y, TRANSPARENCY);
        }

        /* Solid stuff  -- Start with ellipses */
        depth = this->PushCurrentDepth(2.0f);
        C2D_DrawEllipseSolid(x, y, depth, size.x, size.y, color);
        C2D_DrawEllipseSolid(x, y + (height - size.y), depth, size.x, size.y, color);
        C2D_DrawEllipseSolid(x + (width - size.x), y + (height - size.y), depth, size.x, size.y,
                             color);
        C2D_DrawEllipseSolid(x + (width - size.x), y, depth, size.x, size.y, color);

        /* Rectangles */

        C2D_DrawRectSolid(offset.x, y, this->PushCurrentDepth(), width - size.x, height, color);
        C2D_DrawRectSolid(x, offset.y, Graphics::CURRENT_DEPTH, width, height - size.y, color);

        /* Ellipses */

        Graphics::CURRENT_DEPTH += this->PushCurrentDepth(3.0f);
    }
}

void Graphics<Console::CTR>::Ellipse(DrawMode mode, float x, float y, float a, float b)
{
    const auto color      = this->GetColor().rgba();
    const auto& transform = this->GetTransform();

    C2D_ViewRestore(&transform.GetElements());

    if (mode == DRAW_FILL)
        C2D_DrawEllipseSolid(x - a, y - b, Graphics::CURRENT_DEPTH, a * 2, b * 2, color);
    else
    {
        const auto lineWidth = this->GetLineWidth();

        C2D_DrawEllipseSolid((x - a) + lineWidth, (y - b) + lineWidth, this->PushCurrentDepth(1.0f),
                             (a - lineWidth) * 2, (b - lineWidth) * 2, TRANSPARENCY);

        C2D_DrawEllipseSolid(x - a, y - b, Graphics::CURRENT_DEPTH, a * 2, b * 2, color);

        Graphics::CURRENT_DEPTH += this->PushCurrentDepth();
    }
}

void Graphics<Console::CTR>::Circle(DrawMode mode, float x, float y, float radius)
{
    const auto color      = this->GetColor().rgba();
    const auto& transform = this->GetTransform();

    C2D_ViewRestore(&transform.GetElements());

    if (mode == DRAW_FILL)
        C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH, radius, color);
    else
    {
        const auto lineWidth = this->GetLineWidth();
        C2D_DrawCircleSolid(x, y, this->PushCurrentDepth(), radius - lineWidth, TRANSPARENCY);
        C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH, radius, color);

        Graphics::CURRENT_DEPTH += this->PushCurrentDepth();
    }
}

void Graphics<Console::CTR>::Arc(DrawMode mode, ArcMode arcmode, float x, float y, float radius,
                                 float angle1, float angle2)
{
    const float diag_radius   = LOVE_M_SQRT2 * radius;
    const auto calc90Triangle = [radius = diag_radius](float x, float y,
                                                       float angle) -> std::array<Vector2, 3> {
        return { Vector2(x, y), Vector2(x + radius * cosf(angle), y + radius * sinf(angle)),
                 Vector2(x + radius * sqrtf(2) * cosf(angle + LOVE_M_PI_2),
                         y + radius * sqrtf(2) * sinf(angle + LOVE_M_PI_2)) };
    };

    angle1 = normalizeAngle(angle1);
    angle2 = normalizeAngle(angle2);

    // Only go around counterclockwise rather than having a conditional
    if (angle2 > angle1)
        angle2 -= LOVE_M_TAU;

    const auto& transform = this->GetTransform();
    C2D_ViewRestore(&transform.GetElements());

    while (angle2 + LOVE_M_PI_2 < angle1)
    {
        const auto& pts = calc90Triangle(x, y, angle2);
        C2D_DrawTriangle(pts[0].x, pts[0].y, TRANSPARENCY, pts[1].x, pts[1].y, TRANSPARENCY,
                         pts[2].x, pts[2].y, TRANSPARENCY, this->PushCurrentDepth());
        angle2 += LOVE_M_PI_2;
    }

    const std::array<Vector2, 3> finalTriangle = {
        Vector2(x, y), Vector2(x + diag_radius * cosf(angle2), y + diag_radius * sinf(angle2)),
        Vector2(x + diag_radius * cosf(angle1), y + diag_radius * sinf(angle1))
    };

    C2D_DrawTriangle(finalTriangle[0].x, finalTriangle[0].y, TRANSPARENCY, finalTriangle[1].x,
                     finalTriangle[1].y, TRANSPARENCY, finalTriangle[2].x, finalTriangle[2].y,
                     TRANSPARENCY, this->PushCurrentDepth());

    /* Sort of code duplication, but uh.. fix the arcs! */

    const auto color = this->GetColor().rgba();

    if (mode == DRAW_FILL)
        C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH, radius, color);
    else
    {
        const auto lineWidth = this->GetLineWidth();
        C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH, radius - lineWidth,
                            TRANSPARENCY);

        C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH, radius, color);

        Graphics::CURRENT_DEPTH += this->PushCurrentDepth();
    }

    Graphics::CURRENT_DEPTH += this->PushCurrentDepth();
}

void Graphics<Console::CTR>::Line(const Vector2* points, int count)
{
    const auto color      = this->GetColor().rgba();
    const auto& transform = this->GetTransform();
    const auto lineWidth  = this->GetLineWidth();

    C2D_ViewRestore(&transform.GetElements());

    for (size_t index = 1; index < (size_t)count; index++)
    {
        C2D_DrawLine(points[index - 1].x, points[index - 1].y, color, points[index].x,
                     points[index].y, color, lineWidth, Graphics::CURRENT_DEPTH);
    }
}

void Graphics<Console::CTR>::Set3D(bool enabled)
{
    return ::Renderer::Instance().Set3D(enabled);
}

bool Graphics<Console::CTR>::Get3D()
{
    return ::Renderer::Instance().Get3D();
}

void Graphics<Console::CTR>::SetMode(int x, int y, int width, int height)
{
    ::Renderer::Instance().CreateFramebuffers();
    this->RestoreState(this->states.back());

    this->created = true;
}

void Graphics<Console::CTR>::IntersectScissor(const Rect& rectangle)
{
    Rect currect = states.back().scissor.bounds;

    if (!states.back().scissor.active)
    {
        currect.x = 0;
        currect.y = 0;
        currect.w = std::numeric_limits<int>::max();
        currect.h = std::numeric_limits<int>::max();
    }

    int x1 = std::max(currect.x, rectangle.x);
    int y1 = std::max(currect.y, rectangle.y);

    int x2 = std::min(currect.x + currect.w, rectangle.x + rectangle.w);
    int y2 = std::min(currect.y + currect.h, rectangle.y + rectangle.h);

    Rect newrect = { x1, y1, std::max(0, x2 - x1), std::max(0, y2 - y1) };
    SetScissor(newrect);
}

void Graphics<Console::CTR>::SetScissor()
{
    Graphics<Console::ALL>::SetScissor();
    ::Renderer::Instance().SetScissor(Rect::EMPTY, false);
}

void Graphics<Console::CTR>::SetScissor(const Rect& scissor)
{
    Graphics<Console::ALL>::SetScissor(scissor);
    ::Renderer::Instance().SetScissor(scissor, this->IsRenderTargetActive());
}
