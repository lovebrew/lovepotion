/*
** Sample Framework for deko3d Applications
**   CShader.cpp: Utility class for loading shaders from the filesystem
*/
#include "deko3d/CShader.h"

struct DkshHeader
{
    uint32_t magic; // DKSH_MAGIC
    uint32_t header_sz; // sizeof(DkshHeader)
    uint32_t control_sz;
    uint32_t code_sz;
    uint32_t programs_off;
    uint32_t num_programs;
};

bool CShader::load(CMemPool & pool, const char * path)
{
    FILE * f;
    DkshHeader hdr;
    void * ctrlmem;

    m_codemem.destroy();

    f = fopen(path, "rb");
    if (!f)
        return false;

    if (!fread(&hdr, sizeof(hdr), 1, f))
        goto _fail0;

    ctrlmem = malloc(hdr.control_sz);
    if (!ctrlmem)
        goto _fail0;

    rewind(f);
    if (!fread(ctrlmem, hdr.control_sz, 1, f))
        goto _fail1;

    m_codemem = pool.allocate(hdr.code_sz, DK_SHADER_CODE_ALIGNMENT);
    if (!m_codemem)
        goto _fail1;

    if (!fread(m_codemem.getCpuAddr(), hdr.code_sz, 1, f))
        goto _fail2;

    dk::ShaderMaker{m_codemem.getMemBlock(), m_codemem.getOffset()}
        .setControl(ctrlmem)
        .setProgramId(0)
        .initialize(m_shader);

    free(ctrlmem);
    fclose(f);

    return true;

_fail2:
    m_codemem.destroy();
_fail1:
    free(ctrlmem);
_fail0:
    fclose(f);
    return false;
}

bool CShader::loadMemory(CMemPool & pool, void * buffer, size_t size)
{
    DkshHeader hdr;

    m_codemem.destroy();

    // Copy the dksh buffer to the struct
    if (!memcpy(&hdr, buffer, sizeof(hdr)))
        goto _fail0;

    /*
    ** Allocate the code memory from the pool based on `code_sz`
    ** this must be aligned to DK_SHADER_CODE_ALIGNMENT
    */
    m_codemem = pool.allocate(hdr.code_sz, DK_SHADER_CODE_ALIGNMENT);
    if (!m_codemem)
        goto _fail2;

    /*
    ** Copy the buffer's shader code to the CPU address of the code memory
    ** Make sure to add hdr.control_sz to the buffer for the *correct* offset
    ** As though the buffer were being dealt with like an fread
    */
    if (!memcpy(m_codemem.getCpuAddr(), (char *)buffer + hdr.control_sz, hdr.code_sz))
        goto _fail2;

    /*
    ** Create a shader based on m_codemem's memory block and offset
    ** Set its control to control memory from the dksh and programId
    ** to zero, as that is the entry point of the program
    */
    dk::ShaderMaker{m_codemem.getMemBlock(), m_codemem.getOffset()}
        .setControl(buffer)
        .setProgramId(0)
        .initialize(m_shader);

    return true;

_fail2:
    m_codemem.destroy();
_fail0:
    return false;
}