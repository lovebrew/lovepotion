#include <objects/mesh/mesh.hpp>

#include <common/exception.hpp>

#include <objects/texture_ext.hpp>

#include <utilities/log/logfile.hpp>
#include <utilities/temptransform.hpp>

using namespace love;
using namespace love::vertex;

Type Mesh::type("Mesh", &Drawable::type);

Mesh::Mesh(const void* data, size_t dataSize, PrimitiveType mode) :
    mode(mode),
    drawRange {},
    useIndexBuffer(false)
{
    if (dataSize == 0)
        throw Exception("Mesh data size cannot be zero.");

    size_t vertexCount = dataSize / VERTEX_SIZE;
    this->buffer.resize(vertexCount);

    std::memcpy(this->buffer.data(), data, dataSize);

    this->vertexCount  = vertexCount;
    this->vertexStride = VERTEX_SIZE;
}

Mesh::Mesh(int vertexCount, PrimitiveType mode) : mode(mode), drawRange {}, useIndexBuffer(false)
{
    if (vertexCount == 0)
        throw Exception("Mesh vertex count cannot be zero.");

    this->buffer.resize(vertexCount);

    this->vertexCount  = vertexCount;
    this->vertexStride = VERTEX_SIZE;
}

Mesh::~Mesh()
{}

void* Mesh::CheckVertexDataOffset(size_t index, size_t* byteOffset)
{
    if (index >= this->vertexCount)
        throw love::Exception("Invalid vertex index: %d", index + 1);

    if (this->buffer.empty())
        throw love::Exception("Mesh has no vertex data.");

    size_t offset = index * this->vertexStride;
    if (byteOffset != nullptr)
        *byteOffset = offset;

    return this->buffer.data() + offset;
}

size_t Mesh::GetVertexCount() const
{
    return this->vertexCount;
}

size_t Mesh::GetVertexStride() const
{
    return this->vertexStride;
}

std::vector<Vertex> Mesh::GetVertexBuffer() const
{
    return this->buffer;
}

void* Mesh::GetVertexData() const
{
    return (void*)this->buffer.data();
}

void Mesh::Flush()
{}

// TODO
void Mesh::SetVertexMap(const std::vector<uint32_t>& map)
{}

// TODO
void Mesh::SetVertexMap(const void* data, size_t dataSize)
{}

// TODO
void Mesh::SetVertexMap()
{}

// TODO
void Mesh::SetVertexDataModified(size_t byteOffset, size_t dataSize)
{}

bool Mesh::GetVertexMap(std::vector<uint32_t>& map) const
{
    return false;
}

void Mesh::SetIndexBuffer(std::vector<Vertex>& buffer)
{}

std::vector<Vertex> Mesh::GetIndexBuffer() const
{
    return this->indexBuffer;
}

size_t Mesh::GetIndexCount() const
{
    return this->indexCount;
}

void Mesh::SetTexture(Texture<Console::Which>* texture)
{
    this->texture.Set(texture);
}

void Mesh::SetTexture()
{
    this->texture.Set(nullptr);
}

Texture<Console::Which>* Mesh::GetTexture() const
{
    return this->texture.Get();
}

void Mesh::SetDrawMode(PrimitiveType mode)
{
    this->mode = mode;
}

PrimitiveType Mesh::GetDrawMode() const
{
    return this->mode;
}

void Mesh::SetDrawRange(int start, int count)
{
    if (start < 0 || count <= 0)
        throw love::Exception("Invalid draw range: start = %d, count = %d", start, count);

    this->drawRange = Range(start, count);
}

void Mesh::SetDrawRange()
{
    this->drawRange.invalidate();
}

bool Mesh::GetDrawRange(int& start, int& count) const
{
    if (!this->drawRange.isValid())
        return false;

    start = this->drawRange.getOffset();
    count = this->drawRange.getSize();

    return true;
}

void Mesh::Draw(Graphics<Console::Which>& graphics, const Matrix4<Console::Which>& matrix)
{
    this->DrawInternal(graphics, matrix, 1, nullptr, 0);
}

void Mesh::DrawInternal(Graphics<Console::Which>& graphics, const Matrix4<Console::Which>& matrix,
                        int instanceCount, std::vector<Vertex>* indirectArgs, int argsIndex)
{
    if (this->vertexCount <= 0 || (instanceCount <= 0 && indirectArgs == nullptr))
        return;

    if (instanceCount > 1) // TODO is this right?
        throw love::Exception("Instanced drawing is not supported.");

    // TODO what do we do?
    if (indirectArgs != nullptr)
    {
    }

    if (this->mode == PRIMITIVE_TRIANGLE_FAN && this->useIndexBuffer && !this->indexBuffer.empty())
    {
        throw love::Exception(
            "The 'fan' Mesh draw mode cannot be used with an index buffer or vertex map.");
    }

    this->Flush();

    TempTransform transform(graphics, matrix);
    Range range = this->drawRange;

    if (!this->indexBuffer.empty() && (this->indexCount > 0 || indirectArgs != nullptr))
    {
        // TODO: draw indexed
    }
    else if (vertexCount > 0 || indirectArgs != nullptr)
    {
        Range _range(0, this->vertexCount);
        if (range.isValid())
            _range.intersect(range);

        auto shader = Shader<>::STANDARD_TEXTURE;
        if (Console::Is(Console::CTR))
            shader = Shader<>::STANDARD_DEFAULT;

        DrawCommand<Console::Which> command(_range.getSize(), this->mode, shader);
        command.FillVertices(this->buffer.data());

        transform.TransformXYPure(command.vertices.get(), &this->buffer[_range.getOffset()],
                                  command.count);

#if defined(__3DS__)
        if (this->texture != nullptr)
        {
            command.handles = { this->texture->GetHandle() };
            command.format  = CommonFormat::TEXTURE;
        }
#else
        command.handles = { this->texture };
#endif

        command.cullMode = graphics.GetMeshCullMode();

        if (command.count > 0)
            Renderer<Console::Which>::Instance().Render(command);
    }
}