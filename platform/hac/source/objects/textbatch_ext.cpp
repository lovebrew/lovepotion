#include <common/matrix_ext.hpp>

#include <objects/font_ext.hpp>

#include <objects/textbatch_ext.hpp>
#include <objects/texture_ext.hpp>

#include <objects/shader_ext.hpp>

#include <utilities/temptransform.hpp>

using namespace love;

TextBatch<Console::HAC>::TextBatch(Font<Console::HAC>* font, const Font<>::ColoredStrings& text) :
    TextBatch<Console::ALL>(font),
    vertexBuffer {},
    vertexOffset(0),
    textureCacheId((uint32_t)-1)
{
    this->Set(text);
}

TextBatch<Console::HAC>::~TextBatch()
{}

void TextBatch<Console::HAC>::UploadVertices(const std::vector<vertex::Vertex>& vertices,
                                             size_t vertOffset)
{
    size_t offset   = vertOffset * sizeof(vertex::Vertex);
    size_t dataSize = vertices.size() * sizeof(vertex::Vertex);

    if (dataSize > 0 &&
        (this->vertexBuffer.empty() || (offset + dataSize) > this->vertexBuffer.size()))
    {
        size_t newSize = size_t((offset + dataSize) * 1.5);

        if (!this->vertexBuffer.empty())
            newSize = std::max(size_t(this->vertexBuffer.size() * 1.5), newSize);

        this->vertexBuffer.resize(newSize);
    }

    if (!this->vertexBuffer.empty() || dataSize > 0)
        std::memcpy(this->vertexBuffer.data() + vertOffset, &vertices[0], dataSize);
}

void TextBatch<Console::HAC>::RegenerateVertices()
{
    if (this->font->GetTextureCacheID() != this->textureCacheId)
    {
        std::vector<TextData> data = this->textData;
        this->Clear();

        for (const auto& t : data)
            this->AddTextData(t);

        this->textureCacheId = this->font->GetTextureCacheID();
    }
}

void TextBatch<Console::HAC>::AddTextData(const TextBatch<Console::HAC>::TextData& data)
{
    std::vector<vertex::Vertex> vertices;
    std::vector<Font<Console::HAC>::DrawCommand<>> newCommands;

    Font<Console::HAC>::TextInfo info;
    Color constantColor(1.0f, 1.0f, 1.0f, 1.0f);

    if (data.align == Font<>::ALIGN_MAX_ENUM)
        newCommands = this->font->GenerateVertices(data.codepoints, constantColor, vertices, 0.0f,
                                                   Vector2(0.0f, 0.0f), &info);
    else
        newCommands = this->font->GenerateVerticesFormatted(data.codepoints, constantColor,
                                                            data.wrap, data.align, vertices, &info);

    size_t vertOffset = this->vertexOffset;

    if (!data.appendVertices)
    {
        vertOffset         = 0;
        this->vertexOffset = 0;
        this->drawCommands.clear();
        this->textData.clear();
    }

    if (data.useMatrix && !vertices.empty())
        data.matrix.TransformXYVertPure(&vertices[0], &vertices[0], (int)vertices.size());

    this->UploadVertices(vertices, vertOffset);

    if (!newCommands.empty())
    {
        for (auto& command : newCommands)
            command.start += vertOffset;

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

    this->vertexOffset = vertOffset + vertices.size();

    this->textData.push_back(data);
    this->textData.back().textInfo = info;

    if (this->font->GetTextureCacheID() != this->textureCacheId)
        this->RegenerateVertices();
}

void TextBatch<Console::HAC>::Clear()
{
    this->textData.clear();
    this->drawCommands.clear();
    this->textureCacheId = this->font->GetTextureCacheID();
    this->vertexOffset   = 0;
}

void TextBatch<Console::HAC>::Set(const Font<>::ColoredStrings& text)
{
    this->Set(text, -1.0f, Font<>::ALIGN_MAX_ENUM);
}

void TextBatch<Console::HAC>::Set(const Font<>::ColoredStrings& text, float wrap,
                                  Font<>::AlignMode mode)
{
    if (text.empty() || (text.size() == 1 && text[0].string.empty()))
        return this->Clear();

    Font<Console::HAC>::ColoredCodepoints codepoints {};
    Font<Console::HAC>::GetCodepointsFromString(text, codepoints);

    this->AddTextData({ codepoints, wrap, mode, {}, false, false, Matrix4<Console::HAC>() });
}

int TextBatch<Console::HAC>::Add(const Font<>::ColoredStrings& text,
                                 const Matrix4<Console::HAC>& transform)
{
    return this->Addf(text, -1.0f, Font<>::ALIGN_MAX_ENUM, transform);
}

int TextBatch<Console::HAC>::Addf(const Font<>::ColoredStrings& text, float wrap,
                                  Font<>::AlignMode mode, const Matrix4<Console::HAC>& transform)
{
    Font<Console::HAC>::ColoredCodepoints codepoints {};
    Font<Console::HAC>::GetCodepointsFromString(text, codepoints);

    this->AddTextData({ codepoints, wrap, mode, {}, true, true, transform });

    return (int)this->textData.size();
}

void TextBatch<Console::HAC>::Draw(Graphics<Console::HAC>& graphics,
                                   const Matrix4<Console::Which>& localTransform)
{
    if (this->vertexBuffer.empty() || this->drawCommands.empty())
        return;

    if (Shader<Console::HAC>::IsDefaultActive())
        Shader<Console::HAC>::AttachDefault(Shader<>::STANDARD_DEFAULT);

    Texture<Console::HAC>* firstTexture = nullptr;
    if (!this->drawCommands.empty())
        firstTexture = this->drawCommands[0].texture;

    if (this->font->GetTextureCacheID() != this->textureCacheId)
        this->RegenerateVertices();

    int totalverts = 0;
    for (const auto& cmd : drawCommands)
        totalverts = std::max(cmd.start + cmd.count, totalverts);

    TempTransform transform(graphics, localTransform);

    for (const auto& cmd : drawCommands)
    {
        love::DrawCommand<Console::HAC> drawCommand(cmd.count);
        drawCommand.shader  = Shader<>::STANDARD_TEXTURE;
        drawCommand.format  = vertex::CommonFormat::TEXTURE;
        drawCommand.type    = vertex::PRIMITIVE_QUADS;
        drawCommand.handles = { cmd.texture };

        transform.TransformXY(drawCommand.Positions().get(), this->vertexBuffer.data(), cmd.count);

        drawCommand.FillVertices(this->vertexBuffer.data());
        Renderer<Console::HAC>::Instance().Render(drawCommand);
    }
}

void TextBatch<Console::HAC>::SetFont(Font<Console::HAC>* font)
{
    TextBatch<>::SetFont(font);
    this->textureCacheId = (uint32_t)-1;
    this->RegenerateVertices();
}

int TextBatch<Console::HAC>::GetWidth(int index) const
{
    if (index < 0)
        index = std::max((int)this->textData.size() - 1, 0);

    if (index >= (int)this->textData.size())
        return 0;

    return this->textData[index].textInfo.width;
}

int TextBatch<Console::HAC>::GetHeight(int index) const
{
    if (index < 0)
        index = std::max((int)this->textData.size() - 1, 0);

    if (index >= (int)this->textData.size())
        return 0;

    return this->textData[index].textInfo.width;
}
