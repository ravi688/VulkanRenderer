/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: codegen.h is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/

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
