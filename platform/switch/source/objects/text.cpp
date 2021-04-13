#include "objects/text/text.h"

#include "deko3d/deko.h"
#include "modules/graphics/graphics.h"

using namespace love;

Text::Text(Font* font, const std::vector<Font::ColoredString>& text) :
    common::Text(font, text),
    vertexOffset(0),
    textureCacheId(-1)
{
    this->Set(text);
}

Text::~Text()
{}

void Text::RegenerateVertices()
{
    if (this->font->GetTextureCacheID() != this->textureCacheId)
    {
        std::vector<TextData> data = this->textData;
        this->Clear();

        for (const TextData& text : data)
            this->AddTextData(text);

        this->textureCacheId = this->font->GetTextureCacheID();
    }
}

void Text::CopyVertices(const std::vector<vertex::GlyphVertex>& vertices, size_t vertoffset)
{
    size_t offset   = vertoffset * sizeof(vertex::GlyphVertex);
    size_t dataSize = vertices.size() * sizeof(vertex::GlyphVertex);

    if (dataSize > 0 &&
        (this->vertexBuffer.empty() || (offset + dataSize) > this->vertexBuffer.size()))
    {
        size_t newSize = size_t((offset + dataSize) * 1.5);

        if (!this->vertexBuffer.empty())
            newSize = std::max(size_t(this->vertexBuffer.size() * 1.5), newSize);

        this->vertexBuffer.resize(newSize);
    }

    if (!this->vertexBuffer.empty() || dataSize > 0)
        memcpy(this->vertexBuffer.data() + vertoffset, &vertices[0], dataSize);
}

void Text::AddTextData(const Text::TextData& text)
{
    std::vector<vertex::GlyphVertex> vertices;
    std::vector<Font::DrawCommand> commands;

    Font::TextInfo info;

    Colorf constantColor = Colorf(1.0f, 1.0f, 1.0f, 1.0f);

    if (text.align == Font::ALIGN_MAX_ENUM)
        commands = this->font->GenerateVertices(text.codepoints, constantColor, vertices, 0.0f,
                                                Vector2(0.0f, 0.0f), &info);
    else
        commands = this->font->GenerateVerticesFormatted(text.codepoints, constantColor, text.wrap,
                                                         text.align, vertices, &info);

    size_t offset = this->vertexOffset;

    if (!text.appendVertices)
    {
        offset             = 0;
        this->vertexOffset = 0;
        this->drawCommands.clear();
        this->textData.clear();
    }

    if (vertices.empty())
        return;

    if (text.useMatrix)
        text.matrix.TransformXY(&vertices[0], &vertices[0], vertices.size());

    this->CopyVertices(vertices, offset);

    if (!commands.empty())
    {
        for (Font::DrawCommand& command : commands)
            command.startVertex += offset;

        auto firstCmd = commands.begin();

        if (!this->drawCommands.empty())
        {
            auto lastCmd = this->drawCommands.back();

            if (lastCmd.texture == firstCmd->texture &&
                (lastCmd.startVertex + lastCmd.vertexCount) == firstCmd->startVertex)
            {
                this->drawCommands.back().vertexCount += firstCmd->vertexCount;
                ++firstCmd;
            }
        }

        this->drawCommands.insert(this->drawCommands.end(), firstCmd, commands.end());
    }

    this->vertexOffset = offset + vertices.size();

    this->textData.push_back(text);
    this->textData.back().textInfo = info;

    if (this->font->GetTextureCacheID() != this->textureCacheId)
        this->RegenerateVertices();
}

void Text::Set(const std::vector<Font::ColoredString>& text)
{
    return Set(text, -1.0f, Font::ALIGN_MAX_ENUM);
}

void Text::Set(const std::vector<Font::ColoredString>& text, float wrap, Font::AlignMode align)
{
    if (text.empty() || (text.size() == 1 && text[0].string.empty()))
        return this->Clear();

    Font::ColoredCodepoints codepoints;
    Font::GetCodepointsFromString(text, codepoints);

    this->AddTextData({ codepoints, wrap, align, {}, false, false, Matrix4() });
}

int Text::Add(const std::vector<Font::ColoredString>& text, const Matrix4& localTransform)
{
    return this->Addf(text, -1.0f, Font::ALIGN_MAX_ENUM, localTransform);
}

int Text::Addf(const std::vector<Font::ColoredString>& text, float wrap, Font::AlignMode align,
               const Matrix4& localTransform)
{
    Font::ColoredCodepoints codepoints;
    Font::GetCodepointsFromString(text, codepoints);

    this->AddTextData({ codepoints, wrap, align, {}, true, true, localTransform });

    return (int)this->textData.size() - 1;
}

void Text::Clear()
{
    this->textData.clear();
    this->drawCommands.clear();
    this->textureCacheId = this->font->GetTextureCacheID();
    this->vertexOffset   = 0;
}

void Text::SetFont(Font* font)
{
    this->font.Set(font);
    this->textureCacheId = -1;
    this->RegenerateVertices();
}

int Text::GetWidth(int index) const
{
    if (index < 0)
        index = std::max((int)this->textData.size() - 1, 0);

    if (index >= (int)this->textData.size())
        return 0;

    return this->textData[index].textInfo.width;
}

int Text::GetHeight(int index) const
{
    if (index < 0)
        index = std::max((int)this->textData.size() - 1, 0);

    if (index >= (int)this->textData.size())
        return 0;

    return this->textData[index].textInfo.height;
}

void Text::Draw(Graphics* gfx, const Matrix4& localTransform)
{
    if (this->drawCommands.empty() || this->vertexBuffer.empty())
        return;

    if (this->font->GetTextureCacheID() != this->textureCacheId)
        this->RegenerateVertices();

    int totalVertices = 0;
    for (const Font::DrawCommand& command : this->drawCommands)
        totalVertices = std::max(command.startVertex + command.vertexCount, totalVertices);

    Graphics::TempTransform transform(gfx, localTransform);

    for (const Font::DrawCommand& command : this->drawCommands)
    {
        size_t vertexCount = command.vertexCount;
        vertex::GlyphVertex vertexData[command.vertexCount];

        memcpy(vertexData, this->vertexBuffer.data(), sizeof(vertex::GlyphVertex) * vertexCount);
        transform.TransformXY(vertexData, this->vertexBuffer.data(), vertexCount);

        std::vector<vertex::Vertex> verts =
            vertex::GenerateTextureFromGlyphs(vertexData, command.vertexCount);
        ::deko3d::Instance().RenderTexture(command.texture->GetHandle(), verts.data(), vertexCount);
    }
}
