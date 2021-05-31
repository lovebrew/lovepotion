#include <citro2d.h>

#include "citro2d/graphics.h"

using namespace love;
using Screen = love::Graphics::Screen;

#define TRANSPARENCY C2D_Color32(0, 0, 0, 1)

love::citro2d::Graphics::Graphics()
{
    this->RestoreState(this->states.back());
}

void love::citro2d::Graphics::SetActiveScreen(Screen screen)
{
    switch (screen)
    {
        case Screen::SCREEN_LEFT:
            Graphics::ACTIVE_SCREEN = 0;
            return;
        case Screen::SCREEN_RIGHT:
            Graphics::ACTIVE_SCREEN = 1;
            return;
        case Screen::SCREEN_BOTTOM:
            Graphics::ACTIVE_SCREEN = 2;
            return;
        case Screen::SCREEN_MAX_ENUM:
        default: //< if it defaults from something like "top"
            throw love::Exception("Invalid screen type!"); //< shouldn't happen
            break;
    }
}

void love::citro2d::Graphics::SetBlendMode(BlendMode mode, BlendAlpha alphaMode)
{
    if (alphaMode != BLENDALPHA_PREMULTIPLIED)
    {
        const char* modestr = "unknown";
        switch (mode)
        {
            case BLEND_LIGHTEN:
            case BLEND_DARKEN:
            case BLEND_MULTIPLY:
                love::Graphics::GetConstant(mode, modestr);
                throw love::Exception("The '%s' blend mode must be used with premultiplied alpha.",
                                      modestr);
                break;
            default:
                break;
        }
    }

    GPU_BLENDEQUATION func = GPU_BLEND_ADD;

    GPU_BLENDFACTOR srcColor = GPU_ONE;
    GPU_BLENDFACTOR srcAlpha = GPU_ONE;

    GPU_BLENDFACTOR dstColor = GPU_ZERO;
    GPU_BLENDFACTOR dstAlpha = GPU_ZERO;

    switch (mode)
    {
        case love::Graphics::BLEND_ALPHA:
            srcColor = srcAlpha = GPU_ONE;
            dstColor = dstAlpha = GPU_ONE_MINUS_SRC_ALPHA;

            break;
        case love::Graphics::BLEND_MULTIPLY:
            srcColor = srcAlpha = GPU_DST_COLOR;
            dstColor = dstAlpha = GPU_ZERO;

            break;
        case love::Graphics::BLEND_SUBTRACT:
            func = GPU_BLEND_REVERSE_SUBTRACT;

            break;
        case love::Graphics::BLEND_ADD:
            srcColor = GPU_ONE;
            srcAlpha = GPU_ZERO;

            dstColor = dstAlpha = GPU_ONE;

            break;
        case love::Graphics::BLEND_LIGHTEN:
            func = GPU_BLEND_MAX;

            break;
        case love::Graphics::BLEND_DARKEN:
            func = GPU_BLEND_MIN;

            break;
        case love::Graphics::BLEND_SCREEN:
            srcColor = srcAlpha = GPU_ONE;
            dstColor = dstAlpha = GPU_ONE_MINUS_SRC_COLOR;

            break;
        case love::Graphics::BLEND_REPLACE:
        case love::Graphics::BLEND_NONE:
        default:
            srcColor = srcAlpha = GPU_ONE;
            dstColor = dstAlpha = GPU_ZERO;

            break;
    }

    // We can only do alpha-multiplication when srcRGB would have been unmodified.
    if (srcColor == GPU_ONE && alphaMode == BLENDALPHA_MULTIPLY && mode != BLEND_NONE)
        srcColor = GPU_SRC_ALPHA;

    ::citro2d::Instance().SetBlendMode(func, srcColor, srcAlpha, dstColor, dstAlpha);

    this->states.back().blendMode      = mode;
    this->states.back().blendAlphaMode = alphaMode;
}

const int love::citro2d::Graphics::GetWidth(Screen screen) const
{
    switch (screen)
    {
        case Screen::SCREEN_LEFT:
        case Screen::SCREEN_RIGHT:
        default:
            return SCREEN_TOP_WIDTH;
        case Screen::SCREEN_BOTTOM:
            return SCREEN_BOT_WIDTH;
    }
}

