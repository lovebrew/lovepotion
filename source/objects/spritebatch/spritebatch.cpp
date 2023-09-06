#include <objects/spritebatch/spritebatch.hpp>

#include <common/exception.hpp>

#include <objects/texture_ext.hpp>

#include <utilities/temptransform.hpp>

#include <algorithm>
#include <stddef.h>

#include <utilities/log/logfile.hpp>

using namespace love;
using namespace love::vertex;

Type SpriteBatch::type("SpriteBatch", &Drawable::type);

SpriteBatch::SpriteBatch(Texture<Console::Which>* texture, int size) :
    texture(texture),
    size(size),
    next(0),
    color(1.0f, 1.0f, 1.0f, 1.0f),
    colorActive(false),
    rangeStart(-1),
    rangeCount(-1)
{
    if (size <= 0)
        throw love::Exception("Invalid SpriteBatch size.");

    if (texture == nullptr)
        throw love::Exception("A texture must be used when creating a SpriteBatch.");

    this->format       = CommonFormat::TEXTURE;
    this->vertexStride = VERTEX_SIZE;

    this->buffer.reserve(size);
}

int SpriteBatch::Add(const Matrix4<Console::Which>& matrix, int index)
{
    return this->Add(this->texture->GetQuad(), matrix, index);
}

int SpriteBatch::Add(Quad* quad, const Matrix4<Console::Which>& matrix, int index)
{
    /*  todo: layers */

    if (index < -1 || index >= this->size)
        throw love::Exception("Invalid sprite index: %d", index + 1);

    if (index == -1 && this->next >= this->size)
        this->SetBufferSize(this->size * 2);

    const Vector2* quadPositions = quad->GetVertexPositions();

    if (Console::Is(Console::CTR))
    {
        const auto& viewport = quad->GetViewport();

        double width  = NextPo2(this->texture->GetPixelWidth());
        double height = NextPo2(this->texture->GetPixelHeight());

        quad->Refresh(viewport, width, height);
    }

    const Vector2* textureCoords = quad->GetVertexTextureCoords();

    size_t offset  = index == -1 ? this->next : index;
    auto* vertices = this->buffer[offset].data();

    std::array<float, 0x04> color = { 1.0f, 1.0f, 1.0f, 1.0f };

    // clang-format off
    /*
    0    3

    1    2
    */
    std::array<Vertex, 0x06> textureVertices = 
    {
        /*        x                     y                   z                u                    v      */
        Vertex {{ quadPositions[0].x,   quadPositions[0].y, 0.0f }, color, { textureCoords[0].x,  textureCoords[0].y }},
        Vertex {{ quadPositions[1].x,   quadPositions[1].y, 0.0f }, color, { textureCoords[1].x,  textureCoords[1].y }},
        Vertex {{ quadPositions[2].x,   quadPositions[2].y, 0.0f }, color, { textureCoords[2].x,  textureCoords[2].y }},
        Vertex {{ quadPositions[2].x,   quadPositions[2].y, 0.0f }, color, { textureCoords[2].x,  textureCoords[2].y }},
        Vertex {{ quadPositions[3].x,   quadPositions[3].y, 0.0f }, color, { textureCoords[3].x,  textureCoords[3].y }},
        Vertex {{ quadPositions[0].x,   quadPositions[0].y, 0.0f }, color, { textureCoords[0].x,  textureCoords[0].y }}
    };
    // clang-format on

    if (Console::Is(Console::CTR))
    {
        for (auto& vertex : textureVertices)
            vertex.texcoord[1] = 1.0f - vertex.texcoord[1];
    }

    matrix.TransformXYVertPure(vertices, textureVertices.data(), 0x06);

    if (index == -1)
        return this->next++;

    return index;
}

int SpriteBatch::AddLayer(int layer, const Matrix4<Console::Which>& matrix, int index)
{
    return this->AddLayer(layer, this->texture->GetQuad(), matrix, index);
}

