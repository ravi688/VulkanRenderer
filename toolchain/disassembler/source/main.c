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

#include <disassembler/disasm.h>
#include <disk_manager/file_reader.h>
#include <disk_manager/file_writer.h>

#include <disassembler/debug.h>
#include <disassembler/assert.h>

#include <stdio.h>
#include <stdlib.h>

NO_RETURN_FUNCTION static void display_help_and_exit(const char* application_name)
{
	printf(
		"%s: Help\n"
		"\t%s <input_file_name> \n"
		, application_name, application_name
	);
	exit(0);
}

#define cmd_args_error(...) __cmd_args_error(__LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
NO_RETURN_FUNCTION static void __cmd_args_error(u32 line, const char* function, const char* file, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	debug_logv("[Error] [Cmd Args] ", line, function, file, format, args);
	va_end(args);
	display_help_and_exit("disasm");
}

int main(int argc, char** argv)
{
	if(argc < 2)
		cmd_args_error("No enough arguments are given");

	BUFFER* bin = load_binary_from_file(argv[1]);
	BUFFER* str = disassemble(buf_get_ptr(bin), buf_get_element_count(bin));
	if(argc > 2)
		write_text_to_file(argv[2], buf_get_ptr(str));
	else
		puts(CAST_TO(const char*, buf_get_ptr(str)));

	debug_log_info("disassembly success");

	buf_free(bin);
	buf_free(str);
	return 0;
}
