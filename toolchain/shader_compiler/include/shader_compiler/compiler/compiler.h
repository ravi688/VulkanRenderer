#pragma once

#include <shader_compiler/defines.h>
#include <shader_compiler/compiler/compiler_ctx.h>
#include <bufferlib/buffer.h>

BEGIN_CPP_COMPATIBLE

/*
	compiles a V3D shader source configured inside the compiler_ctx_t object
	ctx: compilation context (it also includes the source to be compiled)
	returns a pointer to a BUFFER object

	see: Documents/V3D/V3DShaderBinarySpec.docx for format of the compiled binary output.
 */
SC_API BUFFER* sc_compile(compiler_ctx_t* ctx);

END_CPP_COMPATIBLE
