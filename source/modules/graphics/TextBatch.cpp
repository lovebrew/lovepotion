#include "modules/graphics/TextBatch.hpp"

#if defined(__3DS__)
    #include "driver/display/citro3d.hpp"
#endif

#include "modules/graphics/Graphics.tcc"

#include <algorithm>

#include "common/debug.hpp"

namespace love
{
    Type TextBatch::type("TextBatch", &Drawable::type);

    TextBatch::TextBatch(FontBase* font, const std::vector<ColoredString>& text) :
        font(font),
        vertexAtributesID(font->getVertexAttributesID()),
        vertexData(nullptr),
        modifiedVertices(),
        vertexOffset(0),
        textureCacheID(font->getTextureCacheID())
    {
        this->set(text);
    }

    TextBatch::~TextBatch()
    {
        if (this->vertexData != nullptr)
            std::free(this->vertexData);
    }

    void TextBatch::uploadVertices(const std::vector<FontBase::GlyphVertex>& vertices, size_t vertexOffset)
    {
        size_t offset   = vertexOffset * sizeof(FontBase::GlyphVertex);
        size_t dataSize = vertices.size() * sizeof(FontBase::GlyphVertex);

        if (dataSize > 0 && (!this->vertexBuffer || (offset + dataSize) > this->vertexBuffer->getSize()))
        {
            size_t newSize = size_t((offset + dataSize) * 1.5);

            if (this->vertexBuffer != nullptr)
                newSize = std::max(size_t(this->vertexBuffer->getSize() * 1.5), newSize);

            // clang-format off
            auto* graphics = Module::getInstance<GraphicsBase>(Module::M_GRAPHICS);

            BufferBase::Settings settings(BUFFERUSAGEFLAG_VERTEX, BUFFERDATAUSAGE_DYNAMIC);
            auto decl = BufferBase::getCommonFormatDeclaration(FontBase::VertexFormat);
            StrongRef<BufferBase> newBuffer(graphics->newBuffer(settings, decl, nullptr, newSize, 0), Acquire::NO_RETAIN);
            // clang-format on

            void* newData = nullptr;
            if (this->vertexData != nullptr)
                newData = std::realloc(this->vertexData, newSize);
            else
                newData = std::malloc(newSize);

            if (newData == nullptr)
                throw love::Exception(E_OUT_OF_MEMORY);
            else
                this->vertexData = (uint8_t*)newData;

            this->vertexBuffer = newBuffer;
            this->vertexBuffers.set(0, this->vertexBuffer, 0);
            this->modifiedVertices.encapsulate(0, newSize);
        }

        if (this->vertexData != nullptr && dataSize > 0)
        {
            std::memcpy(this->vertexData + offset, &vertices[0], dataSize);
            this->modifiedVertices.encapsulate(offset, dataSize);
        }
    }

    void TextBatch::regenerateVertices()
    {
        if (this->font->getTextureCacheID() != this->textureCacheID)
        {
            std::vector<TextData> textData = this->textData;
            this->clear();

            for (const TextData& data : textData)
                this->addTextData(data);

            this->textureCacheID = this->font->getTextureCacheID();
        }
    }

    void TextBatch::addTextData(const TextData& data)
    {
        std::vector<FontBase::GlyphVertex> vertices {};
        std::vector<FontBase::DrawCommand> newCommands {};

        TextShaper::TextInfo textInfo {};
        Color constantColor = Color(1.0f, 1.0f, 1.0f, 1.0f);

        // clang-format off
        if (data.align == FontBase::ALIGN_MAX_ENUM)
            newCommands = this->font->generateVertices(data.codepoints, Range(), constantColor, vertices, 0.0f, Vector2(0.0f, 0.0f), &textInfo);
        else
            newCommands = this->font->generateVerticesFormatted(data.codepoints, constantColor, data.wrap, data.align, vertices, &textInfo);
        // clang-format on

        size_t vOffset = this->vertexOffset;

        if (!data.appendVertices)
        {
            vOffset            = 0;
            this->vertexOffset = 0;
            this->drawCommands.clear();
            this->textData.clear();
        }

        if (data.useMatrix && !vertices.empty())
            data.matrix.transformXY(vertices.data(), vertices.data(), (int)vertices.size());

        this->uploadVertices(vertices, vOffset);

        if (!newCommands.empty())
        {
            for (auto& cmd : newCommands)
                cmd.startVertex += (int)vOffset;

            auto firstCmd = newCommands.begin();
            // clang-format off
            if (!this->drawCommands.empty())
            {
                auto prevCmd = this->drawCommands.back();
                if (prevCmd.texture == firstCmd->texture && (prevCmd.startVertex + prevCmd.vertexCount) == firstCmd->startVertex)
                {
                    this->drawCommands.back().vertexCount += firstCmd->vertexCount;
                    ++firstCmd;
                }
            }
            // clang-format on
            this->drawCommands.insert(this->drawCommands.end(), firstCmd, newCommands.end());
        }

        this->vertexOffset = vOffset + vertices.size();
        this->textData.push_back(data);
        this->textData.back().textInfo = textInfo;

        if (this->font->getTextureCacheID() != this->textureCacheID)
            this->regenerateVertices();
    }

