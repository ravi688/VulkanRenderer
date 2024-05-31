/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: header.h is a part of VulkanRenderer

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
#include <shader_compiler/compiler/compiler.h>

#define SB_HDR_STR "V3D Shader Binary"


/* compiler command category */
typedef enum cmplr_cmd_cat_t
{
	/* command is related to shader binary generation */
	CMPLR_CMD_CAT_SB = 1UL,
	/* command is related to shader language compilation */
	CMPLR_CMD_CAT_SL
} cmplr_cmd_cat_t;

/* shader binary version
   i.e #sb version 2022
 */
typedef enum sb_version_t
{
	SB_VERSION_2021 = 2021UL,
	SB_VERSION_2022
} sb_version_t;

/* shader language version
   i.e #sl version 2022
*/
typedef enum sl_version_t
{
	SL_VERSION_2021 = 2021UL,
	SL_VERSION_2022 = 2022UL,
	SL_VERSION_2023 = 2023UL
} sl_version_t;

/*	parses the string pointed by 'start', ends at 'end', and writes the file header to the codegen buffer 'writer'
	start: pointer to the stsart of the string
	end: pointer to the end of the string (must point to null character)
	writer: codgen buffer to which write the file header to.

	NOTE: the expected string should be as follows:
	-------------------------
	#sl version 2022
	#sl version 2022
	-------------------------
	the above string must appear before the "Shader" block
	see: //Documents/V3D/V3DShaderLangSpec.docx
 */
SC_API const char* write_header(compiler_ctx_t* ctx, const char* start, const char* const end);
