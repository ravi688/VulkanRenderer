
#pragma once

#include <disassembler/defines.h>
#include <disassembler/disasm.h>
#include <bufferlib/buffer.h>

BEGIN_CPP_COMPATIBLE

DISASM_API BUFFER* disassemble(const void* bytes, u32 size);

END_CPP_COMPATIBLE