    void TextBatch::set(const std::vector<ColoredString>& text)
    {
        return this->set(text, -1.0f, FontBase::ALIGN_MAX_ENUM);
    }

    void TextBatch::set(const std::vector<ColoredString>& text, float wrap, FontBase::AlignMode align)
    {
        if (text.empty() || (text.size() == 1 && text[0].string.empty()))
            return this->clear();

        ColoredCodepoints codepoints {};
        getCodepointsFromString(text, codepoints);

        this->addTextData({ codepoints, wrap, align, {}, false, false, Matrix4() });
    }

    int TextBatch::add(const std::vector<ColoredString>& text, const Matrix4& matrix)
    {
        return this->addf(text, -1.0f, FontBase::ALIGN_MAX_ENUM, matrix);
    }

    int TextBatch::addf(const std::vector<ColoredString>& text, float wrap, FontBase::AlignMode align,
                        const Matrix4& matrix)
    {
        ColoredCodepoints codepoints {};
        getCodepointsFromString(text, codepoints);

        this->addTextData({ codepoints, wrap, align, {}, true, true, matrix });

        return (int)this->textData.size() - 1;
    }

    void TextBatch::clear()
    {
        this->textData.clear();
        this->drawCommands.clear();
        this->textureCacheID = this->font->getTextureCacheID();
        this->vertexOffset   = 0;
    }

    void TextBatch::setFont(FontBase* font)
    {
        this->font.set(font);
        this->textureCacheID    = (uint32_t)-1;
        this->vertexAtributesID = font->getVertexAttributesID();
        this->regenerateVertices();
    }

    FontBase* TextBatch::getFont() const
    {
        return this->font.get();
    }

    int TextBatch::getWidth(int index) const
    {
        if (index < 0)
            index = std::max((int)this->textData.size() - 1, 0);

        if (index >= (int)this->textData.size())
            return 0;

        return this->textData[index].textInfo.width;
    }

    int TextBatch::getHeight(int index) const
    {
        if (index < 0)
            index = std::max((int)this->textData.size() - 1, 0);

        if (index >= (int)this->textData.size())
            return 0;

        return this->textData[index].textInfo.height;
    }

    void TextBatch::draw(GraphicsBase* graphics, const Matrix4& matrix)
    {
        if (this->vertexBuffer == nullptr || this->vertexData == nullptr || this->drawCommands.empty())
            return;

        graphics->flushBatchedDraws();

        if (this->font->getTextureCacheID() != this->textureCacheID)
            this->regenerateVertices();

        if (ShaderBase::isDefaultActive())
            ShaderBase::attachDefault(ShaderBase::getTextureShader());

        if (this->modifiedVertices.isValid())
        {
            size_t offset = this->modifiedVertices.getOffset();
            size_t size   = this->modifiedVertices.getSize();

            if (this->vertexBuffer->getDataUsage() == BUFFERDATAUSAGE_STREAM)
                this->vertexBuffer->fill(0, this->vertexBuffer->getSize(), this->vertexData);
            else
                this->vertexBuffer->fill(offset, size, this->vertexData + offset);

            this->modifiedVertices.invalidate();
        }

        GraphicsBase::TempTransform transform(graphics, matrix);

#if defined(__3DS__)
        if (!this->drawCommands.empty())
            c3d.setTexEnvMode(this->drawCommands[0].texture, true);
#endif

        // clang-format off
        for (const auto& cmd : this->drawCommands)
            graphics->drawQuads(cmd.startVertex / 4, cmd.vertexCount / 4, this->vertexAtributesID, this->vertexBuffers, cmd.texture);
        // clang-format on
    }
} // namespace love