void love::citro2d::Graphics::Set3D(bool enabled)
{
    ::citro2d::Instance().Set3D(enabled);
}

bool love::citro2d::Graphics::Get3D() const
{
    return ::citro2d::Instance().Get3D();
}

const int love::citro2d::Graphics::GetHeight() const
{
    return SCREEN_HEIGHT;
}

Graphics::Screen love::citro2d::Graphics::GetActiveScreen() const
{
    switch (Graphics::ACTIVE_SCREEN)
    {
        case 0:
            return Screen::SCREEN_LEFT;
        case 1:
            return Screen::SCREEN_RIGHT;
        case 2:
            return Screen::SCREEN_BOTTOM;
        default:
            return Screen::SCREEN_LEFT;
    }
}

std::vector<const char*> love::citro2d::Graphics::GetScreens() const
{
    auto constants = (this->Get3D())
                         ? love::Graphics::GetConstants(Screen::SCREEN_MAX_ENUM)
                         : love::citro2d::Graphics::GetConstants(Screen::SCREEN_MAX_ENUM);

    return constants;
}

void love::citro2d::Graphics::Clear(std::optional<Colorf> color, std::optional<int> stencil,
                                    std::optional<double> depth)
{
    if (!this->IsCanvasActive())
        ::citro2d::Instance().BindFramebuffer();

    if (color.has_value())
    {
        Graphics::GammaCorrectColor(color.value());
        ::citro2d::Instance().ClearColor(color.value());
    }
}

void love::citro2d::Graphics::Present()
{
    if (this->IsCanvasActive())
        throw love::Exception("present cannot be called while a Canvas is active.");

    ::citro2d::Instance().Present();
}

/* Keep out from common */
void Graphics::SetCanvas(Canvas* canvas)
{
    DisplayState& state = this->states.back();
    state.canvas.Set(canvas);

    ::citro2d::Instance().BindFramebuffer(canvas);

    if (this->states.back().scissor)
        this->SetScissor(this->states.back().scissorRect);
}

void love::citro2d::Graphics::SetColor(Colorf color)
{
    this->states.back().foreground = color;
}

Font* love::citro2d::Graphics::NewDefaultFont(int size, const Texture::Filter& filter)
{
    auto fontModule = Module::GetInstance<FontModule>(M_FONT);
    if (!fontModule)
        throw love::Exception("Font module has not been loaded.");

    StrongReference<Rasterizer> r(fontModule->NewBCFNTRasterizer(size), Acquire::NORETAIN);

    return new Font(r.Get(), filter);
}

Font* love::citro2d::Graphics::NewFont(Rasterizer* rasterizer, const Texture::Filter& filter)
{
    return new Font(rasterizer, filter);
}

/* Primitives */

inline const auto normalizeAngle = [](float angle) {
    angle = fmodf(angle, M_TAU);
    if (angle < 0)
        angle += M_TAU;

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
        const float angleOfRightTriangle = (M_PI - angleWithinPolygon) / 2;

        const float lengthOfKite = lineWidth * (1 / cosf(angleOfRightTriangle));

        const float offsetX = cosf(theta + (M_PI_2 - angleOfRightTriangle)) * lengthOfKite;
        const float offsetY = sinf(theta + (M_PI_2 - angleOfRightTriangle)) * lengthOfKite;

        innerPoints[startPoint] = Vector2(middle.x - offsetX, middle.y - offsetY);
    }

    return innerPoints;
}

void love::citro2d::Graphics::Polyfill(const Vector2* points, size_t count, u32 color, float depth)
{
    for (size_t currentPoint = 2; currentPoint < count; currentPoint++)
    {
        C2D_DrawTriangle(points[0].x, points[0].y, color, points[currentPoint - 1].x,
                         points[currentPoint - 1].y, color, points[currentPoint].x,
                         points[currentPoint].y, color, depth);
    }
}

void love::citro2d::Graphics::Polygon(DrawMode mode, const Vector2* points, size_t count)
{
    Colorf color   = this->GetColor();
    u32 foreground = C2D_Color32f(color.r, color.g, color.b, color.a);

    const Matrix4& t = this->GetTransform();
    C2D_ViewRestore(&t.GetElements());

    if (mode == DRAW_LINE)
        this->Polyline(points, count);
    else
        this->Polyfill(points, count, foreground, Graphics::CURRENT_DEPTH);
}

