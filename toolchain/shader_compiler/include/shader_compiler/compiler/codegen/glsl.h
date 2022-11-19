#pragma once

#include <shader_compiler/defines.h>
#include <shader_compiler/compiler/codegen/codegen_buffer.h>

SC_API void write_glsl(const char* start, const char* end, codegen_buffer_t* writer);
