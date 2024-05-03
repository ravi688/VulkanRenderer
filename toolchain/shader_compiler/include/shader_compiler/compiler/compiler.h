/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: compiler.h is a part of VulkanRenderer

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