void love::citro2d::Graphics::Polyline(const Vector2* points, size_t count)
{
    // Generate the outline and draw it
    std::vector<Vector2> outline = GenerateOutline(points, count, this->states.back().lineWidth);
    this->Polyfill(outline.data(), outline.size(), TRANSPARENCY,
                   Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH);

    // Draw our filled polygon
    this->Polygon(DRAW_FILL, points, count);

    Graphics::CURRENT_DEPTH += Graphics::MIN_DEPTH;
}

void love::citro2d::Graphics::Rectangle(DrawMode mode, float x, float y, float width, float height)
{
    Vector2 points[4] = {
        { x, y }, { x + width, y }, { x + width, y + height }, { x, y + height }
    };

    this->Polygon(mode, points, 4);
}

void love::citro2d::Graphics::Rectangle(DrawMode mode, float x, float y, float width, float height,
                                        float rx, float ry)
{
    if (rx == 0 && ry == 0)
    {
        this->Rectangle(mode, x, y, width, height);
        return;
    }

    Colorf color   = this->GetColor();
    u32 foreground = C2D_Color32f(color.r, color.g, color.b, color.a);

    const Matrix4& t = this->GetTransform();
    C2D_ViewRestore(&t.GetElements());

    if (mode == DRAW_FILL)
    {
        // Top Left
        C2D_DrawEllipseSolid(x - rx, y - ry, Graphics::CURRENT_DEPTH, rx * 2, ry * 2, foreground);

        // Bottom Left
        C2D_DrawEllipseSolid(x - rx, (y - ry) + height, Graphics::CURRENT_DEPTH, rx * 2, ry * 2,
                             foreground);

        // Bottom Right
        C2D_DrawEllipseSolid((x - rx) + width, (y - ry) + height, Graphics::CURRENT_DEPTH, rx * 2,
                             ry * 2, foreground);

        // Top Right
        C2D_DrawEllipseSolid((x - rx) + width, y - ry, Graphics::CURRENT_DEPTH, rx * 2, ry * 2,
                             foreground);

        // Vertical Rectangle
        C2D_DrawRectSolid(x, y - rx, Graphics::CURRENT_DEPTH, width, height + (ry * 2), foreground);

        // Horizontal Rectangle
        C2D_DrawRectSolid(x - rx, y, Graphics::CURRENT_DEPTH, width + (rx * 2), height, foreground);
    }
    else
    {
        float lineWidth = this->states.back().lineWidth;

        float ellipse_x = (x - rx) + lineWidth;
        float ellipse_y = (y - ry) + lineWidth;

        float ellipse_rx = (rx - lineWidth);
        float ellipse_ry = (ry - lineWidth);

        /* Rectangles first */

        C2D_DrawRectSolid(x, y - ellipse_ry, Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH, width,
                          (height + ellipse_ry * 2), TRANSPARENCY);

        C2D_DrawRectSolid(x - ellipse_rx, y, Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH,
                          width + (ellipse_rx * 2), height, TRANSPARENCY);
        /* Circles
        ** 1   4
        ** |   |
        ** 2 - 3
        */

        C2D_DrawEllipseSolid(ellipse_x, ellipse_y, Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH,
                             ellipse_rx * 2, ellipse_ry * 2, TRANSPARENCY);

        C2D_DrawEllipseSolid(ellipse_x, ellipse_y + height,
                             Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH, ellipse_rx * 2,
                             ellipse_ry * 2, TRANSPARENCY);

        C2D_DrawEllipseSolid(ellipse_x + width, ellipse_y + height,
                             Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH, ellipse_rx * 2,
                             ellipse_ry * 2, TRANSPARENCY);

        C2D_DrawEllipseSolid(ellipse_x + width, ellipse_y,
                             Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH, ellipse_rx * 2,
                             ellipse_ry * 2, TRANSPARENCY);

        /* Solid stuff  -- Start with Rectangles */

        C2D_DrawRectSolid(x, y - rx, Graphics::CURRENT_DEPTH, width, height + (ry * 2), foreground);

        C2D_DrawRectSolid(x - rx, y, Graphics::CURRENT_DEPTH, width + (rx * 2), height, foreground);

        /* Ellipses */

        C2D_DrawEllipseSolid(x - rx, y - ry, Graphics::CURRENT_DEPTH, rx * 2, ry * 2, foreground);

        C2D_DrawEllipseSolid(x - rx, (y - ry) + height, Graphics::CURRENT_DEPTH, rx * 2, ry * 2,
                             foreground);

        C2D_DrawEllipseSolid((x - rx) + width, (y - ry) + height, Graphics::CURRENT_DEPTH, rx * 2,
                             ry * 2, foreground);

        C2D_DrawEllipseSolid((x - rx) + width, y - ry, Graphics::CURRENT_DEPTH, rx * 2, ry * 2,
                             foreground);

        Graphics::CURRENT_DEPTH += Graphics::MIN_DEPTH;
    }
}