int SpriteBatch::AddLayer(int layer, Quad* quad, const Matrix4<Console::Which>& matrix, int index)
{
    if (index < -1 || index >= size)
        throw love::Exception("Invalid sprite index %d.", index);

    if (layer < 0 || layer >= this->texture->GetLayerCount())
    {
        throw love::Exception("Invalid layer: %d (Texture has %d layers)", layer + 1,
                              texture->GetLayerCount());
    }

    return 0;
}

void SpriteBatch::Clear()
{
    this->next = 0;
}

void SpriteBatch::Flush()
{}

void SpriteBatch::SetTexture(Texture<Console::Which>* texture)
{
    if (this->texture->GetTextureType() != texture->GetTextureType())
    {
        throw love::Exception(
            "Texture must have the same texture type as the SpriteBatch's previous texture.");
    }

    this->texture.Set(texture);
}

Texture<Console::Which>* SpriteBatch::GetTexture() const
{
    return this->texture.Get();
}

void SpriteBatch::SetColor(const Color& color)
{
    Color clamped {};

    clamped.r = std::clamp(color.r, 0.0f, 1.0f);
    clamped.g = std::clamp(color.g, 0.0f, 1.0f);
    clamped.b = std::clamp(color.b, 0.0f, 1.0f);
    clamped.a = std::clamp(color.a, 0.0f, 1.0f);

    this->color = clamped;
}

Color SpriteBatch::GetColor() const
{
    return this->color;
}

int SpriteBatch::GetCount() const
{
    return this->next;
}

void SpriteBatch::SetBufferSize(int newSize)
{
    if (newSize <= 0)
        throw love::Exception("Invalid SpriteBatch size");

    if (newSize == this->size)
        return;

    std::vector<std::array<Vertex, 0x06>> newBuffer(newSize);
    int newNext = std::min(next, newSize);

    /* copy the old data into the new buffer */
    std::copy_n(this->buffer.data(), newNext, newBuffer.data());
    this->buffer.clear();

    this->buffer = newBuffer;
    this->size   = newSize;
    this->next   = newNext;
}

int SpriteBatch::GetBufferSize() const
{
    return this->size;
}

void SpriteBatch::AttachAttribute(const std::string& name)
{}

void SpriteBatch::SetDrawRange(int start, int count)
{
    if (start < 0 || count <= 0)
        throw love::Exception("Invalid draw range.");

    this->rangeStart = start;
    this->rangeCount = count;
}

void SpriteBatch::SetDrawRange()
{
    this->rangeStart = this->rangeCount = -1;
}

bool SpriteBatch::GetDrawRange(int& start, int& count) const
{
    if (this->rangeStart < 0 || this->rangeCount <= 0)
        return false;

    start = this->rangeStart;
    count = this->rangeCount;

    return true;
}

void SpriteBatch::Draw(Graphics<Console::Which>& graphics, const Matrix4<Console::Which>& matrix)
{
    if (this->next == 0)
        return;

    Renderer<Console::Which>::FlushVertices();

    if (this->texture.Get())
    {
        if (Shader<Console::Which>::IsDefaultActive())
        {
            auto defaultShader =
                Console::Is(Console::CTR) ? Shader<>::STANDARD_DEFAULT : Shader<>::STANDARD_TEXTURE;

            /* todo: 2d array texture check*/

            Shader<Console::Which>::AttachDefault(defaultShader);
        }
    }

    TempTransform transform(graphics, matrix);

    int start = std::min(std::max(0, this->rangeStart), this->next - 1);
    int count = this->next;

    if (this->rangeCount > 0)
        count = std::min(count, this->rangeCount);

    count = std::min(count, this->next - start);

    /* draw */
    if (count <= 0)
        return;

    const auto shaderType =
        Console::Is(Console::CTR) ? Shader<>::STANDARD_DEFAULT : Shader<>::STANDARD_TEXTURE;

    DrawCommand<Console::CTR> command(count, PRIMITIVE_TRIANGLES, shaderType);
    command.format = CommonFormat::TEXTURE;

#if defined(__3DS__)
    command.handles = { this->texture->GetHandle() };
#else
    command.handles = { this->texture };
#endif

    const auto* source = this->buffer.data()->data();
    transform.TransformXY(command.Positions().get(), source, command.count);

    command.FillVertices(source);

    Renderer<Console::Which>::Instance().Render(command);
}