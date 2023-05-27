/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: disasm.h is a part of VulkanRenderer

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

#include <disassembler/defines.h>
#include <disassembler/disasm.h>
#include <bufferlib/buffer.h>

BEGIN_CPP_COMPATIBLE

/*	disassembles the V3D shader binary code and puts the disassembled string output to a buffer object 
	bytes: pointer to the V3D shader binary code
	size: size of the bytes array pointed by 'bytes'
	returns a pointer to a BUFFER object (contains null terminated string)
 */
DISASM_API BUFFER* disassemble(const void* bytes, u32 size);

END_CPP_COMPATIBLE