void love::citro2d::Graphics::Ellipse(DrawMode mode, float x, float y, float a, float b)
{
    Colorf color   = this->GetColor();
    u32 foreground = C2D_Color32f(color.r, color.g, color.b, color.a);

    const Matrix4& t = this->GetTransform();
    C2D_ViewRestore(&t.GetElements());

    if (mode == DRAW_FILL)
        C2D_DrawEllipseSolid(x - a, y - b, Graphics::CURRENT_DEPTH, a * 2, b * 2, foreground);
    else
    {
        float lineWidth = this->states.back().lineWidth;

        C2D_DrawEllipseSolid((x - a) + lineWidth, (y - b) + lineWidth,
                             Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH, (a - lineWidth) * 2,
                             (b - lineWidth) * 2, TRANSPARENCY);

        C2D_DrawEllipseSolid(x - a, y - b, Graphics::CURRENT_DEPTH, a * 2, b * 2, foreground);

        Graphics::CURRENT_DEPTH += Graphics::MIN_DEPTH;
    }
}

void love::citro2d::Graphics::Circle(DrawMode mode, float x, float y, float radius)
{
    Colorf color   = this->GetColor();
    u32 foreground = C2D_Color32f(color.r, color.g, color.b, color.a);

    const Matrix4& t = this->GetTransform();
    C2D_ViewRestore(&t.GetElements());

    if (mode == DRAW_FILL)
        C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH, radius, foreground);
    else
    {
        C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH,
                            radius - this->states.back().lineWidth, TRANSPARENCY);

        C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH, radius, foreground);

        Graphics::CURRENT_DEPTH += Graphics::MIN_DEPTH;
    }
}

void love::citro2d::Graphics::Arc(DrawMode mode, ArcMode arcmode, float x, float y, float radius,
                                  float angle1, float angle2)
{
    const auto calc45Triangle = [](float x, float y, float radius,
                                   float angle) -> std::array<Vector2, 3> {
        return { Vector2(x, y), Vector2(x + radius * cosf(angle), y + radius * sinf(angle)),
                 Vector2(x + radius * sqrtf(2) * cosf(angle + M_PI_4),
                         y + radius * sqrtf(2) * sinf(angle + M_PI_4)) };
    };

    angle1 = normalizeAngle(angle1);
    angle2 = normalizeAngle(angle2);

    // Only go around counterclockwise rather than having a conditional
    if (angle2 > angle1)
        angle2 -= M_TAU;

    const Matrix4& t = this->GetTransform();
    C2D_ViewRestore(&t.GetElements());

    while (angle2 + M_PI_4 < angle1)
    {
        const auto& pts = calc45Triangle(x, y, radius, angle2);
        C2D_DrawTriangle(pts[0].x, pts[0].y, TRANSPARENCY, pts[1].x, pts[1].y, TRANSPARENCY,
                         pts[2].x, pts[2].y, TRANSPARENCY,
                         Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH);
        angle2 += M_PI_4;
    }

    const std::array<Vector2, 3> finalTriangle = {
        Vector2(x, y), Vector2(x + radius * cosf(angle2), y + radius * sinf(angle2)),
        Vector2(x + radius * sqrtf(2) * cosf(angle1), y + radius * sqrtf(2) * sinf(angle1))
    };

    C2D_DrawTriangle(finalTriangle[0].x, finalTriangle[0].y, TRANSPARENCY, finalTriangle[1].x,
                     finalTriangle[1].y, TRANSPARENCY, finalTriangle[2].x, finalTriangle[2].y,
                     TRANSPARENCY, Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH);

    /* Sort of code duplication, but uh.. fix the arcs! */

    Colorf color   = this->GetColor();
    u32 foreground = C2D_Color32f(color.r, color.g, color.b, color.a);

    if (mode == DRAW_FILL)
        C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH, radius, foreground);
    else
    {
        C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH,
                            radius - this->states.back().lineWidth, TRANSPARENCY);

        C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH, radius, foreground);

        Graphics::CURRENT_DEPTH += Graphics::MIN_DEPTH;
    }

    Graphics::CURRENT_DEPTH += Graphics::MIN_DEPTH;
}

