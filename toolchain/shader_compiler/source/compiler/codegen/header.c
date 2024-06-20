/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: header.c is a part of VulkanRenderer

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

#define COMMON_PORTABLE_STDLIB /* for portable _strtoui64 */
#include <common/defines.h>

#include <shader_compiler/compiler/compiler.h>
#include <shader_compiler/compiler/codegen/header.h>
#include <shader_compiler/compiler/codegen/codegen_buffer.h>
#include <shader_compiler/utilities/string.h>
#include <shader_compiler/debug.h>
#include <phymac_parser/string.h>
#include <stdlib.h>

/* adding due to ucrt64 compilation issues */
#include <stdio.h>

SC_API const char* write_header(compiler_ctx_t* ctx, const char* start, const char* const end)
{
	codegen_buffer_t* writer = ctx->codegen_buffer;

	codegen_buffer_write_string(writer, ".main", SB_HDR_STR);

	/* write offset of .udat section in the .main section so the dissembler can read udats */
	AUTO udat_addr = codegen_buffer_get_end_address(writer, ".udat");
	codegen_buffer_write_pointer(writer, ".main", udat_addr);

	AUTO compiler_cmd_cnt_addr = codegen_buffer_alloc_u32(writer, ".main");

	bool is_sl = false;

	// skip the whitespaces
	start = skip_ws(start, end);
	u32 count = 0;
	while(*start == '#')
	{
		start++;
		while(*start != '\n')
		{
			// skip the spaces and tabs but not the newline as it is a delimiter for a preprocessor line
			start = skip(start, " \t\r", end);

			// command is for shader binary generation
			if(safe_strncmp(start, "sb", 2) == 0)
			{
				codegen_buffer_write_u32(writer, ".main", (u32)CMPLR_CMD_CAT_SB);
				start += 2;
				count++;
				is_sl = false;
			}
			// command is for shader language compilation
			else if(safe_strncmp(start, "sl", 2) == 0)
			{
				codegen_buffer_write_u32(writer, ".main", (u32)CMPLR_CMD_CAT_SL);
				start += 2;
				count++;
				is_sl = true;
			}
			// version { shader binary generation, shader language generation }
			else if(safe_strncmp(start, "version", 7) == 0)
			{
				start += 7;
				u32 ver = _strtoui64(start, (char**)&start, 10);
				if(ver == 0)
					DEBUG_LOG_FETAL_ERROR("Invalid shader binary or language version");
				codegen_buffer_write_u32(writer, ".main", ver);
				if(is_sl)
					ctx->sl_version = ver;
				else
					ctx->sb_version = ver;
			}
		}
		// skip the new line character
		start++;
	}

	codegen_buffer_set_u32(writer, compiler_cmd_cnt_addr, count);

	return start;
}
