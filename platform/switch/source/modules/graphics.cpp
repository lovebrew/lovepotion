#include "deko3d/graphics.h"
#include "polyline/common.h"

#include "common/bidirectionalmap.h"
#include "common/render/renderer.h"

#include "deko3d/deko.h"
#include "deko3d/vertexattribs.h"

#include "debug/logfile.h"

namespace love
{
    Renderer& Graphics::GetRenderer()
    {
        return ::deko3d::Instance();
    }

    namespace deko3d
    {
        Graphics::Graphics() : streamDrawState {}
        {
            try
            {
                for (int i = 0; i < Shader::STANDARD_MAX_ENUM; i++)
                {
                    if (!Shader::standardShaders[i])
                        Shader::standardShaders[i] = this->NewShader(Shader::StandardShader(i));
                }
            }
            catch (love::Exception&)
            {
                throw;
            }

            Shader::AttachDefault(Shader::STANDARD_DEFAULT);
        }

        Graphics::~Graphics()
        {}

        void Graphics::Polyline(const Vector2* points, size_t count)
        {
            float halfWidth = this->GetLineWidth() * 0.5f;
            float pixelSize = 1.0f / std::max((float)pixelScaleStack.back(), 0.000001f);

            LineJoin lineJoin   = this->GetLineJoin();
            LineStyle lineStyle = this->GetLineStyle();

            bool drawOverdraw = (lineStyle == LINE_SMOOTH);

            if (lineJoin == LINE_JOIN_NONE)
            {
                NoneJoinPolyline line;
                line.Render(points, count, halfWidth, pixelSize, drawOverdraw);

                line.Draw(this);
            }
            else if (lineJoin == LINE_JOIN_BEVEL)
            {
                BevelJoinPolyline line;
                line.Render(points, count, halfWidth, pixelSize, drawOverdraw);

                line.Draw(this);
            }
            else if (lineJoin == LINE_JOIN_MITER)
            {
                MiterJoinPolyline line;
                line.Render(points, count, halfWidth, pixelSize, drawOverdraw);

                line.Draw(this);
            }
        }

        void Graphics::Polygon(DrawMode mode, const Vector2* points, size_t count,
                               bool skipLastVertex)
        {
            if (mode == DRAW_LINE)
                this->Polyline(points, count);
            else
            {

                Colorf color     = this->GetColor();
                const Matrix4& t = this->GetTransform();
                bool is2D        = t.IsAffine2DTransform();

                int vertexCount = (int)count - ((skipLastVertex) ? 1 : 0);
                DrawCommand command(Vertex::PRIMITIVE_TRIANGLE_FAN, vertexCount);

                if (is2D)
                    t.TransformXY(command.GetPositions(), points, vertexCount);

                Vertex::PrimitiveVertex* vertexData = command.GetVertices();

                for (size_t index = 0; index < (size_t)vertexCount; index++)
                {
                    vertexData[index] = { .position = { command.positions[index].x,
                                                        command.positions[index].y, 0.0f },
                                          .color    = { color.r, color.g, color.b, color.a },
                                          .texcoord = { 0, 0 } };
                }

                this->RequestStreamDraw(command);
            }
        } // namespace deko3d

        void Graphics::Line(const Vector2* points, int count)
        {
            this->Polyline(points, count);
        }

        void Graphics::Rectangle(DrawMode mode, float x, float y, float width, float height)
        {
            Vector2 coords[5] = { Vector2(x, y), Vector2(x, y + height),
                                  Vector2(x + width, y + height), Vector2(x + width, y),
                                  Vector2(x, y) };

            this->Polygon(mode, coords, 5);
        }

        void Graphics::Rectangle(DrawMode mode, float x, float y, float width, float height,
                                 float rx, float ry, int points)
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

            const float half_pi = static_cast<float>(LOVE_M_PI / 2);
            float angle_shift   = half_pi / ((float)points + 1.0f);

            int num_coords = (points + 2) * 4;

            Vector2 coords[num_coords + 1] = {};
            float phi                      = .0f;

            for (int i = 0; i <= points + 2; ++i, phi += angle_shift)
            {
                coords[i].x = x + rx * (1 - cosf(phi));
                coords[i].y = y + ry * (1 - sinf(phi));
            }

