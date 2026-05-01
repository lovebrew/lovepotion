#include "modules/graphics/SpriteBatch.hpp"

#include "common/Console.hpp"

#if defined(__3DS__)
    #include "driver/display/citro3d.hpp"
#endif

#include "modules/graphics/Graphics.tcc"
#include "modules/graphics/Quad.hpp"
#include "modules/graphics/Texture.tcc"

#include <algorithm>

#include <stddef.h>

#define E_INVALID_SPRITEBATCH_SIZE "Invalid SpriteBatch size: {:d}."
#define E_SPRITEBATCH_ATTRIBUTE_TOO_FEW_VERTICES \
    "Buffer with attribute '%s' attached to this SpriteBatch has too few vertices."

namespace love
{
    Type SpriteBatch::type("SpriteBatch", &Drawable::type);

    SpriteBatch::SpriteBatch(GraphicsBase* graphics, TextureBase* texture, int size, BufferDataUsage usage) :
        texture(texture),
        size(size),
        next(0),
        color(1.0f, 1.0f, 1.0f, 1.0f),
        attributesID(),
        arrayBuffer(nullptr),
        vertexData(nullptr),
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

        this->vertexStride = getFormatStride(this->vertexFormat);
        size_t vertexSize  = this->vertexStride * 4 * size;

        this->vertexData = (uint8_t*)std::malloc(vertexSize);
        if (this->vertexData == nullptr)
            throw love::Exception(E_OUT_OF_MEMORY);

        // clang-format off
        BufferBase::Settings settings(BUFFERUSAGEFLAG_VERTEX, usage);
        auto declaration = BufferBase::getCommonFormatDeclaration(this->vertexFormat);
        this->arrayBuffer.set(graphics->newBuffer(settings, declaration, nullptr, vertexSize, 0), Acquire::NO_RETAIN);
        // clang-format on
    }

    SpriteBatch::~SpriteBatch()
    {
        std::free(this->vertexData);
    }

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

        const Vector2* positions = quad->getVertexPositions();

        int spriteIndex = (index == -1) ? this->next : index;

        size_t offset  = spriteIndex * this->vertexStride * 4;
        auto* vertices = (XYf_STf_RGBAf*)(this->vertexData + offset);

        matrix.transformXY(vertices, positions, 4);

        if constexpr (Console::is(Console::CTR))
            this->texture->updateQuad(quad);

        const Vector2* texCoords = quad->getTextureCoordinates();

        for (int i = 0; i < 4; i++)
        {
            vertices[i].s     = texCoords[i].x;
            vertices[i].t     = texCoords[i].y;
            vertices[i].color = this->color;
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

            if (this->arrayBuffer->getDataUsage() == BUFFERDATAUSAGE_STREAM)
                this->arrayBuffer->fill(0, this->arrayBuffer->getSize(), this->vertexData);
            else
                this->arrayBuffer->fill(offset, size, this->vertexData + offset);

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
        int newNext       = std::min(this->next, newSize);

        void* newData = std::realloc(this->vertexData, vertexSize);
        if (newData == nullptr)
            throw love::Exception(E_OUT_OF_MEMORY);

        // clang-format off
        auto* graphics = Module::getInstance<GraphicsBase>(Module::M_GRAPHICS);
        BufferBase::Settings settings(this->arrayBuffer->getUsageFlags(), this->arrayBuffer->getDataUsage());
        auto decl = BufferBase::getCommonFormatDeclaration(this->vertexFormat);

        this->arrayBuffer.set(graphics->newBuffer(settings, decl, nullptr, vertexSize, 0), Acquire::NO_RETAIN);
        // clang-format on

        this->arrayBuffer->fill(0, this->vertexStride * 4 * newNext, newData);
        this->vertexData = (uint8_t*)newData;

        this->size = newSize;
        this->next = newNext;
        this->attributesID.invalidate();
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

    void SpriteBatch::updateVertexAttributes(GraphicsBase* graphics)
    {
        VertexAttributes attributes {};
        BufferBindings& buffers = this->bufferBindings;

        buffers.set(0, this->arrayBuffer, 0);
        attributes.setCommonFormat(this->vertexFormat, 0);

        int activeBuffers = 1;
        for (const auto& it : this->attachedAttributes)
        {
            auto* buffer = it.second.buffer.get();
            int index    = it.second.bindingIndex;

            if (index >= 0)
            {
                const auto& member = buffer->getDataMember(it.second.index);

                auto offset = (uint16_t)buffer->getMemberOffset(it.second.index);
                auto stride = (uint16_t)buffer->getArrayStride();

                int bufferIndex = activeBuffers;

                for (int i = 1; i < activeBuffers; i++)
                {
                    if (buffers.info[i].buffer == buffer && attributes.bufferLayouts[i].stride == stride)
                    {
                        bufferIndex = i;
                        break;
                    }
                }

                attributes.set(index, member.declaration.format, offset, bufferIndex);
                attributes.setBufferLayout(bufferIndex, stride);
                buffers.set(bufferIndex, buffer, 0);
                activeBuffers = std::max(activeBuffers, bufferIndex + 1);
            }
        }
        this->attributesID = graphics->registerVertexAttributes(attributes);
    }

    static constexpr ShaderBase::StandardShader SHADER_TYPE =
        (Console::is(Console::CTR)) ? ShaderBase::STANDARD_DEFAULT : ShaderBase::STANDARD_TEXTURE;

    void SpriteBatch::draw(GraphicsBase* graphics, const Matrix4& matrix)
    {
        if (this->next == 0)
            return;

        graphics->flushBatchedDraws();
        this->flush();

        bool attributesIDNeedsUpdate = !this->attributesID.isValid();
        for (const auto& it : this->attachedAttributes)
        {
            auto* buffer = it.second.buffer.get();
            if (buffer->getArrayLength() < (size_t)this->next * 4)
                throw love::Exception(E_SPRITEBATCH_ATTRIBUTE_TOO_FEW_VERTICES, it.first.c_str());

            if (it.second.bindingIndex < 0)
                attributesIDNeedsUpdate = true;

            if (it.second.mesh.get())
                it.second.mesh->flush();
        }

        if (attributesIDNeedsUpdate)
            this->updateVertexAttributes(graphics);

        GraphicsBase::TempTransform transform(graphics, matrix);

        int start = std::min(std::max(0, this->rangeStart), this->next - 1);
        int count = this->next;

        if (this->rangeCount > 0)
            count = std::min(count, this->rangeCount);

        count = std::min(count, next - start);

        if (count <= 0)
            return;

#if defined(__3DS__)
        c3d.setTexEnvMode(this->texture, false);
#endif
        graphics->drawQuads(start, count, this->attributesID, this->bufferBindings, this->texture);
    }
} // namespace love
