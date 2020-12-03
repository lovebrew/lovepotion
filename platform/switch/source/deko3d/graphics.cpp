#include "common/runtime.h"
#include "deko3d/graphics.h"

#include "modules/font/fontmodule.h"

using namespace love;

love::deko3d::Graphics::Graphics()
{
    this->RestoreState(this->states.back());

    try
    {
        for (int i = 0; i < 2; i++)
        {
            if (!Shader::standardShaders[i])
                Shader::standardShaders[i] = this->NewShader(Shader::StandardShader(i));
        }
    }
    catch (love::Exception &)
    {
        throw;
    }

    // A shader should always be active, but the default shader shouldn't be
    // returned by getShader(), so we don't do setShader(defaultShader).
    if (!Shader::current)
        Shader::standardShaders[Shader::STANDARD_DEFAULT]->Attach();
}

love::deko3d::Graphics::~Graphics()
{
    // for (int i = 0; i < Shader::STANDARD_MAX_ENUM; i++)
    // {
    //     if (Shader::standardShaders[i])
    //     {
    //         Shader::standardShaders[i]->Release();
    //         Shader::standardShaders[i] = nullptr;
    //     }
    // }
}

void Graphics::SetCanvas(Canvas * canvas)
{
    dk3d.SetDekoBarrier(DkBarrier_Fragments, 0);
    dk3d.BindFramebuffer(canvas);
}

Graphics::RendererInfo love::deko3d::Graphics::GetRendererInfo() const
{
    RendererInfo info {};

    info.name    = RENDERER_NAME;
    info.device  = RENDERER_DEVICE;
    info.vendor  = RENDERER_VENDOR;
    info.version = RENDERER_VERSION;

    return info;
}

void love::deko3d::Graphics::SetColor(Colorf color)
{
    love::Graphics::SetColor(color);
    dk3d.SetBlendColor(color);
}

love::Image * love::deko3d::Graphics::NewImage(Texture::TextureType t, int width, int height)
{
    return new Image(t, width, height);
}

void love::deko3d::Graphics::SetMeshCullMode(vertex::CullMode mode)
{
    DkFace face = DkFace_None;

    switch (mode)
    {
        case vertex::CULL_BACK:
            face = DkFace_Back;
            break;
        case vertex::CULL_FRONT:
            face = DkFace_Front;
            break;
        case vertex::CULL_NONE:
            face = DkFace_None;
            break;
        default:
            break;
    }

    dk3d.SetCullMode(face);
}

void love::deko3d::Graphics::SetFrontFaceWinding(vertex::Winding winding)
{
    DkFrontFace face = DkFrontFace_CW;

    switch (winding)
    {
        case vertex::WINDING_CW:
            face = DkFrontFace_CW;
            break;
        case vertex::WINDING_CCW:
            face = DkFrontFace_CCW;
            break;
        default:
            break;
    }

    dk3d.SetFrontFaceWinding(face);
}

void love::deko3d::Graphics::SetDefaultFilter(const Texture::Filter & filter)
{
    love::Graphics::SetDefaultFilter(filter);
}

