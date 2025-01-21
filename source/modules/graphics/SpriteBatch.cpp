#include "modules/graphics/SpriteBatch.hpp"

#include "common/Console.hpp"

#include "modules/graphics/Graphics.tcc"
#include "modules/graphics/Quad.hpp"
#include "modules/graphics/Texture.tcc"

#include <algorithm>

#include <stddef.h>

#define E_INVALID_SPRITEBATCH_SIZE "Invalid SpriteBatch size: {:d}."

namespace love
{
    Type SpriteBatch::type("SpriteBatch", &Drawable::type);

    SpriteBatch::SpriteBatch(GraphicsBase* graphics, TextureBase* texture, int size, BufferDataUsage usage) :
        texture(texture),
        size(size),
        next(0),
        color(1.0f, 1.0f, 1.0f, 1.0f),
        buffer {},
        modifiedSprites(),
        rangeStart(-1),
        rangeCount(-1)
    {
        if (size <= 0)
            throw love::Exception(E_INVALID_SPRITEBATCH_SIZE, size);

        if (texture == nullptr)
            throw love::Exception("A texture must be used when creating a SpriteBatch.");

        if (texture->getTextureType() == TEXTURE_2D_ARRAY)
            this->vertexFormat = CommonFormat::XYf_STPf_RGBAf;
        else
            this->vertexFormat = CommonFormat::XYf_STf_RGBAf;

        this->vertexStride = 1;
        size_t vertexSize  = this->vertexStride * 4 * size;

        this->buffer.resize(vertexSize);
    }

    SpriteBatch::~SpriteBatch()
    {}

    int SpriteBatch::add(const Matrix4& matrix, int index)
    {
        return this->add(this->texture->getQuad(), matrix, index);
    }

    int SpriteBatch::add(Quad* quad, const Matrix4& matrix, int index)
    {
        if (this->vertexFormat == CommonFormat::XYf_STPf_RGBAf)
            return this->addLayer(quad->getLayer(), quad, matrix, index);

        if (index < -1 || index >= this->size)
            throw love::Exception("Invalid sprite index: {:d}.", index + 1);

        if (index == -1 && this->next >= this->size)
            this->setBufferSize(this->size * 2);

        if constexpr (Console::is(Console::CTR))
            this->texture->updateQuad(quad);

        const Vector2* positions = quad->getVertexPositions();
        const Vector2* texCoords = quad->getTextureCoordinates();

        int spriteIndex = (index == -1) ? this->next : index;

        size_t offset = spriteIndex * this->vertexStride * 4;
        auto* buffer  = &this->buffer[offset];

        matrix.transformXY(buffer, positions, 4);

        for (int i = 0; i < 4; i++)
        {
            buffer[i].s     = texCoords[i].x;
            buffer[i].t     = texCoords[i].y;
            buffer[i].color = this->color;
        }

        this->modifiedSprites.encapsulate(spriteIndex);

        if (index == -1)
            return this->next++;

        return index;
    }

    int SpriteBatch::addLayer(int layer, const Matrix4& matrix, int index)
    {
        return this->addLayer(layer, this->texture->getQuad(), matrix, index);
    }

    int SpriteBatch::addLayer(int layer, Quad* quad, const Matrix4& matrix, int index)
    {
        return 0;
    }

    void SpriteBatch::clear()
    {
        this->next = 0;
    }

    void SpriteBatch::flush()
    {
        if (this->modifiedSprites.isValid())
        {
            size_t offset = this->modifiedSprites.getOffset() * this->vertexStride * 4;
            size_t size   = this->modifiedSprites.getSize() * this->vertexStride * 4;

            // fill the buffer at offset

            this->modifiedSprites.invalidate();
        }
    }

    void SpriteBatch::setTexture(TextureBase* newTexture)
    {
        if (this->texture->getTextureType() != newTexture->getTextureType())
            throw love::Exception("Texture must have the same type as the SpriteBatch's previous texture.");

        this->texture.set(newTexture);
    }

    TextureBase* SpriteBatch::getTexture() const
    {
        return this->texture.get();
    }

    void SpriteBatch::setColor(const Color& color)
    {
        this->color = color;
    }

    Color SpriteBatch::getColor() const
    {
        return this->color;
    }

    int SpriteBatch::getCount() const
    {
        return this->next;
    }

    void SpriteBatch::setBufferSize(int newSize)
    {
        if (newSize <= 0)
            throw love::Exception(E_INVALID_SPRITEBATCH_SIZE, size);

        if (newSize == this->size)
            return;

        size_t vertexSize = this->vertexStride * 4 * newSize;

        this->buffer.resize(vertexSize);
        this->size = newSize;
        this->next = std::min(this->next, newSize);
    }

    int SpriteBatch::getBufferSize() const
    {
        return this->size;
    }

    void SpriteBatch::setDrawRange(int start, int count)
    {
        if (start < 0 || count <= 0)
            throw love::Exception("Invalid draw range.");

        this->rangeStart = start;
        this->rangeCount = count;
    }

    void SpriteBatch::setDrawRange()
    {
        this->rangeStart = this->rangeCount = -1;
    }

    bool SpriteBatch::getDrawRange(int& start, int& count) const
    {
        if (this->rangeStart < 0 || this->rangeCount <= 0)
            return false;

        start = this->rangeStart;
        count = this->rangeCount;

        return true;
    }

    static constexpr ShaderBase::StandardShader SHADER_TYPE =
        (Console::is(Console::CTR)) ? ShaderBase::STANDARD_DEFAULT : ShaderBase::STANDARD_TEXTURE;

    void SpriteBatch::draw(GraphicsBase* graphics, const Matrix4& matrix)
    {
        if (this->next == 0)
            return;

        GraphicsBase::TempTransform transform(graphics, matrix);

        int start = std::min(std::max(0, this->rangeStart), this->next - 1);
        int count = this->next;

        if (this->rangeCount > 0)
            count = std::min(count, this->rangeCount);

        count = std::min(count, next - start);

        if (count <= 0)
            return;

        BatchedDrawCommand command {};
        command.format        = CommonFormat::XYf_STf_RGBAf;
        command.indexMode     = TRIANGLEINDEX_QUADS;
        command.texture       = this->texture;
        command.vertexCount   = count * 4;
        command.pushTransform = false;
        command.shaderType    = SHADER_TYPE;

        auto data = graphics->requestBatchedDraw(command);

        Vertex* stream = (Vertex*)data.stream;
        std::copy_n(this->buffer.data() + start, command.vertexCount, stream);
    }
} // namespace love
