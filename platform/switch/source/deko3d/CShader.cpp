/*
** Sample Framework for deko3d Applications
**   CShader.cpp: Utility class for loading shaders from the filesystem
*/
#include "deko3d/CShader.h"

bool CShader::load(CMemPool& pool, const void* buffer, size_t size)
{
    return false;
}

bool CShader::load(CMemPool& pool, const char* path)
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
        goto _fail0;

    controlMemory = malloc(header.control_sz);
    if (!controlMemory)
        goto _fail0;

    rewind(file);

    if (!fread(controlMemory, header.control_sz, 1, file))
        goto _fail1;

    m_codemem = pool.allocate(header.code_sz, DK_SHADER_CODE_ALIGNMENT);
    if (!m_codemem)
        goto _fail1;

    if (!fread(m_codemem.getCpuAddr(), header.code_sz, 1, file))
        goto _fail2;

    dk::ShaderMaker { m_codemem.getMemBlock(), m_codemem.getOffset() }
        .setControl(controlMemory)
        .setProgramId(0)
        .initialize(m_shader);

    free(controlMemory);
    fclose(file);

    return true;

_fail2:
    m_codemem.destroy();
_fail1:
    free(controlMemory);
_fail0:
    fclose(file);
    return false;
}