void love::deko3d::Graphics::SetBlendMode(BlendMode mode, BlendAlpha alphamode)
{
    if (mode != this->states.back().blendMode || alphamode != this->states.back().blendAlphaMode)
    {} // flush stream draws

    if (alphamode != BLENDALPHA_PREMULTIPLIED)
    {
        const char * modestr = "unknown";
        switch (mode)
        {
            case BLEND_LIGHTEN:
            case BLEND_DARKEN:
            case BLEND_MULTIPLY:
                Graphics::GetConstant(mode, modestr);
                throw love::Exception("The '%s' blend mode must be used with premultiplied alpha.", modestr);
                break;
            default:
                break;
        }
    }


    DkBlendOp func = DkBlendOp_Add;

    DkBlendFactor srcColor = DkBlendFactor_One;
    DkBlendFactor srcAlpha = DkBlendFactor_One;

    DkBlendFactor dstColor = DkBlendFactor_Zero;
    DkBlendFactor dstAlpha = DkBlendFactor_Zero;

    switch (mode)
    {
        case love::Graphics::BLEND_ALPHA:
            srcColor = srcAlpha = DkBlendFactor_One;
            dstColor = dstAlpha = DkBlendFactor_InvSrcAlpha;

            break;
        case love::Graphics::BLEND_MULTIPLY:
            srcColor = srcAlpha = DkBlendFactor_DstColor;
            dstColor = dstAlpha = DkBlendFactor_Zero;

            break;
        case love::Graphics::BLEND_SUBTRACT:
            func = DkBlendOp_RevSub;

            break;
        case love::Graphics::BLEND_ADD:
            srcColor = DkBlendFactor_One;
            srcAlpha = DkBlendFactor_Zero;

            dstColor = dstAlpha = DkBlendFactor_One;

            break;
        case love::Graphics::BLEND_LIGHTEN:
            func = DkBlendOp_Max;

            break;
        case love::Graphics::BLEND_DARKEN:
            func = DkBlendOp_Min;

            break;
        case love::Graphics::BLEND_SCREEN:
            srcColor = srcAlpha = DkBlendFactor_One;
            dstColor = dstAlpha = DkBlendFactor_InvSrcColor;

            break;
        case love::Graphics::BLEND_REPLACE:
        case love::Graphics::BLEND_NONE:
        default:
            srcColor = srcAlpha = DkBlendFactor_One;
            dstColor = dstAlpha = DkBlendFactor_Zero;

            break;
    }

    // We can only do alpha-multiplication when srcRGB would have been unmodified.
    if (srcColor == DkBlendFactor_One && alphamode == BLENDALPHA_MULTIPLY && mode != BLEND_NONE)
        srcColor = DkBlendFactor_SrcAlpha;

    dk3d.SetBlendMode(func, srcColor, srcAlpha, dstColor, dstAlpha);

    this->states.back().blendMode = mode;
    this->states.back().blendAlphaMode = alphamode;
}

Font * love::deko3d::Graphics::NewFont(Rasterizer * data, const Texture::Filter & filter)
{
    return new Font(data, filter);
}

/* Primitives */

void love::deko3d::Graphics::Polygon(DrawMode mode, const Vector2 * points,
                                     size_t count, bool skipLastVertex)
{
    Colorf color[1] = { this->GetColor() };

    const Matrix4 & t = this->GetTransform();
    bool is2D = t.IsAffine2DTransform();

    int vertexCount = (int)count - (mode == DRAW_FILL && skipLastVertex ? 1 : 0);

    std::vector<vertex::Vertex> verts;

    if (is2D)
    {
        Vector2 transformed[vertexCount];
        t.TransformXY(transformed, points, vertexCount);

        verts = vertex::GeneratePrimitiveFromVectors(transformed, vertexCount, color, 1);
    }

    if (mode == DRAW_FILL)
        dk3d.RenderPolygon(verts.data(),  vertexCount * sizeof(*verts.data()), vertexCount);
    else
        dk3d.RenderPolyline(verts.data(), vertexCount * sizeof(*verts.data()), vertexCount);
}

void love::deko3d::Graphics::SetLineWidth(float width)
{
    dk3d.SetLineWidth(width);
}

void love::deko3d::Graphics::Line(float startx, float starty, float endx, float endy)
{
    Vector2 points[4] =
    {
        { startx, starty },
        { endx,   endy   },
        { startx, endy   },
        { endx,   starty }
    };

    this->Polygon(DRAW_LINE, points, 4);
}

void love::deko3d::Graphics::Rectangle(DrawMode mode, float x, float y, float width, float height)
{
    Vector2 points[5] =
    {
        { x, y,                 },
        { x, y + height,        },
        { x + width, y + height },
        { x + width, y,         },
        { x, y,                 }
    };

    this->Polygon(mode, points, 5);
}

