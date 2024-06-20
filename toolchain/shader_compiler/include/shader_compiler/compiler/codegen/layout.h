/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: layout.h is a part of VulkanRenderer

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
#include <shader_compiler/compiler/codegen/codegen_buffer.h>

/* shader stages */
#define VERTEX_BIT 										BIT32(8 + 0)
#define FRAGMENT_BIT 									BIT32(8 + 1)
#define GEOMETRY_BIT 									BIT32(8 + 2)
#define TESSELLATION_BIT 								BIT32(8 + 3)

/* shader resource description types */
#define PUSH_CONSTANT_BIT 								BIT32(16 + 0)
#define OPAQUE_BIT 										BIT32(16 + 1)
#define UNIFORM_BUFFER_BIT 								BIT32(16 + 2)
#define STORAGE_BUFFER_BIT 								BIT32(16 + 3)
#define PER_VERTEX_ATTRIB_BIT 							BIT32(16 + 4)
#define PER_INSTANCE_ATTRIB_BIT 						BIT32(16 + 5)

/* hints */
#define UDAT_BIT   	   									BIT32(31)
#define ARRAY_BIT										BIT32(30)

BEGIN_CPP_COMPATIBLE

/*	parses the string pointed by 'start', ends at 'end', and writes "Properties" or "Layout" block to the codegen buffer 'writer'.
	start: pointer to the start of the string
	end: pointer to the end of the string (must point to null character)
	writer: codgen buffer to which write the layout data to.
	is_write_count: if true, then this function will write the number of layout 'descriptions' found inside the block for which it is called.

	NOTE: the expected string should be as follows:
	-------------------------
	fragment vertex [MATERIAL_SET, MATERIAL_PROPERTIES_BINDING] uniform Parameters 
	{
		//...
		float reflectance;
		float roughness;
		//...
	} parameters;

	per_vertex [POSITION_LOCATION, POSITION_BINDING] vec3 position;
	-------------------------
	see: //Documents/V3D/V3DShaderLangSpec.docx
 */
SC_API void write_layout(const char* start, const char* end, codegen_buffer_t* writer, bool is_write_count);


SC_API u32 try_parse_to_u32(const char* str);

END_CPP_COMPATIBLE
