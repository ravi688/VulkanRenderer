#pragma once

#include <shader_compiler/defines.h>
#include <shader_compiler/compiler/codegen/codegen_buffer.h>
#include <shader_compiler/compiler/compiler_ctx.h>
#include <bufferlib/buffer.h>
#include <phymac_parser/v3d_generic.h>

/*
	Compiles given source code (string) of length 'length' (without null character)
	into shader binary and returns a BUFFER object ptr containing the compiled shader
	binary data.
 */
SC_API BUFFER* sc_compile(const char* source, u32 length);

/*
	Similar to sc_compile(...) but it also loads a given file from 'file_path' into the
	memory and calls sc_compile internally after that.
 */
SC_API BUFFER* sc_load_and_compile(const char* file_path);