void love::deko3d::Graphics::Rectangle(DrawMode mode, float x, float y, float width, float height, float rx, float ry, int points)
{
    if (rx == 0 || ry == 0)
    {
        this->Rectangle(mode, x, y, width, height);
        return;
    }

    // Radius values that are more than half the rectangle's size aren't handled
    // correctly (for now)...

    if (width >= 0.02f)
        rx = std::min(rx, width / 2.0f - 0.01f);
    if (height >= 0.02f)
        ry = std::min(ry, height / 2.0f - 0.01f);

    points = std::max(points / 4, 1);

    const float half_pi = (float)(M_PI / 2);
    float angle_shift = half_pi / ((float)points + 1.0f);

    int num_coords = (points + 2) * 4;

    Vector2 coords[num_coords + 1] = {};
    float phi = .0f;

    for (int i = 0; i <= points + 2; ++i, phi += angle_shift)
    {
        coords[i].x = x + rx * (1 - cosf(phi));
        coords[i].y = y + ry * (1 - sinf(phi));
    }

    phi = half_pi;

    for (int i = points + 2; i <= 2 * (points + 2); ++i, phi += angle_shift)
    {
        coords[i].x = x + width - rx * (1 + cosf(phi));
        coords[i].y = y +         ry * (1 - sinf(phi));
    }

    phi = 2 * half_pi;

    for (int i = 2 * (points + 2); i <= 3 * (points + 2); ++i, phi += angle_shift)
    {
        coords[i].x = x + width  - rx * (1 + cosf(phi));
        coords[i].y = y + height - ry * (1 + sinf(phi));
    }

    phi = 3 * half_pi;

    for (int i = 3 * (points + 2); i <= 4 * (points + 2); ++i, phi += angle_shift)
    {
        coords[i].x = x +          rx * (1 - cosf(phi));
        coords[i].y = y + height - ry * (1 + sinf(phi));
    }

    coords[num_coords] = coords[0];

    this->Polygon(mode, coords, num_coords + 1);
}

void love::deko3d::Graphics::Rectangle(DrawMode mode, float x, float y, float width, float height, float rx, float ry)
{
    this->Rectangle(mode, x, y, width, height, rx, ry, this->CalculateEllipsePoints(rx, ry));
}

void love::deko3d::Graphics::Ellipse(DrawMode mode, float x, float y, float a, float b, int points)
{
    float two_pi = (float)(M_PI * 2);
    if (points <= 0)
        points = 1;

    float angle_shift = (two_pi / points);
    float phi = .0f;

    // 1 extra point at the end for a closed loop, and 1 extra point at the
    // start in filled mode for the vertex in the center of the ellipse.
    int extrapoints = 1 + (mode == DRAW_FILL ? 1 : 0);

    Vector2 coords[points + extrapoints] = {};

    if (mode == DRAW_FILL)
    {
        coords[0].x = x;
        coords[0].y = y;
    }

    for (int i = 0; i < points; ++i, phi += angle_shift)
    {
        coords[i].x = x + a * cosf(phi);
        coords[i].y = y + b * sinf(phi);
    }

    coords[points] = coords[0];

    // Last argument to polygon(): don't skip the last vertex in fill mode.
    this->Polygon(mode, coords, points + extrapoints, false);
}

void love::deko3d::Graphics::Circle(DrawMode mode, float x, float y, float radius)
{
    this->Ellipse(mode, x, y, radius, radius);
}

void love::deko3d::Graphics::Circle(DrawMode mode, float x, float y, float radius, int points)
{
    this->Ellipse(mode, x, y, radius, radius, points);
}

void love::deko3d::Graphics::Ellipse(DrawMode mode, float x, float y, float a, float b)
{
    this->Ellipse(mode, x, y, a, b, this->CalculateEllipsePoints(a, b));
}

void love::deko3d::Graphics::Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius, float angle1, float angle2, int points)
{
    /*
    ** Nothing to display with no points or equal angles.
    ** (Or is there with line mode?)
    */
    if (points <= 0 || angle1 == angle2)
        return;

    // Oh, you want to draw a circle?
    if (fabs(angle1 - angle2) >= 2.0f * (float)M_PI)
    {
        this->Circle(drawmode, x, y, radius, points);
        return;
    }

    float angle_shift = (angle2 - angle1) / points;

    // Bail on precision issues.
    if (angle_shift == 0.0f)
        return;

    /*
    ** Prevent the connecting line from being drawn if a closed line arc has a
    ** small angle. Avoids some visual issues when connected lines are at sharp
    ** angles, due to the miter line join drawing code.
    */
    if (drawmode == DRAW_LINE && arcmode == ARC_CLOSED && fabsf(angle1 - angle2) < LOVE_TORAD(4))
        arcmode = ARC_OPEN;

    /*
    ** Quick fix for the last part of a filled open arc not being drawn (because
    ** polygon(DRAW_FILL, ...) doesn't work without a closed loop of vertices.)
    */
    if (drawmode == DRAW_FILL && arcmode == ARC_OPEN)
        arcmode = ARC_CLOSED;

    float phi = angle1;

    int num_coords = 0;
    Vector2 * coords = nullptr;

    const auto createPoints = [&](Vector2 * coordinates)
    {
        for (int i = 0; i <= points; ++i, phi += angle_shift)
        {
            coordinates[i].x = x + radius * cosf(phi);
            coordinates[i].y = y + radius * sinf(phi);
        }
    };

    if (arcmode == ARC_PIE)
    {
        num_coords = points + 3;
        coords = new Vector2[num_coords];

        coords[0] = coords[num_coords - 1] = Vector2(x, y);

        createPoints(coords + 1);
    }
    else if (arcmode == ARC_OPEN)
    {
        num_coords = points + 1;
        coords = new Vector2[num_coords];

        createPoints(coords);
    }
    else // ARC_CLOSED
    {
        num_coords = points + 2;
        coords = new Vector2[num_coords];

        createPoints(coords);

        // Connect the ends of the arc.
        coords[num_coords - 1] = coords[0];
    }

    this->Polygon(drawmode, coords, num_coords);
    delete [] coords;
}

