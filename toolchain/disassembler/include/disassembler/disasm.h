
#pragma once

#include <disassembler/defines.h>
#include <disassembler/disasm.h>
#include <bufferlib/buffer.h>

BEGIN_CPP_COMPATIBLE

/*	disassembles the V3D shader binary code and puts the disassembled string output to a buffer object 
	bytes: pointer to the V3D shader binary code
	size: size of the bytes array pointed by 'bytes'
	returns a pointer to a BUFFER object (contains null terminated string)
 */
DISASM_API BUFFER* disassemble(const void* bytes, u32 size);

END_CPP_COMPATIBLE
