#pragma once

#include <shader_compiler/defines.h>
#include <shader_compiler/compiler/compiler_ctx.h>
#include <phymac_parser/v3d_generic.h>

BEGIN_CPP_COMPATIBLE

/*	performs syntax checking and gives diagnostic messages
	node: pointer to the "Shader" node
	ctx: compiler context (contains the grammer data for the V3D shader lang)
 */
SC_API void syntax(v3d_generic_node_t* node, compiler_ctx_t* ctx);

END_CPP_COMPATIBLE
