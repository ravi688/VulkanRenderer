#pragma once

#include <shader_compiler/defines.h>
#include <phymac_parser/v3d_generic.h>

#include <shader_compiler/compiler/compiler_ctx.h>
#include <shader_compiler/compiler/codegen/codegen_buffer.h>

BEGIN_CPP_COMPATIBLE

/*  generates shader binary (writes "Shader" block into codegen buffer 'writer')
 *  node: shader node (v3d_generic_node_t)
 *        this node must be a Shader node containing valid "Properties", "Layout", "RenderPass" blocks,
 *        however if this node doesn't meet these requirements then resulting binary will be empty.
 *  ctx: compiler context, as that contains some data used during codegen phase
 *  writer: buffer to write the compiled binary in.
 *
 *  NOTE: it allocates heap memory internally
 *  see: Documents/V3D/V3DShaderBinarySpec.docx for binary format spec.
 */
SC_API void codegen(v3d_generic_node_t* node, compiler_ctx_t* ctx, codegen_buffer_t* writer);

END_CPP_COMPATIBLE
