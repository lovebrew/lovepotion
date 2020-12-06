#include "common/runtime.h"
#include "objects/text/text.h"

#include "modules/graphics/graphics.h"

using namespace love;

Text::Text(Font * font, const std::vector<Font::ColoredString> & text) : common::Text(font, text),
                                                                         vertexOffset(0),
                                                                         textureCacheId(-1)
{}

Text::~Text()
{}

void Text::RegenerateVertices()
{
    if (this->font->GetTextureCacheID() != this->textureCacheId)
    {
        std::vector<TextData> data = this->textData;
        this->Clear();

        for (const TextData & text : this->textData)
            this->AddTextData(text);

        this->textureCacheId = this->font->GetTextureCacheID();
    }
}

void Text::UploadVertices(const std::vector<vertex::GlyphVertex> & vertices, size_t vertoffset)
{
    size_t offset = vertoffset * sizeof(vertex::GlyphVertex);
    std::copy(vertices.begin(), vertices.begin() + offset, this->vertexBuffer);
}

void Text::AddTextData(const TextData & text)
{
    std::vector<vertex::GlyphVertex> vertices;
    std::vector<Font::DrawCommand> commands;

    Font::TextInfo info;

    Colorf constantColor = Colorf(1.0f, 1.0f, 1.0f, 1.0f);

    if (text.align == Font::ALIGN_MAX_ENUM)
        commands = this->font->GenerateVertices(text.codepoints, constantColor, vertices, 0.0f, Vector2(0.0f, 0.0f), &info);
    else
        commands = this->font->GenerateVerticesFormatted(text.codepoints, constantColor, text.wrap, text.align, vertices, &info);

    size_t offset = this->vertexOffset;

    if (!text.appendVertices)
    {
        offset = 0;
        this->vertexOffset = 0;
        commands.clear();
        textData.clear();
    }

    if (vertices.empty())
        return;

    if (text.useMatrix)
        text.matrix.TransformXY(&vertices[0], &vertices[0], vertices.size());

    this->UploadVertices(vertices, offset);

    if (!commands.empty())
    {
        for (Font::DrawCommand & command : commands)
            command.startVertex += offset;

        auto first = commands.begin();

        if (!commands.empty())
        {
            auto last = commands.back();

            if (last.texture == first->texture && (last.startVertex + last.vertexCount) == first->startVertex)
            {
                commands.back().vertexCount += first->vertexCount;
                ++first;
            }
        }

        this->drawCommands.insert(this->drawCommands.end(), first, commands.end());
    }

    this->vertexOffset = offset + vertices.size();

    this->textData.push_back(textData);
    this->textData.back().textInfo = info;

    if (this->font->GetTextureCacheID() != this->textureCacheId)
        this->RegenerateVertices();
}

void Text::Set(const std::vector<Font::ColoredString> & text)
{
    return Set(text, -1.0f, Font::ALIGN_MAX_ENUM);
}

void Text::Set(const std::vector<Font::ColoredString> & text, float wrap, Font::AlignMode align)
{
    if (text.empty() || text.size() == 1 && text[0].string.empty())
        return this->Clear();

    Font::ColoredCodepoints codepoints;
    Font::GetCodepointsFromString(text, codepoints);

    this->AddTextData({codepoints, wrap, align, {}, false, false, Matrix4()});
}

int Text::Add(const std::vector<Font::ColoredString> & text, const Matrix4 & localTransform)
{
    return this->Addf(text, -1.0f, Font::ALIGN_MAX_ENUM, localTransform);
}

int Text::Addf(const std::vector<Font::ColoredString> & text, float wrap, Font::AlignMode align, const Matrix4 & localTransform)
{
    Font::ColoredCodepoints codepoints;
    Font::GetCodepointsFromString(text, codepoints);

    this->AddTextData({codepoints, wrap, align, {}, true, true, localTransform});

    return (int)this->textData.size() - 1;
}

void Text::Clear()
{
    this->textData.clear();
    this->drawCommands.clear();
    this->textureCacheId = this->font->GetTextureCacheID();
    this->vertexOffset = 0;
}

void Text::SetFont(Font * font)
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

void Text::Draw(Graphics * gfx, const Matrix4 & localTransform)
{
    if (this->drawCommands.empty())
        return;

    if (this->font->GetTextureCacheID() != this->textureCacheId)
        this->RegenerateVertices();

    int totalVertices = 0;
    for (const Font::DrawCommand & command : this->drawCommands)
        totalVertices = std::max(command.startVertex + command.vertexCount, totalVertices);

    Graphics::TempTransform transform(gfx, localTransform);

    for (const Font::DrawCommand & command : this->drawCommands)
    {
        size_t vertexCount = command.vertexCount;

        std::vector<vertex::Vertex> verts = vertex::GenerateTextureFromGlyphs(vertexData.data(), vertexCount);
        dk3d.RenderTexture(command.texture->GetHandle(), verts.data(), vertexCount * sizeof(*verts.data()), vertexCount);
    }
}