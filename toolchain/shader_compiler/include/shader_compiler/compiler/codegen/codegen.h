#pragma once

#include <shader_compiler/defines.h>
#include <phymac_parser/v3d_generic.h>

#include <shader_compiler/compiler/compiler_ctx.h>
#include <shader_compiler/compiler/codegen/codegen_buffer.h>


SC_API void codegen(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer);