void love::deko3d::Graphics::Points(const Vector2 * points, size_t count, const Colorf * colors, size_t colorCount)
{
    std::vector<vertex::Vertex> verts = vertex::GeneratePrimitiveFromVectors(points, count, colors, colorCount);

    dk3d.RenderPoints(verts.data(), count * sizeof(*verts.data()), count);
}

void love::deko3d::Graphics::SetPointSize(float size)
{
    dk3d.SetPointSize(size);
    this->states.back().pointSize = size;
}

void love::deko3d::Graphics::Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius, float angle1, float angle2)
{
    float points = (float)this->CalculateEllipsePoints(radius, radius);

    // The amount of points is based on the fraction of the circle created by the arc.
    float angle = fabsf(angle1 - angle2);
    if (angle < 2.0f * (float)M_PI)
        points *= angle / (2.0f * (float)M_PI);

    this->Arc(drawmode, arcmode, x, y, radius, angle1, angle2, (int)(points + 0.5f));
}

int love::deko3d::Graphics::CalculateEllipsePoints(float rx, float ry) const
{
    int points = (int) sqrtf(((rx + ry) / 2.0f) * 20.0f * (float)this->pixelScaleStack.back());
    return std::max(points, 8);
}

/* Primitives */

void love::deko3d::Graphics::Clear(std::optional<Colorf> color, std::optional<int> stencil, std::optional<double> depth)
{
    dk3d.BindFramebuffer();

    if (color.has_value())
    {
        Graphics::GammaCorrectColor(color.value());
        dk3d.ClearColor(color.value());
    }

    if (stencil.has_value())
        dk3d.ClearDepthStencil(depth.value(), stencil.value());

    dk3d.BeginFrame();
}

void love::deko3d::Graphics::Present()
{
    dk3d.Present();
}

void love::deko3d::Graphics::SetScissor(const Rect & scissor)
{
    DisplayState & state = this->states.back();

    dk3d.SetScissor(scissor, false);

    state.scissor = true;
    state.scissorRect = scissor;
}

void love::deko3d::Graphics::SetColorMask(ColorMask mask)
{
    // flushStreamDraws();

    dk3d.SetColorMask(mask.r, mask.g, mask.b, mask.a);
    states.back().colorMask = mask;
}

void love::deko3d::Graphics::SetScissor()
{
    int width, height = 0;
    this->GetDimensions(&width, &height);

    dk3d.SetScissor({0, 0, width, height}, false);
    states.back().scissor = false;
}

Shader * love::deko3d::Graphics::NewShader(Shader::StandardShader type)
{
    Shader * s = new Shader();
    s->LoadDefaults(type);
    return s;
}

Font * love::deko3d::Graphics::NewDefaultFont(int size, TrueTypeRasterizer::Hinting hinting, const Texture::Filter & filter)
{
    auto fontModule = Module::GetInstance<FontModule>(M_FONT);
    if (!fontModule)
        throw love::Exception("Font module has not been loaded.");

    StrongReference<Rasterizer> r(fontModule->NewTrueTypeRasterizer(size, TrueTypeRasterizer::HINTING_NORMAL), Acquire::NORETAIN);

    return new Font(r.Get(), filter);
}