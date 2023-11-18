#include <common/matrix.hpp>

#include <objects/textbatch/textbatch.hpp>

#include <modules/graphics/graphics.tcc>

#include <utilities/temptransform.hpp>

#include <algorithm>

using namespace love;
using namespace love::vertex;

TextBatch::TextBatch(Font* font, const ColoredStrings& strings) :
    font(font),
    modifiedVertices(),
    vertexOffset(0),
    textureCacheId((uint32_t)-1)
{
    this->Set(strings);
}

TextBatch::~TextBatch()
{}

void TextBatch::SetFont(Font* font)
{
    this->font.Set(font);
    this->textureCacheId = (uint32_t)-1;
    this->RegenerateVertices();
}

void TextBatch::UploadVertices(const std::vector<Vertex>& vertices, size_t vertexOffset)
{
    size_t offset   = vertexOffset * VERTEX_SIZE;
    size_t dataSize = vertices.size() * VERTEX_SIZE;

    const auto currentByteSize = this->buffer.size() * VERTEX_SIZE;

    if (dataSize > 0 && (this->buffer.empty() || (offset + dataSize) > currentByteSize))
    {
        size_t newSize = (size_t((offset + dataSize) * 1.5)) / VERTEX_SIZE;

        if (!this->buffer.empty())
            newSize = std::max(size_t(this->buffer.size() * 1.5), newSize);

        this->buffer.resize(newSize);
    }

    if (!this->buffer.empty() && dataSize > 0)
    {
        std::memcpy(this->buffer.data() + vertexOffset, &vertices[0], dataSize);
        modifiedVertices.encapsulate(offset, dataSize);
    }
}

void TextBatch::RegenerateVertices()
{
    if (this->font->GetTextureCacheID() != this->textureCacheId)
    {
        auto newTextData = this->textData;
        this->Clear();

        for (const auto& text : newTextData)
            this->AddTextData(text);

        this->textureCacheId = this->font->GetTextureCacheID();
    }
}

void TextBatch::AddTextData(const TextData& text)
{
    std::vector<Vertex> vertices {};
    std::vector<Font::DrawCommand> newCommands {};
    TextShaper::TextInfo info {};

    Color constantColor(1, 1, 1, 1);
    if (text.align == Font::ALIGN_MAX_ENUM)
    {
        newCommands = this->font->GenerateVertices(text.codepoints, Range(), constantColor,
                                                   vertices, 0.0f, Vector2(0.0f, 0.0f), &info);
    }
    else
    {
        newCommands = this->font->GenerateVerticesFormatted(text.codepoints, constantColor,
                                                            text.wrap, text.align, vertices, &info);
    }

    size_t vertexOffset = this->vertexOffset;

    if (!text.appendVertices)
    {
        vertexOffset       = 0;
        this->vertexOffset = 0;
        this->drawCommands.clear();
        this->textData.clear();
    }

    if (text.useMatrix && !vertices.empty())
        text.matrix.TransformXY(vertices, vertices);

    this->UploadVertices(vertices, vertexOffset);

    if (!newCommands.empty())
    {
        for (auto& command : newCommands)
            command.start += vertexOffset;

        auto firstCommand = newCommands.begin();

        if (!this->drawCommands.empty())
        {
            auto prevCommand = this->drawCommands.back();
            if (prevCommand.texture == firstCommand->texture &&
                (prevCommand.start + prevCommand.count) == firstCommand->start)
            {
                this->drawCommands.back().count += firstCommand->count;
                ++firstCommand;
            }
        }

        this->drawCommands.insert(this->drawCommands.end(), firstCommand, newCommands.end());
    }

    this->vertexOffset = vertexOffset + vertices.size();
    this->textData.push_back(text);
    this->textData.back().textInfo = info;

    if (this->font->GetTextureCacheID() != this->textureCacheId)
        this->RegenerateVertices();
}

void TextBatch::Clear()
{
    this->textData.clear();
    this->drawCommands.clear();
    this->textureCacheId = this->font->GetTextureCacheID();
    this->vertexOffset   = 0;
}

void TextBatch::Set(const ColoredStrings& text)
{
    this->Set(text, -1.0f, Font::ALIGN_MAX_ENUM);
}

void TextBatch::Set(const ColoredStrings& strings, float wrap, Font::AlignMode align)
{
    if (strings.empty() || (strings.size() == 1 && strings[0].str.empty()))
        return this->Clear();

    ColoredCodepoints codepoints {};
    GetCodepointsFromString(strings, codepoints);

    this->AddTextData({ codepoints, wrap, align, {}, false, false, Matrix4() });
}

int TextBatch::Add(const ColoredStrings& strings, const Matrix4& matrix)
{
    return this->Addf(strings, -1.0f, Font::ALIGN_MAX_ENUM, matrix);
}

int TextBatch::Addf(const ColoredStrings& strings, float wrap, Font::AlignMode align,
                    const Matrix4& matrix)
{
    ColoredCodepoints codepoints {};
    GetCodepointsFromString(strings, codepoints);

    this->AddTextData({ codepoints, wrap, align, {}, true, true, matrix });

    return (int)this->textData.size() - 1;
}

int TextBatch::GetWidth(int index) const
{
    if (index < 0)
        index = std::max((int)this->textData.size() - 1, 0);

    if (index >= (int)this->textData.size())
        return 0;

    return this->textData[index].textInfo.width;
}

int TextBatch::GetHeight(int index) const
{
    if (index < 0)
        index = std::max((int)this->textData.size() - 1, 0);

    if (index >= (int)this->textData.size())
        return 0;

    return this->textData[index].textInfo.height;
}

void TextBatch::Draw(Graphics<Console::Which>& graphics, const Matrix4& matrix)
{
    if (this->buffer.empty() || this->drawCommands.empty())
        return;

    if (Shader<Console::Which>::IsDefaultActive())
        Shader<Console::Which>::AttachDefault(Shader<>::STANDARD_DEFAULT);

    TextureHandle* firstTexture = nullptr;
    if (!this->drawCommands.empty())
        firstTexture = this->drawCommands[0].texture;

    if (this->font->GetTextureCacheID() != this->textureCacheId)
        this->RegenerateVertices();

    int totalVertices = 0;
    for (const auto& command : drawCommands)
        totalVertices = std::max(command.start + command.count, totalVertices);

    TempTransform transform(graphics, matrix);

    for (const auto& command : drawCommands)
    {
        love::DrawCommand drawCommand(command.count);

        if (!Console::Is(Console::CTR))
        {
            drawCommand.shader = Shader<>::STANDARD_TEXTURE;
            drawCommand.format = CommonFormat::TEXTURE;
        }
        else
            drawCommand.format = CommonFormat::FONT;

        drawCommand.handles = { command.texture };

        transform.TransformXY(std::span(drawCommand.Positions().get(), command.count),
                              std::span(&this->buffer[command.start], command.count));

        drawCommand.FillVertices(&this->buffer[command.start]);
        Renderer<Console::Which>::Instance().Render(drawCommand);
    }
}