void love::citro2d::Graphics::Line(const Vector2* points, int count)
{
    Colorf color   = this->GetColor();
    u32 foreground = C2D_Color32f(color.r, color.g, color.b, color.a);

    const Matrix4& t = this->GetTransform();
    C2D_ViewRestore(&t.GetElements());

    for (size_t index = 1; index < (size_t)count; index += 2)
        C2D_DrawLine(points[index - 1].x, points[index - 1].y, foreground, points[index].x,
                     points[index].y, foreground, this->states.back().lineWidth,
                     Graphics::CURRENT_DEPTH);
}

void love::citro2d::Graphics::SetLineWidth(float width)
{
    this->states.back().lineWidth = width;
}

void love::citro2d::Graphics::SetDefaultFilter(const Texture::Filter& filter)
{
    Texture::defaultFilter = filter;
}

/* End Primitives */

void love::citro2d::Graphics::SetScissor(const Rect& scissor)
{
    DisplayState& state = this->states.back();

    if (state.scissor)
        C2D_Flush();

    int screenWidth = this->GetWidth(this->GetActiveScreen());
    ::citro2d::Instance().SetScissor(GPU_SCISSOR_NORMAL, scissor, screenWidth, false);

    state.scissor     = true;
    state.scissorRect = scissor;
}

void love::citro2d::Graphics::SetScissor()
{
    if (states.back().scissor)
        C2D_Flush();

    int screenWidth = this->GetWidth(this->GetActiveScreen());
    ::citro2d::Instance().SetScissor(GPU_SCISSOR_DISABLE, { 0, 0, screenWidth, 240 }, screenWidth,
                                     false);

    states.back().scissor = false;
}

Graphics::RendererInfo love::citro2d::Graphics::GetRendererInfo() const
{
    RendererInfo info {};

    info.name    = RENDERER_NAME;
    info.device  = RENDERER_DEVICE;
    info.vendor  = RENDERER_VENDOR;
    info.version = RENDERER_VERSION;

    return info;
}

bool love::citro2d::Graphics::GetConstant(const char* in, Screen& out)
{
    return plainScreens.Find(in, out);
}

bool love::citro2d::Graphics::GetConstant(Screen in, const char*& out)
{
    return plainScreens.Find(in, out);
}

std::vector<const char*> love::citro2d::Graphics::GetConstants(Screen)
{
    return plainScreens.GetNames();
}

/* 2D Screens */

// clang-format off
constexpr StringMap<Graphics::Screen, love::citro2d::Graphics::MAX_2D_SCREENS>::Entry plainScreenEntries[] =
{
    { "top",    Graphics::Screen::SCREEN_LEFT   },
    { "bottom", Graphics::Screen::SCREEN_BOTTOM }
};

constinit const StringMap<Graphics::Screen, love::citro2d::Graphics::MAX_2D_SCREENS> love::citro2d::Graphics::plainScreens(plainScreenEntries);

/* "3D" Screens */

constexpr StringMap<Graphics::Screen, Graphics::MAX_SCREENS>::Entry screenEntries[] =
{
    { "left",   Graphics::Screen::SCREEN_LEFT   },
    { "right",  Graphics::Screen::SCREEN_RIGHT  },
    { "bottom", Graphics::Screen::SCREEN_BOTTOM }
};

constinit const StringMap<Graphics::Screen, Graphics::MAX_SCREENS> Graphics::screens(screenEntries);
// clang-format on
