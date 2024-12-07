#include "common/Console.hpp"

#include "modules/graphics/Graphics.tcc"
#include "modules/graphics/TextBatch.hpp"

#include <algorithm>

namespace love
{
    Type TextBatch::type("TextBatch", &Drawable::type);

    TextBatch::TextBatch(FontBase* font, const std::vector<ColoredString>& text) :
        font(font),
        vertexAttributes(CommonFormat::XYf_STf_RGBAf, 0),
        buffer {},
        modifiedVertices(),
        vertexOffset(0),
        textureCacheID((uint32_t)-1)
    {
        this->set(text);
    }

    TextBatch::~TextBatch()
    {}

    void TextBatch::uploadVertices(const std::vector<FontBase::GlyphVertex>& vertices, size_t vertexOffset)
    {
        size_t offset   = vertexOffset * sizeof(FontBase::GlyphVertex);
        size_t dataSize = vertices.size() * sizeof(FontBase::GlyphVertex);

        const auto currentByteSize = this->buffer.size() * sizeof(FontBase::GlyphVertex);

        if (dataSize > 0 && ((offset + dataSize) > currentByteSize))
        {
            size_t newSize = (size_t(offset + dataSize * 1.5)) / sizeof(FontBase::GlyphVertex);

            if (!this->buffer.empty())
                newSize = std::max(size_t(this->buffer.size() * 1.5), newSize);

            this->buffer.resize(newSize);
        }

        if (!this->buffer.empty() && dataSize > 0)
        {
            std::memcpy(&this->buffer[vertexOffset], &vertices[0], dataSize);
            this->modifiedVertices = Range(offset, dataSize);
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

        size_t vertexOffset = this->vertexOffset;

        if (!data.appendVertices)
        {
            vertexOffset       = 0;
            this->vertexOffset = 0;
            this->drawCommands.clear();
            this->textData.clear();
        }

        if (data.useMatrix && !vertices.empty())
            data.matrix.transformXY(vertices.data(), vertices.data(), (int)vertices.size());

        this->uploadVertices(vertices, vertexOffset);

        if (!newCommands.empty())
        {
            for (auto& command : newCommands)
                command.startVertex += vertexOffset;

            auto firstCommand = newCommands.begin();

            if (!this->drawCommands.empty())
            {
                auto previous  = this->drawCommands.back();
                bool sameRange = (previous.startVertex + previous.vertexCount) == firstCommand->startVertex;

                if (previous.texture == firstCommand->texture && sameRange)
                {
                    previous.vertexCount += firstCommand->vertexCount;
                    ++firstCommand;
                }
            }

            this->drawCommands.insert(this->drawCommands.end(), firstCommand, newCommands.end());
        }

        this->vertexOffset += vertices.size();
        this->textData.push_back(data);

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
        this->textureCacheID = (uint32_t)-1;
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

    static constexpr auto shaderType =
        Console::is(Console::CTR) ? ShaderBase::STANDARD_DEFAULT : ShaderBase::STANDARD_TEXTURE;

    void TextBatch::draw(GraphicsBase* graphics, const Matrix4& matrix)
    {
        if (this->buffer.empty() || this->drawCommands.empty())
            return;

        graphics->flushBatchedDraws();

        if (this->font->getTextureCacheID() != this->textureCacheID)
            this->regenerateVertices();

        if (ShaderBase::isDefaultActive())
            ShaderBase::attachDefault(ShaderBase::STANDARD_DEFAULT);

        TextureBase* firstTexture = nullptr;
        if (!this->drawCommands.empty())
            firstTexture = this->drawCommands[0].texture;

        int totalVertices = 0;
        for (const auto& command : this->drawCommands)
            totalVertices = std::max(command.startVertex + command.vertexCount, totalVertices);

        if (this->modifiedVertices.isValid())
            this->modifiedVertices.invalidate();

        GraphicsBase::TempTransform transform(graphics, matrix);

        for (const FontBase::DrawCommand& cmd : this->drawCommands)
        {
            BatchedDrawCommand command {};
            command.format        = CommonFormat::XYf_STf_RGBAf;
            command.indexMode     = TRIANGLEINDEX_QUADS;
            command.vertexCount   = cmd.vertexCount;
            command.texture       = cmd.texture;
            command.shaderType    = shaderType;
            command.isFont        = true;
            command.pushTransform = false;

            auto data                         = graphics->requestBatchedDraw(command);
            FontBase::GlyphVertex* vertexdata = (FontBase::GlyphVertex*)data.stream;

            std::copy_n(&this->buffer[cmd.startVertex], cmd.vertexCount, vertexdata);
        }
    }
} // namespace love
