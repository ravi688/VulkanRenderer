#pragma once

#include <shader_compiler/defines.h>
#include <shader_compiler/compiler/codegen/codegen_buffer.h>
#include <shader_compiler/compiler/compiler_ctx.h>

SC_API void write_glsl(const char* start, const char* end, codegen_buffer_t* writer, compiler_ctx_t* ctx);