            phi = half_pi;

            for (int i = points + 2; i <= 2 * (points + 2); ++i, phi += angle_shift)
            {
                coords[i].x = x + width - rx * (1 + cosf(phi));
                coords[i].y = y + ry * (1 - sinf(phi));
            }

            phi = 2 * half_pi;

            for (int i = 2 * (points + 2); i <= 3 * (points + 2); ++i, phi += angle_shift)
            {
                coords[i].x = x + width - rx * (1 + cosf(phi));
                coords[i].y = y + height - ry * (1 + sinf(phi));
            }

            phi = 3 * half_pi;

            for (int i = 3 * (points + 2); i <= 4 * (points + 2); ++i, phi += angle_shift)
            {
                coords[i].x = x + rx * (1 - cosf(phi));
                coords[i].y = y + height - ry * (1 + sinf(phi));
            }

            coords[num_coords] = coords[0];

            this->Polygon(mode, coords, num_coords + 1);
        }

        void Graphics::Rectangle(DrawMode mode, float x, float y, float width, float height,
                                 float rx, float ry)
        {
            int points = this->CalculateEllipsePoints(std::min(rx, std::abs(width / 2)),
                                                      std::min(ry, std::abs(height / 2)));

            this->Rectangle(mode, x, y, width, height, rx, ry, points);
        }

        void Graphics::Ellipse(DrawMode mode, float x, float y, float a, float b, int points)
        {
            float two_pi = (float)(LOVE_M_PI * 2);
            if (points <= 0)
                points = 1;

            float angle_shift = (two_pi / points);
            float phi         = .0f;

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

        void Graphics::Circle(DrawMode mode, float x, float y, float radius)
        {
            this->Ellipse(mode, x, y, radius, radius);
        }

        void Graphics::Circle(DrawMode mode, float x, float y, float radius, int points)
        {
            this->Ellipse(mode, x, y, radius, radius, points);
        }

        void Graphics::Ellipse(DrawMode mode, float x, float y, float a, float b)
        {
            this->Ellipse(mode, x, y, a, b, this->CalculateEllipsePoints(a, b));
        }

        void Graphics::Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius,
                           float angle1, float angle2, int points)
        {
            /*
            ** Nothing to display with no points or equal angles.
            ** (Or is there with line mode?)
            */
            if (points <= 0 || angle1 == angle2)
                return;

            // Oh, you want to draw a circle?
            if (fabs(angle1 - angle2) >= 2.0f * (float)LOVE_M_PI)
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
            if (drawmode == DRAW_LINE && arcmode == ARC_CLOSED &&
                fabsf(angle1 - angle2) < LOVE_TORAD(4))
                arcmode = ARC_OPEN;

            /*
            ** Quick fix for the last part of a filled open arc not being drawn (because
            ** polygon(DRAW_FILL, ...) doesn't work without a closed loop of vertices.)
            */
            if (drawmode == DRAW_FILL && arcmode == ARC_OPEN)
                arcmode = ARC_CLOSED;

            float phi = angle1;

            int num_coords  = 0;
            Vector2* coords = nullptr;

            const auto createPoints = [&](Vector2* coordinates) {
                for (int i = 0; i <= points; ++i, phi += angle_shift)
                {
                    coordinates[i].x = x + radius * cosf(phi);
                    coordinates[i].y = y + radius * sinf(phi);
                }
            };

            if (arcmode == ARC_PIE)
            {
                num_coords = points + 3;
                coords     = new Vector2[num_coords];

                coords[0] = coords[num_coords - 1] = Vector2(x, y);

                createPoints(coords + 1);
            }
            else if (arcmode == ARC_OPEN)
            {
                num_coords = points + 1;
                coords     = new Vector2[num_coords];

                createPoints(coords);
            }
            else // ARC_CLOSED
            {
                num_coords = points + 2;
                coords     = new Vector2[num_coords];

                createPoints(coords);

                // Connect the ends of the arc.
                coords[num_coords - 1] = coords[0];
            }

            this->Polygon(drawmode, coords, num_coords);
            delete[] coords;
        }

