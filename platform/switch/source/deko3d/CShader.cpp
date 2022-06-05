/*
** Sample Framework for deko3d Applications
**   CShader.cpp: Utility class for loading shaders from the filesystem
*/
#include "deko3d/CShader.h"
#include "deko3d/deko.h"

bool CShader::load(const void* buffer, size_t size)
{
    return false;
}

bool CShader::load(const char* path)
{
    FILE* file;
    DkshHeader header;
    void* controlMemory;

    m_codemem.destroy();

    if (path == nullptr)
        return false;

    file = fopen(path, "rb");
    if (!file)
        return false;

    if (!fread(&header, sizeof(header), 1, file))
    {
        fclose(file);
        return false;
    }

    controlMemory = malloc(header.control_sz);
    if (!controlMemory)
    {
        fclose(file);

        return false;
    }

    rewind(file);

    if (!fread(controlMemory, header.control_sz, 1, file))
    {
        free(controlMemory);
        fclose(file);

        return false;
    }

    m_codemem = ::deko3d::Instance().AllocatePool(deko3d::MEMPOOL_CODE, header.code_sz,
                                                  DK_SHADER_CODE_ALIGNMENT);

    if (!m_codemem)
    {
        free(controlMemory);
        fclose(file);

        return false;
    }

    if (!fread(m_codemem.getCpuAddr(), header.code_sz, 1, file))
    {
        free(controlMemory);
        fclose(file);
        m_codemem.destroy();

        return false;
    }

    dk::ShaderMaker { m_codemem.getMemBlock(), m_codemem.getOffset() }
        .setControl(controlMemory)
        .setProgramId(0)
        .initialize(m_shader);

    free(controlMemory);
    fclose(file);

    return true;
}
