#pragma once

#include <shader_compiler/defines.h>
#include <shader_compiler/compiler/codegen/codegen_buffer.h>
#include <shader_compiler/compiler/compiler_ctx.h>

BEGIN_CPP_COMPATIBLE

/*	parses the string pointed by 'start', ends at 'end', and writes "GLSL" block to the codegen buffer 'writer'
	start: pointer to the stsart of the string
	end: pointer to the end of the string (must point to null character)
	writer: codgen buffer to which write the "GLSL" block to.
	ctx: the pointer to the compiler_ctx_t object

	NOTE: the expected string should be as follows:
	-------------------------
	#stage vertex

	<vertex GLSL source>

	#stage fragment

	<fragment GLSL source>
	-------------------------
	see: //Documents/V3D/V3DShaderLangSpec.docx
 */
SC_API void write_glsl(const char* start, const char* end, codegen_buffer_t* writer, compiler_ctx_t* ctx);

END_CPP_COMPATIBLE