        void Graphics::Points(const Vector2* points, size_t count, const Colorf* colors,
                              size_t colorCount)
        {
            const Matrix4& t = this->GetTransform();
            bool is2D        = t.IsAffine2DTransform();

            Vector2 transformed[count];
            std::fill_n(transformed, count, Vector2 {});

            if (is2D)
                t.TransformXY(transformed, points, count);

            Colorf colorList[colorCount];
            memcpy(colorList, colors, colorCount);

            auto vertices = Vertex::GeneratePrimitiveFromVectors(std::span(transformed, count),
                                                                 std::span(colorList, colorCount));

            // ::deko3d::Instance().RenderPoints(vertices.get(), count);
        }

        void Graphics::Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius,
                           float angle1, float angle2)
        {
            float points = (float)this->CalculateEllipsePoints(radius, radius);

            // The amount of points is based on the fraction of the circle created by the arc.
            float angle = fabsf(angle1 - angle2);
            if (angle < 2.0f * (float)LOVE_M_PI)
                points *= angle / (2.0f * (float)LOVE_M_PI);

            this->Arc(drawmode, arcmode, x, y, radius, angle1, angle2, (int)(points + 0.5f));
        }

        int Graphics::CalculateEllipsePoints(float rx, float ry) const
        {
            int points =
                (int)sqrtf(((rx + ry) / 2.0f) * 20.0f * (float)this->pixelScaleStack.back());
            return std::max(points, 8);
        }

        /* Primitives */

        Shader* love::deko3d::Graphics::NewShader(Shader::StandardShader type)
        {
            Shader* s = new Shader();
            s->LoadDefaults(type);

            return s;
        }

        Font* Graphics::NewDefaultFont(int size, TrueTypeRasterizer::Hinting hinting)
        {
            auto fontModule = Module::GetInstance<FontModule>(M_FONT);
            if (!fontModule)
                throw love::Exception("Font module has not been loaded.");

            StrongReference<Rasterizer> r(
                fontModule->NewTrueTypeRasterizer(size, TrueTypeRasterizer::HINTING_NORMAL),
                Acquire::NORETAIN);

            return this->NewFont(r.Get());
        }

        void Graphics::RequestStreamDraw(const DrawCommand& command)
        {
            StreamDrawState& state = this->streamDrawState;

            bool shouldFlush  = false;
            bool shouldResize = false;

            if (command.shaderType != state.shaderType || command.texture != state.texture ||
                command.primitiveMode != state.primitveMode ||
                command.vertexFormat != state.vertexFormat)

            {
                shouldFlush = true;
            }

            size_t totalVertices = command.count + state.count;
            size_t totalSize     = totalVertices * Vertex::PRIM_VERTEX_SIZE;

            /* flush if we hit the limit (max of uint16_t) */
            if (totalVertices > std::numeric_limits<uint16_t>::max())
                shouldFlush = true;

            /* flush if we have data and total verts is bigger than our state count */
            if (state.vertices.data() != nullptr && totalSize > state.size)
                shouldFlush = true;

            /* resize if total size exceeds state size */
            if (totalSize > state.size)
                shouldResize = true;

            if (shouldFlush || shouldResize)
            {
                this->FlushStreamDraws();

                state.primitveMode   = command.primitiveMode;
                state.textureHandles = command.textureHandles;
                state.vertexFormat   = command.vertexFormat;
                state.shaderType     = command.shaderType;
                state.texture        = command.texture;
            }

            if (Shader::IsDefaultActive() && !Shader::IsActive(state.shaderType))
                Shader::AttachDefault(state.shaderType);

            if (shouldResize)
            {
                if (state.count < totalVertices)
                    state.vertices.resize(totalVertices);
            }

            if (state.vertices.data())
                memcpy(state.vertices.data() + state.count, command.vertices.get(), command.size);

            state.size = state.count * Vertex::PRIM_VERTEX_SIZE;

            if (state.count > 0)
                this->drawCallsBatched++;

            state.count += command.count;
        }

        void Graphics::FlushStreamDraws()
        {
            StreamDrawState& state = this->streamDrawState;

            if (state.count == 0 || state.size == 0)
                return;

            ::deko3d::Instance().Render(state);

            state.count = 0;
            state.size  = 0;
        }
    } // namespace deko3d
} // namespace love
