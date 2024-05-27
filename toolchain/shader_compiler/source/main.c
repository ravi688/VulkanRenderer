/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: main.c is a part of VulkanRenderer

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


#include <shader_compiler/compiler/compiler.h>
#include <shader_compiler/utilities/string.h>
#include <shader_compiler/defines.h>
#include <shader_compiler/assert.h>
#include <disk_manager/file_writer.h>
#include <disk_manager/file_reader.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef PLATFORM_WINDOWS
#	include <windows.h> // GetCurrentDirectory
#elif defined(PLATFORM_LINUX)
#	include <unistd.h> // getcwd
# 	include <linux/limits.h> // PATH_MAX
#endif

void display_help_and_exit(const char* application_name)
{
	printf(
		"%s: Help\n"
		"\t%s [-I<include_path>] <input_file_name> <output_file_name>\n"
		"\tor\n"
		"\t%s [-I<include_path>] <input_file_name>\n"
		, application_name, application_name, application_name
	);
	exit(0);
}

typedef struct cmd_args_parse_result_t
{
	/* current working directory */
	const char* cwd;
	/* path the vsc executable file */
	const char* exe_path;
	/* unordered list of include path to be searched for when include a file with <> brackets */
	BUFFER include_paths;
	/* ordered list of paths (it includes src path and sb path*/
	BUFFER paths;
} cmd_args_parse_result_t;

#define cmd_args_error(...) __cmd_args_error(__LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
static void __cmd_args_error(u32 line, const char* function, const char* file, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	debug_logv("[Error] [Cmd Args] ", line, function, file, format, args);
	va_end(args);
	display_help_and_exit("vsc");
}

#ifdef PLATFORM_WINDOWS
/* 	returns the current working directory
	NOTE: windows only
 */
static const char* get_cwd()
{
	DWORD length = GetCurrentDirectory(0, NULL);
	_assert(length > 0);
	LPTSTR buffer  = CAST_TO(LPTSTR, malloc(length));
	DWORD read_length = GetCurrentDirectory(length, buffer);
	_assert(read_length == (length - 1));
	return CAST_TO(const char*, buffer);
}
#elif defined(PLATFORM_LINUX)
static const char* get_cwd()
{
	char* cwd = CAST_TO(char*, malloc(PATH_MAX));
	if(getcwd(cwd, PATH_MAX) != NULL)
		return CAST_TO(const char*, cwd);
	else
		DEBUG_LOG_ERROR("Failed to get current working directory");
	return NULL;
}
#endif

static cmd_args_parse_result_t* parse_cmd_args(const char* const* argv, int arg_count)
{
	cmd_args_parse_result_t* result = CAST_TO(cmd_args_parse_result_t*, malloc(sizeof(cmd_args_parse_result_t)));
	memset(result, 0, sizeof(cmd_args_parse_result_t));

	_ASSERT(arg_count > 1);

	result->cwd = get_cwd();
	result->exe_path = argv[0];
	result->include_paths = buf_new(char*);				// list of null terminated strings
	result->paths = buf_new(char*);						// list of null terminated strings

	for(u32 i = 1; i < arg_count; i++)
	{
		const char* str = argv[i];
		u32 len = strlen(str);

		_ASSERT(len != 0);

		/* option */
		if((len > 0) && (str[0] == '-'))
		{
			/* include path */
			if((len > 1) && (str[1] == 'I'))
			{
				if(len > 2)
				{
					const char* ptr = str + 2;
					buf_push(&result->include_paths, CAST_TO(void*, &ptr));
				}
				else
					cmd_args_error("Invalid include path is given");
			}
			else if(len == 1)
				cmd_args_error("Invalid option is given");
			else
				cmd_args_error("Unrecognized option \"%.*s\"", get_word_length(str + 1, 0), str + 1);
		}
		else
			buf_push(&result->paths, CAST_TO(void*, &str));
	}

	return result;
}

static void destroy_cmd_args_parse_result(cmd_args_parse_result_t* result)
{
	free(CAST_TO(void*, result->cwd));
	buf_free(&result->include_paths);
	buf_free(&result->paths);
	free(result);
}

int main(int arg_count, const char* const* argv)
{
	cmd_args_parse_result_t* result = parse_cmd_args(argv, arg_count);

	u32 path_count = buf_get_element_count(&result->paths);

	_ASSERT(path_count <= 2);

	if(path_count == 0)
		cmd_args_error("No source input found");

	char* sb_path = (path_count > 1) ? CAST_TO(char**, buf_pop_get_ptr(&result->paths))[0] : NULL;
	const char* src_path = CAST_TO(const char* const*, buf_pop_get_ptr(&result->paths))[0];

	if(sb_path == NULL)
	{
		sb_path = CAST_TO(char*, malloc(strlen(src_path) + strlen(".sb") + 1));
		strcpy(sb_path, src_path);
		
		char* ptr = strrchr(sb_path, '.');
		
		/* if the src_path has an extension */
		if(ptr != NULL)
		{
			ptr++;
			strcpy(ptr, "sb");
		}
		else
		{
			/* otherwise just append .sb */
			ptr = sb_path + strlen(sb_path);
			strcpy(ptr, ".sb");
		}
	}

	/* load the source file into memory */
	BUFFER* src = load_text_from_file(src_path);

	if(src == NULL)
	{
		DEBUG_LOG_ERROR("Failed to open the file %s or it is empty", src_path);
		destroy_cmd_args_parse_result(result);
		return 0;
	}

	/* create compiler context */
	sc_compiler_input_t input = 
	{
		.src = buf_get_ptr(src),
		.src_len = buf_get_element_count(src),
		.src_path = src_path,
		.exe_path = result->exe_path,
		.cwd = result->cwd,
		.include_paths = CAST_TO(const char**, buf_get_ptr(&result->include_paths)),
		.include_path_count = CAST_TO(u32, buf_get_element_count(&result->include_paths))
	};
	
	/* kick the compilation  */	
	sc_compiler_output_t output = sc_compile(&input, NULL);
	if(output.is_success)
	{
		debug_log_info("Compiled shader binary info: { size = %llu bytes (%.1f KB) }", output.sb_byte_count, CAST_TO(float, output.sb_byte_count) / 1024);
		/* write the result into disk */
		write_binary_to_file(sb_path, output.sb_bytes, output.sb_byte_count);
	}
	else DEBUG_LOG_ERROR("Failed to compile %s, Reason: %s", src_path, output.log);
	
	sc_compiler_output_destroy(&output, NULL);

	/* release allocated memory */
	buf_free(src);
	destroy_cmd_args_parse_result(result);
}
