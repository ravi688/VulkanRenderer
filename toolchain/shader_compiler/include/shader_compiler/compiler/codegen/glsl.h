/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: glsl.h is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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
#include <shader_compiler/compiler/codegen/codegen_buffer.h>
#include <shader_compiler/compiler/compiler_ctx.h>

BEGIN_CPP_COMPATIBLE

/*	parses the string pointed by 'start', ends at 'end', and writes "GLSL" block to the codegen buffer 'writer'
	start: pointer to the stsart of the string
	end: pointer to the end of the string (must point to null character)
	writer: codgen buffer to which write the "GLSL" block to.
	ctx: the pointer to the compiler_ctx_t object

	NOTE: the expected string should be as follows:
	-------------------------
	#stage vertex

	<vertex GLSL source>

	#stage fragment

	<fragment GLSL source>
	-------------------------
	see: //Documents/V3D/V3DShaderLangSpec.docx
 */
SC_API void write_glsl(const char* start, const char* end, codegen_buffer_t* writer, compiler_ctx_t* ctx);

END_CPP_COMPATIBLE
