#include <disassembler/disasm.h>

DISASM_API BUFFER* disassemble(const void* bytes, u32 size)
{
	BUFFER* str = BUFcreate(NULL, sizeof(char), 1024, 0);
	buf_push_null(str);
	return str;
}
