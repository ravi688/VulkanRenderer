#pragma once

#include <shader_compiler/defines.h>
#include <shader_compiler/compiler/compiler_ctx.h>
#include <bufferlib/buffer.h>

/*
	Compiles given source code (string) of length 'length' (without null character)
	into shader binary and returns a BUFFER object ptr containing the compiled shader
	binary data.
 */
SC_API BUFFER* sc_compile(compiler_ctx_t* ctx);
